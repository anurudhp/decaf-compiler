#pragma once

#include <vector>
#include <string>
#include <stack>
#include <map>
#include <ostream>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "visitor.hh"

class CodeGenerator : public ASTvisitor {
public:
	CodeGenerator() = default;
	virtual ~CodeGenerator() = default;

public:
	// visits:
	virtual void visit(BaseAST& node);

	// literals.hh
	virtual void visit(LiteralAST& node);
	virtual void visit(IntegerLiteralAST& node);
	virtual void visit(BooleanLiteralAST& node);
	virtual void visit(StringLiteralAST& node);

	// variables.hh
	virtual void visit(LocationAST& node);
	virtual void visit(VariableLocationAST& node);
	virtual void visit(ArrayLocationAST& node);

	// operators.hh
	virtual void visit(UnaryOperatorAST& node);
	virtual void visit(BinaryOperatorAST& node);
	virtual void visit(ArithBinOperatorAST& node);
	virtual void visit(CondBinOperatorAST& node);
	virtual void visit(RelBinOperatorAST& node);
	virtual void visit(EqBinOperatorAST& node);
	virtual void visit(UnaryMinusAST& node);
	virtual void visit(UnaryNotAST& node);

	// statements.hh
	virtual void visit(ReturnStatementAST& node);
	virtual void visit(BreakStatementAST& node);
	virtual void visit(ContinueStatementAST& node);
	virtual void visit(IfStatementAST& node);
	virtual void visit(ForStatementAST& node);
	virtual void visit(AssignStatementAST& node);

	// blocks.hh
	virtual void visit(StatementBlockAST& node);

	// methods.hh
	virtual void visit(MethodDeclarationAST& node);
	virtual void visit(MethodCallAST& node);
	virtual void visit(CalloutCallAST& node);

	// program.hh
	virtual void visit(ProgramAST& node);
};