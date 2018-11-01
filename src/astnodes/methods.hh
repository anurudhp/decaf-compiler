#pragma once

#include "ast.hh"
#include "variables.hh"
#include "blocks.hh"

// Method declarations
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
	virtual ~MethodDeclaration();

	virtual void accept(ASTvisitor& V);

	std::string name;
	ValueType return_type;
	std::vector<VariableDeclaration> parameters;
	StatementBlock *body;
};

// Method calls
class MethodCall : public ASTnode {
public:
	MethodCall(std::string _id, std::vector<ASTnode *> args)
	: id(_id), arguments(args) {}
	virtual ~MethodCall();

	virtual void accept(ASTvisitor& V);
	
	std::string id;
	std::vector<ASTnode *> arguments;
};

class CalloutCall : public MethodCall {
public:
	CalloutCall(std::string _id, std::vector<ASTnode *> args)
	: MethodCall(_id, args) {}
	virtual ~CalloutCall() = default;
	
	virtual void accept(ASTvisitor& V);
};