#pragma once

#include <vector>

#include "ast.hh"
#include "variables.hh"
#include "methods.hh"

class Program : public ASTnode {
public:
	Program(const std::vector<VariableDeclaration *>& _glob_vars,
			const std::vector<MethodDeclaration *>& _methods)
	: global_variables(_glob_vars), methods(_methods) {} 
	virtual ~Program();

	virtual void accept(ASTvisitor& V);

	std::vector<VariableDeclaration *> global_variables;
	std::vector<MethodDeclaration *> methods;
};