#include "methods.hh"
#include "../visitors/visitor.hh"

MethodDeclarationAST::~MethodDeclarationAST() {
  delete body;
  for (auto param : parameters) {
    delete param;
  }
}
void MethodDeclarationAST::accept(ASTvisitor &V) { V.visit(*this); }
std::string MethodDeclarationAST::to_string() {
  std::string res = value_type_to_string(return_type) + " " + name;
  res += "(";
  bool first = true;
  for (auto param : parameters) {
    if (!first)
      res += ", ";
    res += value_type_to_string(param->type);
    first = false;
  }
  res += ")";
  return res;
}

MethodCallAST::~MethodCallAST() {
  for (auto arg : this->arguments) {
    delete arg;
  }
}
void MethodCallAST::accept(ASTvisitor &V) { V.visit(*this); }

void CalloutCallAST::accept(ASTvisitor &V) { V.visit(*this); }
