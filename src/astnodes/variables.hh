#pragma once

#include <string>
#include <vector>

#include "ast.hh"

enum class ValueType {
	VOID, NONE,
	INT, BOOL, CHAR, STRING,
	INT_ARRAY, BOOL_ARRAY
};

/*** Locations ***/

class Location : public ASTnode {
public:
	Location(std::string _id, ASTnode *_index)
	: id(_id), index_expr(_index) {}
	virtual ~Location();

	virtual void accept(ASTvisitor& V);

	std::string id;
	ASTnode *index_expr;
};

class VariableLocation : public Location {
public:
	VariableLocation(std::string id)
	: Location(id, NULL) {}
	virtual ~VariableLocation() = default;

	virtual void accept(ASTvisitor& V);
};

class ArrayLocation : public Location {
public:
	ArrayLocation(std::string id, ASTnode *index)
	: Location(id, index) {}
	virtual ~ArrayLocation() = default;

	virtual void accept(ASTvisitor& V);
};

/*** Variable Declarations ***/

struct VariableDeclaration {
	std::string id;
	ValueType type;

	VariableDeclaration(std::string _id, ValueType _type = ValueType::NONE)
	: id(_id), type(_type) {}
};

struct ArrayDeclaration : public VariableDeclaration {
	int array_len;

	ArrayDeclaration(std::string _id, int _len, ValueType _type = ValueType::NONE)
	: VariableDeclaration(_id, _type), array_len(_len) {}
};

