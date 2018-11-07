#pragma once

#include <vector>
#include <string>
#include <stack>
#include <map>
#include <ostream>

#include "visitor.hh"

class SemanticAnalyzer : public ASTvisitor {
public:
	SemanticAnalyzer() = default;
	virtual ~SemanticAnalyzer() = default;

	bool check(ASTnode& root);
	void display(std::ostream& out, const bool show_rules = false);

protected:

	class SymbolTable {
	public:
		SymbolTable(SemanticAnalyzer& _analyzer)
		: analyzer(_analyzer), variables(0), methods(), scope_depth(0) {}
		~SymbolTable() = default;

		SemanticAnalyzer& analyzer;

		// enter a new block, add an inner-most scope layer
		void block_start(); 
		// remove the inner-most scope layer
		void block_end();

		// add a variable to the table
		void add_variable(VariableDeclaration *variable);
		// add a function to the table
		void add_method(MethodDeclaration *method);

		// lookup:
		VariableDeclaration* lookup_variable(Location *varloc);
		MethodDeclaration* lookup_method(MethodCall *mcall);

		std::vector<
			std::map<std::string, VariableDeclaration *>
		> variables;
		std::map<std::string, MethodDeclaration *> methods;
		int scope_depth;
	};

	void log_error(const int error_type, const std::string& location, const std::string& fmt, ...);

private:
	SymbolTable *symbol_table;
	// std::stack<ValueType> type_stack;

	std::vector<std::pair<int, std::string>> errors;
	const static int BUFFER_LENGTH = 100;
	char _buffer[BUFFER_LENGTH];

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