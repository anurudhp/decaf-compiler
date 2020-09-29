#include "program.hh"
#include "../visitors/visitor.hh"

ProgramAST::~ProgramAST() {
  for (auto method : methods) {
    delete method;
  }
  for (auto var : global_variables) {
    delete var;
  }
}
void ProgramAST::accept(ASTvisitor &V) { V.visit(*this); }
