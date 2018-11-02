#include <stdexcept>

#include "treegen.hh"
#include "../ast/ast.hh"
#include "../ast/literals.hh"
#include "../ast/operators.hh"
#include "../ast/variables.hh"
#include "../ast/statements.hh"
#include "../ast/blocks.hh"
#include "../ast/methods.hh"
#include "../ast/program.hh"
#include "../exceptions.hh"

void TreeGenerator::generate(ASTnode& root, std::ostream& out) {
	try {
		root.accept(*this);
	} catch(...) {
		throw;
	}
	out << "graph TD\n";
	for (int i = 0; i < (int) node_names.size(); i++) {
		out << i 
			<< "[\"" << node_names[i] << "\"]"
			<< "\n";
	}
	for (int u = 0; u < (int) out_edges.size(); u++) {
		for (int v: out_edges[u]) {
			out << u << "-->" << v << "\n";
		}
	}
}

int TreeGenerator::add_node(std::string name) {
	node_names.push_back(name);
	out_edges.emplace_back();
	return node_names.size() - 1;
}

void TreeGenerator::add_edge(int u, int v) {
	out_edges[u].push_back(v);
}
void TreeGenerator::add_edge_top(int u) {
	if (!stack.empty()) {
		out_edges[u].push_back(stack.top());
		stack.pop();
	}
}
void TreeGenerator::add_edge_implicit(int u, const std::string& sv) {
	int v = add_node(sv);
	add_edge(u, v);
}

// Visit methods
void TreeGenerator::visit(ASTnode& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}

void TreeGenerator::visit(Literal& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void TreeGenerator::visit(IntegerLiteral& node) {
	int id = add_node("[LIT] " + std::to_string(node.value));
	stack.push(id);
}
void TreeGenerator::visit(BooleanLiteral& node) {
	int id = add_node(std::string("[LIT] ") + (node.value ? "TRUE" : "FALSE"));
	stack.push(id);
}
void TreeGenerator::visit(StringLiteral& node) {
	int id = add_node("[LIT] \'" + node.value + "\'");
	stack.push(id);
}

void TreeGenerator::visit(Location& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void TreeGenerator::visit(VariableLocation& node) {
	int id = add_node(node.id);
	stack.push(id);
}
void TreeGenerator::visit(ArrayLocation& node) {
	int id = add_node(node.id + "[]");
	stack.push(id);
}

void TreeGenerator::visit(UnaryOperator& node) {
	int id = add_node(operator_type_to_string(node.op));

	node.val->accept(*this);
	add_edge_top(id);
	
	stack.push(id);
}
void TreeGenerator::visit(BinaryOperator& node) {
	int id = add_node(operator_type_to_string(node.op));

	node.lval->accept(*this);
	add_edge_top(id);
	node.rval->accept(*this);
	add_edge_top(id);
	
	stack.push(id);
}
void TreeGenerator::visit(ArithBinOperator& node) {
	visit(dynamic_cast<BinaryOperator&>(node));
}
void TreeGenerator::visit(CondBinOperator& node) {
	visit(dynamic_cast<BinaryOperator&>(node));
}
void TreeGenerator::visit(RelBinOperator& node) {
	visit(dynamic_cast<BinaryOperator&>(node));
}
void TreeGenerator::visit(EqBinOperator& node) {
	visit(dynamic_cast<BinaryOperator&>(node));
}
void TreeGenerator::visit(UnaryMinus& node) {
	visit(dynamic_cast<UnaryOperator&>(node));
}
void TreeGenerator::visit(UnaryNot& node) {
	visit(dynamic_cast<UnaryOperator&>(node));
}

void TreeGenerator::visit(ReturnStatement& node) {
	int id = add_node("return");
	if (node.ret_expr != NULL) {
		node.ret_expr->accept(*this);
		add_edge_top(id);
	}
	stack.push(id);
}
void TreeGenerator::visit(BreakStatement& node) {
	int id = add_node("break");
	stack.push(id);
}
void TreeGenerator::visit(ContinueStatement& node) {
	int id = add_node("continue");
	stack.push(id);
}
void TreeGenerator::visit(IfStatement& node) {
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
void TreeGenerator::visit(ForStatement& node) {
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
void TreeGenerator::visit(AssignStatement& node) {
	int id = add_node(operator_type_to_string(node.op));

	node.lloc->accept(*this);
	add_edge_top(id);
	node.rval->accept(*this);
	add_edge_top(id);
	
	stack.push(id);
}

void TreeGenerator::visit(StatementBlock& node) {
	int id = add_node("{statement-block}");

	int var_decl_id = add_node("variables");
	add_edge(id, var_decl_id);
	for (auto var: node.variable_declarations) {
		add_edge_implicit(var_decl_id, var->to_string());
	}

	int stm = add_node("statements");
	add_edge(id, stm);
	for (auto s: node.statements) {
		s->accept(*this);
		add_edge_top(stm);
	}

	stack.push(id);
}

void TreeGenerator::visit(MethodDeclaration& node) {
	int id = add_node(value_type_to_string(node.return_type) 
					  + " " + node.name + "()");

	int params = add_node("parameters");
	add_edge(id, params);
	for (auto param: node.parameters) {
		add_edge_implicit(params, param->to_string());
	}	

	node.body->accept(*this);
	add_edge_top(id);

	stack.push(id);
}
void TreeGenerator::visit(MethodCall& node) {
	int id = add_node("call: " + node.id);

	for (auto arg: node.arguments) {
		arg->accept(*this);
		add_edge_top(id);
	}

	stack.push(id);
}
void TreeGenerator::visit(CalloutCall& node) {
	visit(dynamic_cast<MethodCall&>(node));
}

void TreeGenerator::visit(Program& node) {
	int id = add_node("Program");
	for (auto method: node.methods) {
		method->accept(*this);
		add_edge_top(id);
	}

	int var_decl_id = add_node("Variables");
	add_edge(id, var_decl_id);
	for (auto var: node.global_variables) {
		add_edge_implicit(var_decl_id, var->to_string());
	}
}