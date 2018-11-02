#pragma once

#include <string>

#include "../ast/ast.hh"

class ASTvisitor {
public:
	ASTvisitor() = default;
	virtual ~ASTvisitor() = default;

	virtual void visit(ASTnode& node) = 0;

	// literals.hh
	virtual void visit(Literal& node) = 0;
	virtual void visit(IntegerLiteral& node) = 0;
	virtual void visit(BooleanLiteral& node) = 0;
	virtual void visit(StringLiteral& node) = 0;

	// variables.hh
	virtual void visit(Location& node) = 0;
	virtual void visit(VariableLocation& node) = 0;
	virtual void visit(ArrayLocation& node) = 0;

	// operators.hh
	virtual void visit(UnaryOperator& node) = 0;
	virtual void visit(BinaryOperator& node) = 0;
	virtual void visit(ArithBinOperator& node) = 0;
	virtual void visit(CondBinOperator& node) = 0;
	virtual void visit(RelBinOperator& node) = 0;
	virtual void visit(EqBinOperator& node) = 0;
	virtual void visit(UnaryMinus& node) = 0;
	virtual void visit(UnaryNot& node) = 0;

	// statements.hh
	virtual void visit(ReturnStatement& node) = 0;
	virtual void visit(BreakStatement& node) = 0;
	virtual void visit(ContinueStatement& node) = 0;
	virtual void visit(IfStatement& node) = 0;
	virtual void visit(ForStatement& node) = 0;
	virtual void visit(AssignStatement& node) = 0;

	// blocks.hh
	virtual void visit(StatementBlock& node) = 0;

	// methods.hh
	virtual void visit(MethodDeclaration& node) = 0;
	virtual void visit(MethodCall& node) = 0;
	virtual void visit(CalloutCall& node) = 0;

	// program.hh
	virtual void visit(Program& node) = 0;
};