#include <cassert>
#include <cstdarg>
#include <cstdio>

#include "../ast/ast.hh"
#include "../ast/blocks.hh"
#include "../ast/literals.hh"
#include "../ast/methods.hh"
#include "../ast/operators.hh"
#include "../ast/program.hh"
#include "../ast/statements.hh"
#include "../ast/variables.hh"
#include "../exceptions.hh"
#include "semantic_analyzer.hh"

/*** SemanticAnalyzer::SymbolTable ***/
void SemanticAnalyzer::SymbolTable::block_start() {
  if (hold_depth > 0) {
    hold_depth--;
    return;
  }
  variables.emplace_back();
  scope_depth++;
}
void SemanticAnalyzer::SymbolTable::block_end() {
  assert(scope_depth > 1);
  variables.pop_back();
  scope_depth--;
}

void SemanticAnalyzer::silent(bool f) { _silent = f; }

// add
void SemanticAnalyzer::SymbolTable::add_variable(
    VariableDeclarationAST *variable) {
  auto &current_scope = variables.back();
  if (current_scope.count(variable->id)) {
    auto previous_decl = current_scope[variable->id];
    analyzer.log_error(
        1, variable->location,
        "Redeclaration of variable `%s` (previously declared at [%s]: `%s`)",
        variable->id.c_str(), previous_decl->location.c_str(),
        previous_decl->to_string().c_str());
    return;
  }

  if (scope_depth == 1) {
    if (arrays.count(variable->id)) {
      auto previous_decl = arrays[variable->id];
      analyzer.log_error(
          1, variable->location,
          "Redeclaration of variable `%s` (previously declared at [%s]: `%s`)",
          variable->id.c_str(), previous_decl->location.c_str(),
          previous_decl->to_string().c_str());
      return;
    }
  }

  current_scope[variable->id] = variable;
}
void SemanticAnalyzer::SymbolTable::add_array(ArrayDeclarationAST *array) {
  if (arrays.count(array->id)) {
    auto previous_decl = arrays[array->id];
    analyzer.log_error(
        1, array->location,
        "Redeclaration of array `%s` (previously declared at [%s]: `%s`)",
        array->id.c_str(), previous_decl->location.c_str(),
        previous_decl->to_string().c_str());
    return;
  }

  auto &current_scope = variables[0];
  if (current_scope.count(array->id)) {
    auto previous_decl = current_scope[array->id];
    analyzer.log_error(
        1, array->location,
        "Redeclaration of array `%s` (previously declared at [%s]: `%s`)",
        array->id.c_str(), previous_decl->location.c_str(),
        previous_decl->to_string().c_str());
    return;
  }

  if (array->array_len == 0) {
    analyzer.log_error(4, array->location,
                       "Array length cannot be zero! (declared `%s`)",
                       array->to_string().c_str());
  }

  arrays[array->id] = array;
}

void SemanticAnalyzer::SymbolTable::add_method(MethodDeclarationAST *method) {
  if (methods.count(method->name)) {
    auto previous_decl = methods[method->name];
    analyzer.log_error(
        1, method->location,
        "Reuse of method name `%s` (previously declared at [%s]: `%s`)",
        method->name.c_str(), previous_decl->location.c_str(),
        previous_decl->to_string().c_str());
    return;
  }

  auto &global_scope = variables[0];
  if (global_scope.count(method->name)) {
    auto previous_decl = global_scope[method->name];
    analyzer.log_error(1, method->location,
                       "Invalid reuse of variable name `%s` for method "
                       "(previously declared at [%s]: `%s`)",
                       method->name.c_str(), previous_decl->location.c_str(),
                       previous_decl->to_string().c_str());
    return;
  }

  if (arrays.count(method->name)) {
    auto previous_decl = arrays[method->name];
    analyzer.log_error(1, method->location,
                       "Invalid reuse of array name `%s` for method "
                       "(previously declared at [%s]: `%s`)",
                       method->name.c_str(), previous_decl->location.c_str(),
                       previous_decl->to_string().c_str());
    return;
  }

  methods[method->name] = method;
}

// lookup
VariableDeclarationAST *
SemanticAnalyzer::SymbolTable::lookup_variable(LocationAST *varloc) {
  for (int i = scope_depth - 1; i >= 0; i--) {
    if (variables[i].count(varloc->id)) {
      return variables[i][varloc->id];
    }
  }

  if (arrays.count(varloc->id)) {
    auto decl = arrays[varloc->id];
    analyzer.log_error(
        9, varloc->location,
        "Invalid use of array `%s` as variable (declared at [%s]: `%s`)",
        varloc->id.c_str(), decl->location.c_str(), decl->to_string().c_str());
  } else if (methods.count(varloc->id)) {
    auto decl = methods[varloc->id];
    analyzer.log_error(
        9, varloc->location,
        "Invalid use of method `%s` as variable (declared at [%s]: `%s`)",
        varloc->id.c_str(), decl->location.c_str(), decl->to_string().c_str());
  } else {
    analyzer.log_error(2, varloc->location, "Variable `%s` not declared",
                       varloc->id.c_str());
  }
  return nullptr;
}
ArrayDeclarationAST *
SemanticAnalyzer::SymbolTable::lookup_array_element(LocationAST *arrloc) {
  for (int i = scope_depth - 1; i >= 0; i--) {
    if (variables[i].count(arrloc->id)) {
      auto decl = variables[i][arrloc->id];
      analyzer.log_error(9, arrloc->location,
                         "Invalid use of scalar variable `%s` as array "
                         "(declared at [%s]: `%s`)",
                         arrloc->id.c_str(), decl->location.c_str(),
                         decl->to_string().c_str());
      return nullptr;
    }
  }

  if (arrays.count(arrloc->id)) {
    return arrays[arrloc->id];
  } else if (methods.count(arrloc->id)) {
    auto decl = methods[arrloc->id];
    analyzer.log_error(
        9, arrloc->location,
        "Invalid use of method `%s` as array (declared at [%s]: `%s`)",
        arrloc->id.c_str(), decl->location.c_str(), decl->to_string().c_str());
  } else {
    analyzer.log_error(2, arrloc->location, "Array `%s` not declared",
                       arrloc->id.c_str());
  }
  return nullptr;
}

MethodDeclarationAST *
SemanticAnalyzer::SymbolTable::lookup_method(MethodCallAST *mcall) {
  for (int i = scope_depth - 1; i >= 0; i--) {
    if (variables[i].count(mcall->id)) {
      auto decl = variables[i][mcall->id];
      analyzer.log_error(
          2, mcall->location,
          "Invalid use of variable `%s` as method (declared at [%s]: `%s`",
          mcall->id.c_str(), decl->location.c_str(), decl->to_string().c_str());
      return nullptr;
    }
  }

  if (arrays.count(mcall->id)) {
    auto decl = arrays[mcall->id];
    analyzer.log_error(
        2, mcall->location,
        "Invalid use of array `%s` as method (declared at [%s]: `%s`",
        mcall->id.c_str(), decl->location.c_str(), decl->to_string().c_str());
    return nullptr;
  }

  if (methods.count(mcall->id)) {
    return methods[mcall->id];
  }

  analyzer.log_error(2, mcall->location, "Method `%s` not declared",
                     mcall->id.c_str());
  return nullptr;
}

/*** SemanticAnalyzer ***/
void SemanticAnalyzer::log_error(const int error_type,
                                 const std::string &location,
                                 const std::string &fmt, ...) {
  if (_silent)
    return; // ignore errors

  va_list args;
  va_start(args, fmt);
  vsnprintf(_buffer, BUFFER_LENGTH, fmt.c_str(), args);
  va_end(args);

  // errors
  std::string msg(_buffer);
  if (!location.empty()) {
    snprintf(_buffer, BUFFER_LENGTH, "[%s] ", location.c_str());
    msg = _buffer + msg;
  }
  errors.emplace_back(error_type, msg);
}

bool SemanticAnalyzer::check(BaseAST &root) {
  symbol_table = new SymbolTable(*this);
  root.accept(*this);
  delete symbol_table;
  symbol_table = nullptr;

  return errors.empty();
}
void SemanticAnalyzer::display(std::ostream &out, const bool show_rules) {
  for (auto &err : errors) {
    if (show_rules) {
      snprintf(_buffer, BUFFER_LENGTH, "{Rule:%2d} ", err.first);
      out << _buffer;
    }
    out << err.second << "\n";
  }
}

ValueType SemanticAnalyzer::get_top_type(bool pop) {
  ValueType res = type_stack.top();
  if (pop)
    type_stack.pop();
  return res;
}

// Visit functions
void SemanticAnalyzer::visit(BaseAST &node) {
  throw invalid_call_error(__PRETTY_FUNCTION__);
}

// literals.hh
void SemanticAnalyzer::visit(LiteralAST &node) {
  throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(IntegerLiteralAST &node) {
  type_stack.push(ValueType::INT);
}
void SemanticAnalyzer::visit(BooleanLiteralAST &node) {
  type_stack.push(ValueType::BOOL);
}
void SemanticAnalyzer::visit(StringLiteralAST &node) {
  type_stack.push(ValueType::STRING);
}

// variables.hh
void SemanticAnalyzer::visit(LocationAST &node) {
  throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(VariableLocationAST &node) {
  auto decl = symbol_table->lookup_variable(&node);
  type_stack.push(decl ? decl->type : ValueType::NONE);
}
void SemanticAnalyzer::visit(ArrayLocationAST &node) {
  auto decl = symbol_table->lookup_array_element(&node);
  type_stack.push(decl ? decl->type : ValueType::NONE);

  node.index_expr->accept(*this);
  ValueType index_type = get_top_type();
  if (index_type != ValueType::INT && index_type != ValueType::NONE) {
    log_error(10, node.location,
              "Invalid index for array, expected `int` expression, got `%s`",
              value_type_to_string(index_type).c_str());
  }
}
void SemanticAnalyzer::visit(ArrayAddressAST &node) {
  auto decl = symbol_table->lookup_array_element(&node);
  if (decl && decl->type == ValueType::INT) {
    type_stack.push(ValueType::INT_ARRAY);
  } else {
    type_stack.push(ValueType::NONE);
  }
}

// operators.hh
void SemanticAnalyzer::visit(UnaryOperatorAST &node) {
  throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(BinaryOperatorAST &node) {
  throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(ArithBinOperatorAST &node) {
  bool has_error = false;

  for (auto val : {node.lval, node.rval}) {
    val->accept(*this);
    ValueType res = get_top_type();
    if (res != ValueType::INT) {
      has_error = true;
      if (res == ValueType::NONE)
        continue;
      log_error(12, val->location,
                "Invalid operand for `%s`: Expected `int`, got `%s`",
                operator_type_to_string(node.op).c_str(),
                value_type_to_string(res).c_str());
    }
  }

  type_stack.push(has_error ? ValueType::NONE : ValueType::INT);
}
void SemanticAnalyzer::visit(CondBinOperatorAST &node) {
  bool has_error = false;

  for (auto val : {node.lval, node.rval}) {
    val->accept(*this);
    ValueType res = get_top_type();
    if (res != ValueType::BOOL) {
      has_error = true;
      if (res == ValueType::NONE)
        continue;
      log_error(14, val->location,
                "Invalid operand for `%s`: Expected `boolean`, got `%s`",
                operator_type_to_string(node.op).c_str(),
                value_type_to_string(res).c_str());
    }
  }

  type_stack.push(has_error ? ValueType::NONE : ValueType::BOOL);
}
void SemanticAnalyzer::visit(RelBinOperatorAST &node) {
  bool has_error = false;

  for (auto val : {node.lval, node.rval}) {
    val->accept(*this);
    ValueType res = get_top_type();
    if (res != ValueType::INT) {
      has_error = true;
      if (res == ValueType::NONE)
        continue;
      log_error(12, val->location,
                "Invalid operand for `%s`: Expected `int`, got `%s`",
                operator_type_to_string(node.op).c_str(),
                value_type_to_string(res).c_str());
    }
  }

  type_stack.push(has_error ? ValueType::NONE : ValueType::BOOL);
}
void SemanticAnalyzer::visit(EqBinOperatorAST &node) {
  bool has_error = false;

  node.lval->accept(*this);
  ValueType ltype = get_top_type();
  node.rval->accept(*this);
  ValueType rtype = get_top_type();

  if (ltype != rtype) {
    if (ltype != ValueType::NONE && rtype != ValueType::NONE) {
      log_error(
          12, node.location,
          "Invalid operands for `%s`: Expected same type, got `%s` and `%s`",
          operator_type_to_string(node.op).c_str(),
          value_type_to_string(ltype).c_str(),
          value_type_to_string(rtype).c_str());
    }
    has_error = true;
  }

  type_stack.push(has_error ? ValueType::NONE : ValueType::BOOL);
}

void SemanticAnalyzer::visit(UnaryMinusAST &node) {
  node.val->accept(*this);
  ValueType res = get_top_type();
  if (res != ValueType::INT && res != ValueType::NONE) {
    log_error(12, node.val->location,
              "Invalid operand for `%s`: Expected `int`, got `%s`",
              operator_type_to_string(node.op).c_str(),
              value_type_to_string(res).c_str());
  }

  type_stack.push(res);
}
void SemanticAnalyzer::visit(UnaryNotAST &node) {
  node.val->accept(*this);
  ValueType res = get_top_type();
  if (res != ValueType::BOOL && res != ValueType::NONE) {
    log_error(14, node.val->location,
              "Invalid operand for `%s`: Expected `boolean`, got `%s`",
              operator_type_to_string(node.op).c_str(),
              value_type_to_string(res).c_str());
  }

  type_stack.push(res);
}

// statements.hh
void SemanticAnalyzer::visit(ReturnStatementAST &node) {
  if (current_method->return_type == ValueType::VOID &&
      node.ret_expr != nullptr) {
    log_error(7, node.location, "Unexpected return expression for method `%s`",
              current_method->to_string().c_str());
  }

  if (current_method->return_type != ValueType::VOID) {
    ValueType ret_type = ValueType::VOID;
    if (node.ret_expr != nullptr) {
      node.ret_expr->accept(*this);
      ret_type = get_top_type();
    }

    if (ret_type == ValueType::NONE)
      return;

    if (ret_type != current_method->return_type) {
      log_error(8, node.location,
                "In method `%s`: Expected `%s` return expression, got `%s`",
                current_method->to_string().c_str(),
                value_type_to_string(current_method->return_type).c_str(),
                value_type_to_string(ret_type).c_str());
    }
  }
}
void SemanticAnalyzer::visit(BreakStatementAST &node) {
  if (for_loop_depth == 0) {
    log_error(18, node.location, "Unexpected break");
  }
}
void SemanticAnalyzer::visit(ContinueStatementAST &node) {
  if (for_loop_depth == 0) {
    log_error(18, node.location, "Unexpected continue");
  }
}
void SemanticAnalyzer::visit(IfStatementAST &node) {
  node.cond_expr->accept(*this);
  ValueType cond_type = get_top_type();
  if (cond_type != ValueType::BOOL && cond_type != ValueType::NONE) {
    log_error(11, node.location,
              "Expected boolean expression for `if` condition, got `%s`",
              value_type_to_string(cond_type).c_str());
  }

  node.then_block->accept(*this);
  if (node.else_block != nullptr) {
    node.else_block->accept(*this);
  }
}
void SemanticAnalyzer::visit(ForStatementAST &node) {
  for (auto expr : {node.start_expr, node.end_expr}) {
    expr->accept(*this);
    ValueType type = get_top_type();
    if (type != ValueType::INT && type != ValueType::NONE) {
      log_error(17, expr->location,
                "Invalid loop bound expression: Expected `int`, got `%s`",
                value_type_to_string(type).c_str());
    }
  }

  for_loop_depth++;
  symbol_table->block_start();
  auto iter = new VariableDeclarationAST(node.iterator_id, ValueType::INT);
  symbol_table->add_variable(iter);

  node.block->accept(*this);

  symbol_table->block_end();
  delete iter;
  for_loop_depth--;
}
void SemanticAnalyzer::visit(AssignStatementAST &node) {
  node.lloc->accept(*this);
  ValueType ltype = get_top_type();
  node.rval->accept(*this);
  ValueType rtype = get_top_type();

  if (ltype == ValueType::NONE || rtype == ValueType::NONE)
    return;

  if (node.op == OperatorType::ASSIGN) {
    if (ltype != rtype) {
      log_error(
          15, node.location,
          "Mismatched operand types for `%s`: expected same, got `%s` and `%s`",
          operator_type_to_string(node.op).c_str(),
          value_type_to_string(ltype).c_str(),
          value_type_to_string(rtype).c_str());
    }
  } else {
    if (ltype != ValueType::INT) {
      log_error(
          16, node.location,
          "Invalid (lvalue) location type for `%s`: expected `int`, got `%s`",
          operator_type_to_string(node.op).c_str(),
          value_type_to_string(ltype).c_str());
    }
    if (rtype != ValueType::INT) {
      log_error(
          16, node.location,
          "Invalid (rvalue) expression type for `%s`: expected `int`, got `%s`",
          operator_type_to_string(node.op).c_str(),
          value_type_to_string(rtype).c_str());
    }
  }
}

// blocks.hh
void SemanticAnalyzer::visit(StatementBlockAST &node) {
  symbol_table->block_start();

  for (auto decl : node.variable_declarations) {
    symbol_table->add_variable(decl);
  }
  for (auto statement : node.statements) {
    statement->accept(*this);
  }

  symbol_table->block_end();
}

// methods.hh
void SemanticAnalyzer::visit(MethodDeclarationAST &node) {
  symbol_table->block_start(); // method scope
  for (auto param : node.parameters) {
    symbol_table->add_variable(param);
  }
  symbol_table->hold_depth = 1; // parameter scope == function scope
  node.body->accept(*this);
}

void SemanticAnalyzer::visit(MethodCallAST &node) {
  auto decl = symbol_table->lookup_method(&node);
  type_stack.push(decl ? decl->return_type : ValueType::NONE);

  if (!decl)
    return;

  // check: argument ~ parameter
  if (decl->parameters.size() != node.arguments.size()) {
    log_error(5, node.location,
              "Too %s arguments to method `%s` (expected %d, got %d)",
              (decl->parameters.size() < node.arguments.size()) ? "many"
                                                                : "few",
              node.id.c_str(), (int)decl->parameters.size(),
              (int)node.arguments.size());
  } else {
    for (unsigned i = 0; i < decl->parameters.size(); i++) {
      node.arguments[i]->accept(*this);
      ValueType arg_type = get_top_type();
      ValueType param_type = decl->parameters[i]->type;

      if (param_type != arg_type) {
        log_error(5, node.location,
                  "Method call `%s(...)`: Type mismatch for parameter `%s`: "
                  "expected %s, got %s",
                  node.id.c_str(), decl->parameters[i]->id.c_str(),
                  value_type_to_string(param_type).c_str(),
                  value_type_to_string(arg_type).c_str());
      }
    }
  }
}

void SemanticAnalyzer::visit(CalloutCallAST &node) {
  for (auto &arg : node.arguments) {
    // array address as argument
    // TODO: cleanup (its *way* too convoluted right now)
    {
      silent(true);
      LocationAST *loc = dynamic_cast<LocationAST *>(arg);
      if (loc != nullptr && loc->index_expr == nullptr) {
        ArrayLocationAST array_loc(loc->id, nullptr);
        auto array_decl = symbol_table->lookup_array_element(&array_loc);
        if (array_decl != nullptr && array_decl->type == ValueType::INT) {
          BaseAST *new_arg = new ArrayAddressAST(loc->id);
          std::swap(arg, new_arg);
        }
      }
      silent(false);
    }

    arg->accept(*this);

    ValueType expr = get_top_type();
    if (expr == ValueType::NONE)
      continue;

    if (expr != ValueType::INT && expr != ValueType::BOOL &&
        expr != ValueType::STRING && expr != ValueType::INT_ARRAY) {
      log_error(5, arg->location, "Invalid callout argument type `%s`",
                value_type_to_string(expr).c_str());
    }

    node.arg_types.push_back(expr);
  }
  type_stack.push(ValueType::INT);
}

// program.hh
void SemanticAnalyzer::visit(ProgramAST &node) {
  symbol_table->block_start(); // global scope

  for_loop_depth = 0;

  for (auto decl : node.global_variables) {
    auto adecl = dynamic_cast<ArrayDeclarationAST *>(decl);
    if (adecl == nullptr) {
      symbol_table->add_variable(decl);
    } else {
      symbol_table->add_array(adecl);
    }
  }

  for (auto method : node.methods) {
    symbol_table->add_method(method);
    current_method = method;
    method->accept(*this);
  }

  // check for main:
  if (!symbol_table->methods.count("main")) {
    log_error(3, "", "Method `main` not declared!");
  } else {
    auto main = symbol_table->methods["main"];
    if (main->return_type != ValueType::VOID) {
      log_error(3, main->location,
                "Method `main` must return void (instead returns `%s`)",
                value_type_to_string(main->return_type).c_str());
    }
    if (!main->parameters.empty()) {
      log_error(3, main->location,
                "Method `main` cannot have any parameters (declared: `%s`)",
                main->to_string().c_str());
    }
  }
}
