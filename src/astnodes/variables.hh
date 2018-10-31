#pragma once

#include "ast.hh"
#include <string>
#include <vector>

enum class ValueType {
	VOID, NONE,
	INT, BOOL, CHAR,
	INT_ARRAY, BOOL_ARRAY
};

/*** Locations ***/

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

/*** Variable Declarations ***/

/* TODO
struct Variable {
	std::string id;
	int array_len;

	Variable(std::string _id)
	: id(_id), array_len(0) {}

	Variable(std::string _id, ValueType _type, int len)
	: id(_id), array_len(len) {}
};

class VariableDeclaration : public ASTnode {
public:
	VariableDeclaration(ValueType _type, std::vector<Variable> _vars)
	: type(_type), variables(_vars) {}

	virtual void accept(ASTvisitor& V);

	ValueType type;
	std::vector<Variable> variables;
};
*/