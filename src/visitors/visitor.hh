#pragma once

#include <string>

#include "../ast/ast.hh"

class ASTvisitor {
public:
  ASTvisitor() = default;
  virtual ~ASTvisitor() = default;

  virtual void visit(BaseAST &node) = 0;

  // literals.hh
  virtual void visit(LiteralAST &node) = 0;
  virtual void visit(IntegerLiteralAST &node) = 0;
  virtual void visit(BooleanLiteralAST &node) = 0;
  virtual void visit(StringLiteralAST &node) = 0;

  // variables.hh
  virtual void visit(LocationAST &node) = 0;
  virtual void visit(VariableLocationAST &node) = 0;
  virtual void visit(ArrayLocationAST &node) = 0;
  virtual void visit(ArrayAddressAST &node) = 0;
  virtual void visit(VariableDeclarationAST &node) = 0;
  virtual void visit(ArrayDeclarationAST &node) = 0;

  // operators.hh
  virtual void visit(UnaryOperatorAST &node) = 0;
  virtual void visit(BinaryOperatorAST &node) = 0;
  virtual void visit(ArithBinOperatorAST &node) = 0;
  virtual void visit(CondBinOperatorAST &node) = 0;
  virtual void visit(RelBinOperatorAST &node) = 0;
  virtual void visit(EqBinOperatorAST &node) = 0;
  virtual void visit(UnaryMinusAST &node) = 0;
  virtual void visit(UnaryNotAST &node) = 0;

  // statements.hh
  virtual void visit(ReturnStatementAST &node) = 0;
  virtual void visit(BreakStatementAST &node) = 0;
  virtual void visit(ContinueStatementAST &node) = 0;
  virtual void visit(IfStatementAST &node) = 0;
  virtual void visit(ForStatementAST &node) = 0;
  virtual void visit(AssignStatementAST &node) = 0;

  // blocks.hh
  virtual void visit(StatementBlockAST &node) = 0;

  // methods.hh
  virtual void visit(MethodDeclarationAST &node) = 0;
  virtual void visit(MethodCallAST &node) = 0;
  virtual void visit(CalloutCallAST &node) = 0;

  // program.hh
  virtual void visit(ProgramAST &node) = 0;
};