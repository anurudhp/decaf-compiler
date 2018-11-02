#pragma once

#include <vector>

#include "ast.hh"
#include "variables.hh"

class StatementBlock : public ASTnode {
public:
	StatementBlock(const std::vector<VariableDeclaration *>& _var_decl,
				   const std::vector<ASTnode *>& _stmts)
	: variable_declarations(_var_decl),
	  statements(_stmts) {}
	virtual ~StatementBlock();

	virtual void accept(ASTvisitor& V);

	std::vector<VariableDeclaration *> variable_declarations;
	std::vector<ASTnode *> statements;
};
