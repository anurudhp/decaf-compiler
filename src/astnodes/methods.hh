#pragma once

#include "ast.hh"
#include "variables.hh"
#include "blocks.hh"

class MethodDeclaration : public ASTnode {
public:
	MethodDeclaration(ValueType _rtype,
					  const std::string& _name,
					  const std::vector<VariableDeclaration>& _params, 
					  StatementBlock *_body)
	: name(_name), 
	  return_type(_rtype), 
	  parameters(_params),
	  body(_body) {}

	virtual void accept(ASTvisitor& V);

	std::string name;
	ValueType return_type;
	std::vector<VariableDeclaration> parameters;
	StatementBlock *body;
};
