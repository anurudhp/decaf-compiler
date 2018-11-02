#pragma once

#include <string>
#include <vector>
#include <map>
#include <stack>
#include <ostream>

#include "visitor.hh"

class TreeGenerator: public ASTvisitor {
public:
	TreeGenerator() = default;
	virtual ~TreeGenerator() = default;

	void generate(ASTnode& root, std::ostream& out);

private:
	int add_node(std::string name);
	void add_edge(int u, int v);
	void add_edge_top(int u);
	void add_edge_implicit(int u, const std::string& v);

	std::vector<std::vector<int>> out_edges;
	std::vector<std::string> node_names;

	std::stack<int> stack;

public:
	// visits:
	virtual void visit(ASTnode& node);

	// literals.hh
	virtual void visit(Literal& node);
	virtual void visit(IntegerLiteral& node);
	virtual void visit(BooleanLiteral& node);
	virtual void visit(StringLiteral& node);

	// variables.hh
	virtual void visit(Location& node);
	virtual void visit(VariableLocation& node);
	virtual void visit(ArrayLocation& node);

	// operators.hh
	virtual void visit(UnaryOperator& node);
	virtual void visit(BinaryOperator& node);
	virtual void visit(ArithBinOperator& node);
	virtual void visit(CondBinOperator& node);
	virtual void visit(RelBinOperator& node);
	virtual void visit(EqBinOperator& node);
	virtual void visit(UnaryMinus& node);
	virtual void visit(UnaryNot& node);

	// statements.hh
	virtual void visit(ReturnStatement& node);
	virtual void visit(BreakStatement& node);
	virtual void visit(ContinueStatement& node);
	virtual void visit(IfStatement& node);
	virtual void visit(ForStatement& node);
	virtual void visit(AssignStatement& node);

	// blocks.hh
	virtual void visit(StatementBlock& node);

	// methods.hh
	virtual void visit(MethodDeclaration& node);
	virtual void visit(MethodCall& node);
	virtual void visit(CalloutCall& node);

	// program.hh
	virtual void visit(Program& node);
};
