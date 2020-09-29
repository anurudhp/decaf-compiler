#include "blocks.hh"
#include "../visitors/visitor.hh"

StatementBlockAST::~StatementBlockAST() {
  for (auto statement : statements) {
    delete statement;
  }
  for (auto var : variable_declarations) {
    delete var;
  }
}
void StatementBlockAST::accept(ASTvisitor &V) { V.visit(*this); }
