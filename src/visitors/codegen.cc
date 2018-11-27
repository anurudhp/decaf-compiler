#include <cstring>
#include <cstdarg>
#include <iostream>

#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include "codegen.hh"
#include "../ast/ast.hh"
#include "../ast/literals.hh"
#include "../ast/operators.hh"
#include "../ast/variables.hh"
#include "../ast/statements.hh"
#include "../ast/blocks.hh"
#include "../ast/methods.hh"
#include "../ast/program.hh"
#include "../exceptions.hh"

/*** CodeGenerator::SymbolTable ***/
void CodeGenerator::SymbolTable::block_start() {
	variables.emplace_back();
}
void CodeGenerator::SymbolTable::block_end() {
	assert(!variables.empty());
	variables.pop_back();
}

void CodeGenerator::SymbolTable::add_variable(std::string id, llvm::AllocaInst *alloca) {
	 variables.back()[id] = alloca;
}
llvm::AllocaInst * CodeGenerator::SymbolTable::lookup_variable(std::string id) {
	for (auto it = variables.rbegin(); it != variables.rend(); it++) {
		if (it->count(id) > 0) {
			return (*it)[id];
		}
	}
	return nullptr;
}
bool CodeGenerator::SymbolTable::is_global_scope() {
	return variables.empty();
}

/*** CodeGenerator ***/
CodeGenerator::CodeGenerator(std::string name)
: builder(context) {
	module = new llvm::Module(name, context);
}
CodeGenerator::~CodeGenerator() {
	delete module;
}

void CodeGenerator::add_builtin(std::string name, std::vector<ValueType> _params, ValueType ret) {
	if (module->getFunction(name) != nullptr) return;

	std::vector<llvm::Type *> params;
	for (auto p: _params) {
		if (p == ValueType::INT) {
			params.push_back(llvm::Type::getInt32Ty(context));
		} else if (p == ValueType::BOOL) {
			params.push_back(llvm::Type::getInt1Ty(context));
		}
	}

	llvm::Type *ret_type;
	if (ret == ValueType::INT) {
		ret_type = llvm::Type::getInt32Ty(context);
	} else if (ret == ValueType::BOOL) {
		ret_type = llvm::Type::getInt1Ty(context);
	} else {
		ret_type = llvm::Type::getVoidTy(context);
	}

	llvm::FunctionType *ftype = llvm::FunctionType::get(ret_type, params, false);
	llvm::Function::Create(ftype, llvm::Function::ExternalLinkage, name, module);
}

void CodeGenerator::generate(BaseAST& root) {
	/** generate code **/
	root.accept(*this);
}

void CodeGenerator::print(const std::string& outf) {
	// std::error_code EC;
	// llvm::raw_fd_ostream out(outf, EC);
	// if (EC) {
	// 	std::cerr << "Error writing to file " << outf << "\n";
	//  return;
	// }

	module->print(llvm::outs(), nullptr);
}

llvm::Value* CodeGenerator::get_return_stack_top(bool pop) {
	assert(!return_stack.empty());
	llvm::Value *res = return_stack.top();
	if (pop) return_stack.pop();
	return res;
}
llvm::Value* CodeGenerator::get_return(BaseAST& node) {
	node.accept(*this);
	return get_return_stack_top();
}

llvm::Type* CodeGenerator::get_type(ValueType ty) {
	if (ty == ValueType::INT) {
		return llvm::Type::getInt32Ty(context);
	}
	if (ty == ValueType::BOOL) {
		return llvm::Type::getInt1Ty(context);
	}
	if (ty == ValueType::VOID) {
		return llvm::Type::getVoidTy(context);
	}

	return nullptr;
}

void CodeGenerator::error(const std::string& fmt, ...) {
	static const int SIZE = 300;
	std::string err(SIZE, '\0');

	va_list args;
	va_start(args, fmt);
	vsnprintf(&err[0], SIZE, fmt.c_str(), args);
	va_end(args);

	std::cerr << err << '\n';
}

/*** visits: ***/
void CodeGenerator::visit(BaseAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}

// literals.hh
void CodeGenerator::visit(LiteralAST& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(IntegerLiteralAST& node) {
	llvm::Value* value = llvm::ConstantInt::get(context, llvm::APInt(32, node.value));
	return_stack.push(value);
}
void CodeGenerator::visit(BooleanLiteralAST& node) {
	llvm::Value* value =  llvm::ConstantInt::get(context, llvm::APInt(1, node.value));
	return_stack.push(value);
}
void CodeGenerator::visit(StringLiteralAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}

// variables.hh
void CodeGenerator::visit(LocationAST& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(VariableLocationAST& node) {
	llvm::Value *var = symbol_table.lookup_variable(node.id);
	if (var == nullptr) {
		var = module->getNamedGlobal(node.id);
	}
	if (!node.is_lvalue) {
		var = builder.CreateLoad(var, node.id.c_str());
	}
	return_stack.push(var);
}
void CodeGenerator::visit(ArrayLocationAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}

void CodeGenerator::visit(VariableDeclarationAST& node) {
	if (symbol_table.is_global_scope()) { // global variable
		throw unimplemented_error(__PRETTY_FUNCTION__);
	} else { // local/block variable
		llvm::AllocaInst* alloca = builder.CreateAlloca(get_type(node.type), 0, node.id);
		symbol_table.add_variable(node.id, alloca);
		return_stack.push(alloca);
	}
}

void CodeGenerator::visit(ArrayDeclarationAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}

// operators.hh
void CodeGenerator::visit(UnaryOperatorAST& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(BinaryOperatorAST& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}

void CodeGenerator::visit(ArithBinOperatorAST& node) {
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

void CodeGenerator::visit(CondBinOperatorAST& node) {
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

void CodeGenerator::visit(RelBinOperatorAST& node) {
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

void CodeGenerator::visit(EqBinOperatorAST& node) {
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

void CodeGenerator::visit(UnaryMinusAST& node) {
	llvm::Value *value = get_return(*node.val);
	value = builder.CreateNeg(value, "UnaryMinus");
	return_stack.push(value);
}

void CodeGenerator::visit(UnaryNotAST& node) {
	llvm::Value *value = get_return(*node.val);
	value = builder.CreateNot(value, "UnaryNot");
	return_stack.push(value);
}

// statements.hh
void CodeGenerator::visit(ReturnStatementAST& node) {
	// throw unimplemented_error(__PRETTY_FUNCTION__);

}
void CodeGenerator::visit(BreakStatementAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(ContinueStatementAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(IfStatementAST& node) {
	llvm::Function *func = builder.GetInsertBlock()->getParent();

	llvm::Value *cond = get_return(*node.cond_expr);
	cond = builder.CreateICmpEQ(cond, llvm::ConstantInt::get(context, llvm::APInt(1, 1)), "ifcond");

	llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(context, "then", func);
	llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(context, "else", func);
	llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(context, "ifcont");

	builder.CreateCondBr(cond, thenBB, elseBB);

	// then block
	builder.SetInsertPoint(thenBB);
	node.then_block->accept(*this);

	builder.CreateBr(mergeBB);
	thenBB = builder.GetInsertBlock();

	// else block
	builder.SetInsertPoint(elseBB);
	if (node.else_block) {
		node.else_block->accept(*this);
	}

	builder.CreateBr(mergeBB);
	elseBB = builder.GetInsertBlock();

	// continuation
	func->getBasicBlockList().push_back(mergeBB);
	builder.SetInsertPoint(mergeBB);
}

void CodeGenerator::visit(ForStatementAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(AssignStatementAST& node) {
	llvm::Value *rvalue = get_return(*node.rval);
	llvm::Value *lvalue = get_return(*node.lloc);

	if (node.op != OperatorType::ASSIGN) {
		llvm::Value *ivalue = builder.CreateLoad(lvalue);
		if (node.op == OperatorType::ASSIGN_ADD) {
			rvalue = builder.CreateAdd(ivalue, rvalue, "+=");
		} else {
			rvalue = builder.CreateSub(ivalue, rvalue, "-=");
		}
	}
	
	builder.CreateStore(rvalue, lvalue);
}

// blocks.hh
void CodeGenerator::visit(StatementBlockAST& node) {
	llvm::Function *func = builder.GetInsertBlock()->getParent();

	symbol_table.block_start();

	llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "block", func);
	builder.CreateBr(BB); // jump to new block
	builder.SetInsertPoint(BB);

	for (auto decl: node.variable_declarations) {
		(void) get_return(*decl);
	}

	for (auto statement: node.statements) {
		statement->accept(*this);
	}

	symbol_table.block_end();
}

// methods.hh
void CodeGenerator::visit(MethodDeclarationAST& node) {
	// function proto
	std::vector<llvm::Type *> argument_types;
	for (auto param: node.parameters) {
		argument_types.push_back(get_type(param->type));
	}
	llvm::Type *return_type = get_type(node.return_type);
	llvm::FunctionType *func_type = llvm::FunctionType::get(return_type, argument_types, false);
	llvm::Function *func = llvm::Function::Create(func_type,
												  llvm::Function::ExternalLinkage,
												  node.name,
												  module);

	// function body
	symbol_table.block_start();

	// generate code for body
	llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "entry", func);
	builder.SetInsertPoint(BB);

	{
		auto iter = node.parameters.begin();
		for (auto& arg: func->args()) {
			auto param = *iter;

			arg.setName(param->id);
			llvm::Value* alloca = get_return(*param);
			builder.CreateStore(&arg, alloca);

			iter++;
		}
	}
	
	node.body->accept(*this);

	if (node.return_type == ValueType::VOID) {
		// create a return at the end of void function, to avoid IR error
		builder.CreateRetVoid();
	} else if (node.return_type == ValueType::INT) {
		// return 0 on reaching end (default)
		// TODO: check for missing return in semantic analysis
		builder.CreateRet(llvm::ConstantInt::get(context, llvm::APInt(32, 0)));
	} else if (node.return_type == ValueType::BOOL) {
		// return false on reaching end (default)
		builder.CreateRet(llvm::ConstantInt::get(context, llvm::APInt(1, 0)));
	}

	symbol_table.block_end();
	
	llvm::verifyFunction(*func);
}

void CodeGenerator::visit(MethodCallAST& node) {
	std::vector<llvm::Value *> args;
	for (auto arg: node.arguments) {
		args.push_back(get_return(*arg));
	}
	llvm::Function *func = module->getFunction(node.id);

	if (func->getReturnType() == llvm::Type::getVoidTy(context)) { // void function
		builder.CreateCall(func, args);
	} else {
		auto ret = builder.CreateCall(func, args, "fcall");
		return_stack.push(ret);
	}
}

void CodeGenerator::visit(CalloutCallAST& node) {
	add_builtin(node.id, node.arg_types, ValueType::INT);
	MethodCallAST *p = dynamic_cast<MethodCallAST *>(&node);
	visit(*p);
}

// program.hh
void CodeGenerator::visit(ProgramAST& node) {
	for (auto decl: node.global_variables) {
		decl->accept(*this);
	}

	for (auto method: node.methods) {
		method->accept(*this);
	}

}