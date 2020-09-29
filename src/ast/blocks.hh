#pragma once

#include <vector>

#include "ast.hh"
#include "variables.hh"

class StatementBlockAST : public BaseAST {
public:
  StatementBlockAST(const std::vector<VariableDeclarationAST *> &_var_decl,
                    const std::vector<BaseAST *> &_stmts)
      : variable_declarations(_var_decl), statements(_stmts) {}
  virtual ~StatementBlockAST();

  virtual void accept(ASTvisitor &V);

  std::vector<VariableDeclarationAST *> variable_declarations;
  std::vector<BaseAST *> statements;
};
