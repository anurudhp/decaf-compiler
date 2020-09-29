#include "statements.hh"
#include "../visitors/visitor.hh"

ReturnStatementAST::~ReturnStatementAST() { delete ret_expr; }
void ReturnStatementAST::accept(ASTvisitor &V) { V.visit(*this); }
void BreakStatementAST::accept(ASTvisitor &V) { V.visit(*this); }
void ContinueStatementAST::accept(ASTvisitor &V) { V.visit(*this); }

IfStatementAST::~IfStatementAST() {
  delete cond_expr;
  delete then_block;
  delete else_block;
}
void IfStatementAST::accept(ASTvisitor &V) { V.visit(*this); }

ForStatementAST::~ForStatementAST() {
  delete start_expr;
  delete end_expr;
  delete block;
}
void ForStatementAST::accept(ASTvisitor &V) { V.visit(*this); }

AssignStatementAST::~AssignStatementAST() {
  delete lloc;
  delete rval;
}
void AssignStatementAST::accept(ASTvisitor &V) { V.visit(*this); }