#pragma once

#include "ast.hh"
#include <string>

class Location : public ASTnode {
public:
	Location(std::string _id, ASTnode *_index)
	: id(_id), index_expr(_index) {}

	virtual void accept(ASTvisitor& V);

	std::string id;
	ASTnode *index_expr;
};

class VariableLocation : public Location {
public:
	VariableLocation(std::string id)
	: Location(id, NULL) {}

	virtual void accept(ASTvisitor& V);
};

class ArrayLocation : public Location {
public:
	ArrayLocation(std::string id, ASTnode *index)
	: Location(id, index) {}

	virtual void accept(ASTvisitor& V);
};