#include <cstdarg>
#include <cstring>
#include <iostream>

#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include "../ast/ast.hh"
#include "../ast/blocks.hh"
#include "../ast/literals.hh"
#include "../ast/methods.hh"
#include "../ast/operators.hh"
#include "../ast/program.hh"
#include "../ast/statements.hh"
#include "../ast/variables.hh"
#include "../exceptions.hh"
#include "codegen.hh"

/*** CodeGenerator::SymbolTable ***/
void CodeGenerator::SymbolTable::block_start() { variables.emplace_back(); }
void CodeGenerator::SymbolTable::block_end() {
  assert(!variables.empty());
  variables.pop_back();
}

void CodeGenerator::SymbolTable::add_variable(std::string id,
                                              llvm::AllocaInst *alloca) {
  variables.back()[id] = alloca;
}
llvm::AllocaInst *CodeGenerator::SymbolTable::lookup_variable(std::string id) {
  for (auto it = variables.rbegin(); it != variables.rend(); it++) {
    if (it->count(id) > 0) {
      return (*it)[id];
    }
  }
  return nullptr;
}

void CodeGenerator::SymbolTable::add_array(std::string id, int length) {
  array_lengths[id] = length;
}
int CodeGenerator::SymbolTable::get_array_len(std::string id) {
  return array_lengths[id];
}

bool CodeGenerator::SymbolTable::is_global_scope() { return variables.empty(); }

/*** CodeGenerator ***/
CodeGenerator::CodeGenerator(std::string name) : builder(context) {
  module = new llvm::Module(name, context);
  has_error = false;
}
CodeGenerator::~CodeGenerator() { delete module; }

void CodeGenerator::add_builtin(std::string name,
                                std::vector<ValueType> _params, ValueType ret) {
  if (module->getFunction(name) != nullptr)
    return;

  llvm::Type *ret_type = get_llvm_type(ret);
  llvm::FunctionType *ftype = llvm::FunctionType::get(ret_type, true);
  llvm::Function::Create(ftype, llvm::Function::ExternalLinkage, name, module);
}

void CodeGenerator::generate(BaseAST &root) {
  // add decl for exit, write_string
  add_builtin("exit", std::vector<ValueType>(1, ValueType::INT),
              ValueType::VOID);
  add_builtin("write_string", std::vector<ValueType>(1, ValueType::STRING),
              ValueType::INT);

  /** generate code **/
  root.accept(*this);
}

void CodeGenerator::print(std::string outf) {
  if (outf != "") {
    std::error_code EC;
    llvm::raw_fd_ostream out(outf, EC, llvm::sys::fs::F_None);
    if (EC) {
      std::cerr << "Error writing to file " << outf << "\n";
      return;
    }
    module->print(out, nullptr);
  } else { // no file provided, write to stdout
    module->print(llvm::outs(), nullptr);
  }
}

llvm::Value *CodeGenerator::get_return_stack_top(bool pop) {
  assert(!return_stack.empty());
  llvm::Value *res = return_stack.top();
  if (pop)
    return_stack.pop();
  return res;
}
llvm::Value *CodeGenerator::get_return(BaseAST &node) {
  node.accept(*this);
  return get_return_stack_top();
}

llvm::Type *CodeGenerator::get_llvm_type(ValueType ty) {
  if (ty == ValueType::INT) {
    return llvm::Type::getInt32Ty(context);
  }
  if (ty == ValueType::BOOL) {
    return llvm::Type::getInt1Ty(context);
  }
  if (ty == ValueType::VOID) {
    return llvm::Type::getVoidTy(context);
  }
  if (ty == ValueType::STRING) {
    return llvm::Type::getInt8PtrTy(context);
  }

  return nullptr;
}

void CodeGenerator::add_runtime_error_inst(int ec, std::string err) {
  {
    llvm::Value *ev = builder.CreateGlobalStringPtr(
        "Runtime error: " + err + "\n", "error_message");
    llvm::Function *func = module->getFunction("write_string");
    std::vector<llvm::Value *> args;
    args.push_back(ev);
    builder.CreateCall(func, args, "errtmp");
  }
  {
    llvm::Function *func = module->getFunction("exit");
    std::vector<llvm::Value *> args;
    args.push_back(llvm::ConstantInt::get(context, llvm::APInt(32, ec)));
    builder.CreateCall(func, args);
  }
  {
    // dummy ret, to avoid IR error
    llvm::Function *func = builder.GetInsertBlock()->getParent();
    if (func->getReturnType()->isVoidTy()) {
      builder.CreateRetVoid();
    } else if (func->getReturnType()->isIntegerTy(32)) {
      builder.CreateRet(llvm::ConstantInt::get(context, llvm::APInt(32, 0)));
    } else if (func->getReturnType()->isIntegerTy(1)) {
      builder.CreateRet(llvm::ConstantInt::get(context, llvm::APInt(1, 0)));
    }
  }
}

void CodeGenerator::error(const std::string &fmt, ...) {
  static const int SIZE = 300;
  std::string err(SIZE, '\0');

  va_list args;
  va_start(args, fmt);
  vsnprintf(&err[0], SIZE, fmt.c_str(), args);
  va_end(args);

  std::cerr << err << '\n';
}

/*** visits: ***/
void CodeGenerator::visit(BaseAST &node) {
  throw invalid_call_error(__PRETTY_FUNCTION__);
}

// literals.hh
void CodeGenerator::visit(LiteralAST &node) {
  throw invalid_call_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(IntegerLiteralAST &node) {
  llvm::Value *value =
      llvm::ConstantInt::get(context, llvm::APInt(32, node.value));
  return_stack.push(value);
}
void CodeGenerator::visit(BooleanLiteralAST &node) {
  llvm::Value *value =
      llvm::ConstantInt::get(context, llvm::APInt(1, node.value));
  return_stack.push(value);
}
void CodeGenerator::visit(StringLiteralAST &node) {
  return_stack.push(builder.CreateGlobalStringPtr(node.value, "literal"));
}

// variables.hh
void CodeGenerator::visit(LocationAST &node) {
  throw invalid_call_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(VariableLocationAST &node) {
  llvm::Value *var = symbol_table.lookup_variable(node.id);
  if (var == nullptr) {
    var = module->getNamedGlobal(node.id);
  }
  if (!node.is_lvalue) {
    var = builder.CreateLoad(var, node.id);
  }
  return_stack.push(var);
}
void CodeGenerator::visit(ArrayLocationAST &node) {
  llvm::Value *var = module->getNamedGlobal(node.id);
  std::vector<llvm::Value *> index;
  index.push_back(llvm::ConstantInt::get(context, llvm::APInt(64, 0)));
  index.push_back(get_return(*node.index_expr));

  llvm::Function *func = builder.GetInsertBlock()->getParent();

  llvm::BasicBlock *lowerBoundPassBB =
      llvm::BasicBlock::Create(context, "array-bound-lower", func);
  llvm::BasicBlock *upperBoundPassBB =
      llvm::BasicBlock::Create(context, "array-bound-upper", func);
  llvm::BasicBlock *errorBB =
      llvm::BasicBlock::Create(context, "array-out-of-bounds", func);

  llvm::Value *lower_bound_cond = builder.CreateICmpSGE(
      index[1], llvm::ConstantInt::get(context, llvm::APInt(32, 0)),
      "array-bound-ge-0");
  builder.CreateCondBr(lower_bound_cond, lowerBoundPassBB, errorBB);
  builder.SetInsertPoint(lowerBoundPassBB);

  int array_len = symbol_table.get_array_len(node.id);
  llvm::Value *upper_bound_cond = builder.CreateICmpSLT(
      index[1], llvm::ConstantInt::get(context, llvm::APInt(32, array_len)),
      "array-bound-lt-size");
  builder.CreateCondBr(upper_bound_cond, upperBoundPassBB, errorBB);

  builder.SetInsertPoint(errorBB);
  add_runtime_error_inst(1, "Array access out of bounds: " + node.id);

  builder.SetInsertPoint(upperBoundPassBB);

  var = builder.CreateGEP(var, index, "array_location");

  if (!node.is_lvalue) {
    var = builder.CreateLoad(var, node.id);
  }

  return_stack.push(var);
}

void CodeGenerator::visit(ArrayAddressAST &node) {
  llvm::Value *var = module->getNamedGlobal(node.id);

  std::vector<llvm::Value *> index;
  index.push_back(llvm::ConstantInt::get(context, llvm::APInt(64, 0)));
  index.push_back(llvm::ConstantInt::get(context, llvm::APInt(64, 0)));

  var = builder.CreateGEP(var, index, "array_address");

  return_stack.push(var);
}

void CodeGenerator::visit(VariableDeclarationAST &node) {
  auto type = get_llvm_type(node.type);
  auto init = llvm::Constant::getNullValue(type);
  if (symbol_table.is_global_scope()) { // global variable
    llvm::GlobalVariable *var = new llvm::GlobalVariable(
        *module, type, false, llvm::GlobalValue::InternalLinkage, nullptr,
        node.id);
    var->setInitializer(init);
  } else { // local/block variable
    llvm::AllocaInst *alloca = builder.CreateAlloca(type, 0, node.id);
    builder.CreateStore(init, alloca);
    symbol_table.add_variable(node.id, alloca);
    return_stack.push(alloca);
  }
}

void CodeGenerator::visit(ArrayDeclarationAST &node) {
  llvm::ArrayType *type =
      llvm::ArrayType::get(get_llvm_type(node.type), node.array_len);
  llvm::GlobalVariable *var = new llvm::GlobalVariable(
      *module, type, false, llvm::GlobalValue::InternalLinkage, nullptr,
      node.id);
  var->setInitializer(llvm::ConstantAggregateZero::get(type));
  symbol_table.add_array(node.id, node.array_len);
}

// operators.hh
void CodeGenerator::visit(UnaryOperatorAST &node) {
  throw invalid_call_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(BinaryOperatorAST &node) {
  throw invalid_call_error(__PRETTY_FUNCTION__);
}

void CodeGenerator::visit(ArithBinOperatorAST &node) {
  llvm::Value *lvalue = get_return(*node.lval);
  llvm::Value *rvalue = get_return(*node.rval);

  llvm::Value *value;
  if (node.op == OperatorType::ADD) {
    value = builder.CreateAdd(lvalue, rvalue, "Add");
  } else if (node.op == OperatorType::SUB) {
    value = builder.CreateSub(lvalue, rvalue, "Sub");
  } else if (node.op == OperatorType::MUL) {
    value = builder.CreateMul(lvalue, rvalue, "Mul");
  } else if (node.op == OperatorType::DIV) {
    value = builder.CreateSDiv(lvalue, rvalue, "Div");
  } else if (node.op == OperatorType::MOD) {
    value = builder.CreateSRem(lvalue, rvalue, "Mod");
  }

  return_stack.push(value);
}

void CodeGenerator::visit(CondBinOperatorAST &node) {
  llvm::Value *lvalue = get_return(*node.lval);
  llvm::Value *rvalue = get_return(*node.rval);

  llvm::Value *value;
  if (node.op == OperatorType::AND) {
    value = builder.CreateAnd(lvalue, rvalue, "And");
  } else if (node.op == OperatorType::OR) {
    value = builder.CreateOr(lvalue, rvalue, "Or");
  }

  return_stack.push(value);
}

void CodeGenerator::visit(RelBinOperatorAST &node) {
  llvm::Value *lvalue = get_return(*node.lval);
  llvm::Value *rvalue = get_return(*node.rval);

  llvm::Value *value;
  if (node.op == OperatorType::LE) {
    value = builder.CreateICmpSLE(lvalue, rvalue, "LE");
  } else if (node.op == OperatorType::LT) {
    value = builder.CreateICmpSLT(lvalue, rvalue, "LT");
  } else if (node.op == OperatorType::GE) {
    value = builder.CreateICmpSGE(lvalue, rvalue, "GE");
  } else if (node.op == OperatorType::GT) {
    value = builder.CreateICmpSGT(lvalue, rvalue, "GT");
  }

  return_stack.push(value);
}

void CodeGenerator::visit(EqBinOperatorAST &node) {
  llvm::Value *lvalue = get_return(*node.lval);
  llvm::Value *rvalue = get_return(*node.rval);

  llvm::Value *value;
  if (node.op == OperatorType::EQ) {
    value = builder.CreateICmpEQ(lvalue, rvalue, "EQ");
  } else if (node.op == OperatorType::NE) {
    value = builder.CreateICmpNE(lvalue, rvalue, "NE");
  }

  return_stack.push(value);
}

void CodeGenerator::visit(UnaryMinusAST &node) {
  llvm::Value *value = get_return(*node.val);
  value = builder.CreateNeg(value, "UnaryMinus");
  return_stack.push(value);
}

void CodeGenerator::visit(UnaryNotAST &node) {
  llvm::Value *value = get_return(*node.val);
  value = builder.CreateNot(value, "UnaryNot");
  return_stack.push(value);
}

// statements.hh
void CodeGenerator::visit(ReturnStatementAST &node) {
  if (node.ret_expr == NULL) { // ret void
    builder.CreateRetVoid();
  } else { // ret val
    auto ret = get_return(*node.ret_expr);
    builder.CreateRet(ret);
  }
}

void CodeGenerator::visit(IfStatementAST &node) {
  llvm::Function *func = builder.GetInsertBlock()->getParent();

  llvm::Value *cond = get_return(*node.cond_expr);
  cond = builder.CreateICmpEQ(
      cond, llvm::ConstantInt::get(context, llvm::APInt(1, 1)), "ifcond");

  llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(context, "then", func);
  llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(context, "else", func);
  llvm::BasicBlock *afterBB =
      llvm::BasicBlock::Create(context, "if-cont", func);

  builder.CreateCondBr(cond, thenBB, elseBB);

  // then block
  builder.SetInsertPoint(thenBB);
  node.then_block->accept(*this);
  builder.CreateBr(afterBB);

  // else block
  builder.SetInsertPoint(elseBB);
  if (node.else_block) {
    node.else_block->accept(*this);
  }
  builder.CreateBr(afterBB);

  // continuation
  builder.SetInsertPoint(afterBB);
}

void CodeGenerator::visit(BreakStatementAST &node) {
  builder.CreateBr(for_jump_blocks.top().second);
}

void CodeGenerator::visit(ContinueStatementAST &node) {
  builder.CreateBr(for_jump_blocks.top().first);
}

void CodeGenerator::visit(ForStatementAST &node) {
  llvm::Function *func = builder.GetInsertBlock()->getParent();

  symbol_table.block_start(); // for scope (contains iterator)

  // pre-block: computes range, and initializes iterator
  llvm::BasicBlock *preBB = llvm::BasicBlock::Create(context, "for-init", func);
  llvm::BasicBlock *condBB =
      llvm::BasicBlock::Create(context, "for-cond", func);
  llvm::BasicBlock *midBB = llvm::BasicBlock::Create(context, "for-temp", func);
  llvm::BasicBlock *incrBB =
      llvm::BasicBlock::Create(context, "for-incr", func);
  llvm::BasicBlock *afterBB =
      llvm::BasicBlock::Create(context, "for-cont", func);

  builder.CreateBr(preBB);
  builder.SetInsertPoint(preBB);

  // loop iterator
  VariableDeclarationAST loop_iter_ast(node.iterator_id, ValueType::INT);
  // const to prevent modifications to iterator ptr
  llvm::Value *const loop_iter = get_return(loop_iter_ast);

  llvm::Value *const init_val = get_return(*node.start_expr);
  llvm::Value *const final_val = get_return(*node.end_expr);
  builder.CreateStore(init_val, loop_iter);

  // condition check
  builder.CreateBr(condBB);
  builder.SetInsertPoint(condBB);
  llvm::Value *loop_iter_val = builder.CreateLoad(loop_iter, "iter-curr");
  llvm::Value *cond =
      builder.CreateICmpSLT(loop_iter_val, final_val, "for-cond-check");
  builder.CreateCondBr(cond, midBB, afterBB);

  // loop body
  builder.SetInsertPoint(midBB);

  for_jump_blocks.emplace(incrBB, afterBB);
  node.block->accept(*this);
  for_jump_blocks.pop();

  // jump to increment block
  builder.CreateBr(incrBB);
  builder.SetInsertPoint(incrBB);
  loop_iter_val = builder.CreateLoad(loop_iter, "iter-curr");
  loop_iter_val = builder.CreateAdd(
      loop_iter_val, llvm::ConstantInt::get(context, llvm::APInt(32, 1)),
      "iter-incr");
  builder.CreateStore(loop_iter_val, loop_iter);
  builder.CreateBr(condBB); // jump to condition

  // restore to continuation
  builder.SetInsertPoint(afterBB);

  symbol_table.block_end(); // end for scope
}

void CodeGenerator::visit(AssignStatementAST &node) {
  llvm::Value *rvalue = get_return(*node.rval);
  llvm::Value *lvalue = get_return(*node.lloc);

  if (node.op != OperatorType::ASSIGN) {
    llvm::Value *ivalue = builder.CreateLoad(lvalue, "lvaltmp");
    if (node.op == OperatorType::ASSIGN_ADD) {
      rvalue = builder.CreateAdd(ivalue, rvalue, "plus-assign");
    } else {
      rvalue = builder.CreateSub(ivalue, rvalue, "minus-assign");
    }
  }

  builder.CreateStore(rvalue, lvalue);
}

// blocks.hh
void CodeGenerator::visit(StatementBlockAST &node) {
  llvm::Function *func = builder.GetInsertBlock()->getParent();

  symbol_table.block_start();

  llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "block", func);
  builder.CreateBr(BB); // jump to new block
  builder.SetInsertPoint(BB);

  for (auto decl : node.variable_declarations) {
    (void)get_return(*decl);
  }

  for (auto statement : node.statements) {
    statement->accept(*this);
  }

  symbol_table.block_end();
}

// methods.hh
void CodeGenerator::visit(MethodDeclarationAST &node) {
  // function proto
  std::vector<llvm::Type *> argument_types;
  for (auto param : node.parameters) {
    argument_types.push_back(get_llvm_type(param->type));
  }
  llvm::Type *return_type = get_llvm_type(node.return_type);
  llvm::FunctionType *func_type =
      llvm::FunctionType::get(return_type, argument_types, false);
  auto linkage = node.name == "main" ? llvm::Function::ExternalLinkage
                                     : llvm::Function::InternalLinkage;

  llvm::Function *func =
      llvm::Function::Create(func_type, linkage, node.name, module);

  // function body
  symbol_table.block_start();

  // generate code for body
  llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "entry", func);
  builder.SetInsertPoint(BB);

  {
    auto iter = node.parameters.begin();
    for (auto &arg : func->args()) {
      auto param = *iter;

      arg.setName(param->id);
      llvm::Value *alloca = get_return(*param);
      builder.CreateStore(&arg, alloca);

      iter++;
    }
  }

  node.body->accept(*this);

  if (node.return_type == ValueType::VOID) {
    // create a return at the end of void function, to avoid IR error
    builder.CreateRetVoid();
  } else {
    // TODO: check for missing return in semantic analysis
    add_runtime_error_inst(2, "Control reaches end of function `" + node.name +
                                  "`");
  }

  symbol_table.block_end();

  if (llvm::verifyFunction(*func)) {
    has_error = true;
  }
}

void CodeGenerator::visit(MethodCallAST &node) {
  std::vector<llvm::Value *> args;
  for (auto arg : node.arguments) {
    args.push_back(get_return(*arg));
  }
  llvm::Function *func = module->getFunction(node.id);

  if (func->getReturnType() ==
      llvm::Type::getVoidTy(context)) { // void function
    builder.CreateCall(func, args);
  } else {
    auto ret = builder.CreateCall(func, args, "fcall");
    return_stack.push(ret);
  }
}

void CodeGenerator::visit(CalloutCallAST &node) {
  add_builtin(node.id, node.arg_types, ValueType::INT);
  MethodCallAST *p = dynamic_cast<MethodCallAST *>(&node);
  visit(*p);
}

// program.hh
void CodeGenerator::visit(ProgramAST &node) {
  for (auto decl : node.global_variables) {
    decl->accept(*this);
  }

  for (auto method : node.methods) {
    method->accept(*this);
  }
}