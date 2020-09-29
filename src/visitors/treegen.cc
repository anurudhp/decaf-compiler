#include <stdexcept>

#include "../ast/ast.hh"
#include "../ast/blocks.hh"
#include "../ast/literals.hh"
#include "../ast/methods.hh"
#include "../ast/operators.hh"
#include "../ast/program.hh"
#include "../ast/statements.hh"
#include "../ast/variables.hh"
#include "../exceptions.hh"
#include "treegen.hh"

void TreeGenerator::generate(BaseAST &root, std::ostream &out) {
  root.accept(*this);

  out << "graph TD\n";
  for (int i = 0; i < (int)node_names.size(); i++) {
    out << i << "[\"" << node_names[i] << "\"]"
        << "\n";
  }
  for (int u = 0; u < (int)out_edges.size(); u++) {
    for (int v : out_edges[u]) {
      out << u << "-->" << v << "\n";
    }
  }
}

int TreeGenerator::add_node(std::string name) {
  node_names.push_back(name);
  out_edges.emplace_back();
  return node_names.size() - 1;
}

void TreeGenerator::add_edge(int u, int v) { out_edges[u].push_back(v); }
void TreeGenerator::add_edge_top(int u) {
  if (!stack.empty()) {
    out_edges[u].push_back(stack.top());
    stack.pop();
  }
}
void TreeGenerator::add_edge_implicit(int u, const std::string &sv) {
  int v = add_node(sv);
  add_edge(u, v);
}

// Visit methods
void TreeGenerator::visit(BaseAST &node) {
  throw invalid_call_error(__PRETTY_FUNCTION__);
}

void TreeGenerator::visit(LiteralAST &node) {
  throw invalid_call_error(__PRETTY_FUNCTION__);
}
void TreeGenerator::visit(IntegerLiteralAST &node) {
  int id = add_node("[LIT] " + std::to_string(node.value));
  stack.push(id);
}
void TreeGenerator::visit(BooleanLiteralAST &node) {
  int id = add_node(std::string("[LIT] ") + (node.value ? "TRUE" : "FALSE"));
  stack.push(id);
}
void TreeGenerator::visit(StringLiteralAST &node) {
  int id = add_node("[LIT] \'" + node.value + "\'");
  stack.push(id);
}

void TreeGenerator::visit(LocationAST &node) {
  throw invalid_call_error(__PRETTY_FUNCTION__);
}
void TreeGenerator::visit(VariableLocationAST &node) {
  int id = add_node(node.id);
  stack.push(id);
}
void TreeGenerator::visit(ArrayLocationAST &node) {
  int id = add_node(node.id + "[]");
  stack.push(id);
}

void TreeGenerator::visit(UnaryOperatorAST &node) {
  int id = add_node(operator_type_to_string(node.op));

  node.val->accept(*this);
  add_edge_top(id);

  stack.push(id);
}
void TreeGenerator::visit(BinaryOperatorAST &node) {
  int id = add_node(operator_type_to_string(node.op));

  node.lval->accept(*this);
  add_edge_top(id);
  node.rval->accept(*this);
  add_edge_top(id);

  stack.push(id);
}
void TreeGenerator::visit(ArithBinOperatorAST &node) {
  visit(dynamic_cast<BinaryOperatorAST &>(node));
}
void TreeGenerator::visit(CondBinOperatorAST &node) {
  visit(dynamic_cast<BinaryOperatorAST &>(node));
}
void TreeGenerator::visit(RelBinOperatorAST &node) {
  visit(dynamic_cast<BinaryOperatorAST &>(node));
}
void TreeGenerator::visit(EqBinOperatorAST &node) {
  visit(dynamic_cast<BinaryOperatorAST &>(node));
}
void TreeGenerator::visit(UnaryMinusAST &node) {
  visit(dynamic_cast<UnaryOperatorAST &>(node));
}
void TreeGenerator::visit(UnaryNotAST &node) {
  visit(dynamic_cast<UnaryOperatorAST &>(node));
}

void TreeGenerator::visit(ReturnStatementAST &node) {
  int id = add_node("return");
  if (node.ret_expr != NULL) {
    node.ret_expr->accept(*this);
    add_edge_top(id);
  }
  stack.push(id);
}
void TreeGenerator::visit(BreakStatementAST &node) {
  int id = add_node("break");
  stack.push(id);
}
void TreeGenerator::visit(ContinueStatementAST &node) {
  int id = add_node("continue");
  stack.push(id);
}
void TreeGenerator::visit(IfStatementAST &node) {
  int id = add_node("if");

  node.cond_expr->accept(*this);
  add_edge_top(id);
  node.then_block->accept(*this);
  add_edge_top(id);
  if (node.else_block != NULL) {
    node.else_block->accept(*this);
    add_edge_top(id);
  }

  stack.push(id);
}
void TreeGenerator::visit(ForStatementAST &node) {
  int id = add_node("for");

  add_edge_implicit(id, node.iterator_id);
  node.start_expr->accept(*this);
  add_edge_top(id);
  node.end_expr->accept(*this);
  add_edge_top(id);
  node.block->accept(*this);
  add_edge_top(id);

  stack.push(id);
}
void TreeGenerator::visit(AssignStatementAST &node) {
  int id = add_node(operator_type_to_string(node.op));

  node.lloc->accept(*this);
  add_edge_top(id);
  node.rval->accept(*this);
  add_edge_top(id);

  stack.push(id);
}

void TreeGenerator::visit(StatementBlockAST &node) {
  int id = add_node("{statement-block}");

  int var_decl_id = add_node("variables");
  add_edge(id, var_decl_id);
  for (auto var : node.variable_declarations) {
    add_edge_implicit(var_decl_id, var->to_string());
  }

  int stm = add_node("statements");
  add_edge(id, stm);
  for (auto s : node.statements) {
    s->accept(*this);
    add_edge_top(stm);
  }

  stack.push(id);
}

void TreeGenerator::visit(MethodDeclarationAST &node) {
  int id =
      add_node(value_type_to_string(node.return_type) + " " + node.name + "()");

  int params = add_node("parameters");
  add_edge(id, params);
  for (auto param : node.parameters) {
    add_edge_implicit(params, param->to_string());
  }

  node.body->accept(*this);
  add_edge_top(id);

  stack.push(id);
}
void TreeGenerator::visit(MethodCallAST &node) {
  int id = add_node("call: " + node.id);

  for (auto arg : node.arguments) {
    arg->accept(*this);
    add_edge_top(id);
  }

  stack.push(id);
}
void TreeGenerator::visit(CalloutCallAST &node) {
  visit(dynamic_cast<MethodCallAST &>(node));
}

void TreeGenerator::visit(ProgramAST &node) {
  int id = add_node("Program");
  for (auto method : node.methods) {
    method->accept(*this);
    add_edge_top(id);
  }

  int var_decl_id = add_node("Variables");
  add_edge(id, var_decl_id);
  for (auto var : node.global_variables) {
    add_edge_implicit(var_decl_id, var->to_string());
  }
}