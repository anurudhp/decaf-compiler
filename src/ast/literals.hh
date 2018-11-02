#pragma once

#include <string>

#include "ast.hh"
#include "variables.hh"

class Literal : public ASTnode {
public:
	Literal(ValueType _type)
	: type(_type) {}
	virtual ~Literal() = default;

	virtual void accept(ASTvisitor& V);
	
	ValueType type;
};

class IntegerLiteral: public Literal {
public:
	IntegerLiteral(int _value)
	: Literal(ValueType::INT), value(_value) {}
	virtual ~IntegerLiteral() = default;
	
	virtual void accept(ASTvisitor& V);

	int value;
};

class BooleanLiteral: public Literal {
public:
	BooleanLiteral(bool _value)
	: Literal(ValueType::BOOL), value(_value) {}
	virtual ~BooleanLiteral() = default;

	virtual void accept(ASTvisitor& V);

	bool value;
};

class StringLiteral: public Literal {
public:
	StringLiteral(std::string _value)
	: Literal(ValueType::STRING), value(_value) {}
	virtual ~StringLiteral() = default;

	virtual void accept(ASTvisitor& V);

	std::string value;
};