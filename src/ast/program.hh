#pragma once

#include <vector>

#include "ast.hh"
#include "methods.hh"
#include "variables.hh"

class ProgramAST : public BaseAST {
public:
  ProgramAST(const std::vector<VariableDeclarationAST *> &_glob_vars,
             const std::vector<MethodDeclarationAST *> &_methods)
      : global_variables(_glob_vars), methods(_methods) {}
  virtual ~ProgramAST();

  virtual void accept(ASTvisitor &V);

  std::vector<VariableDeclarationAST *> global_variables;
  std::vector<MethodDeclarationAST *> methods;
};