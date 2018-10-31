#pragma once

#include <string>
#include "ast.hh"

class IntegerLiteral: public ASTnode {
public:
	IntegerLiteral(int _value): value(_value) {}
	
	virtual void accept(ASTvisitor& V);

private:
	int value;
};

class BooleanLiteral: public ASTnode {
public:
	BooleanLiteral(bool _value): value(_value) {}

	virtual void accept(ASTvisitor& V);

private:
	bool value;
};

class StringLiteral: public ASTnode {
public:
	StringLiteral(std::string _value): value(_value) {}

	virtual void accept(ASTvisitor& V);

private:
	std::string value;
};