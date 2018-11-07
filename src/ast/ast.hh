#pragma once

#include <string>
#include "../location.hh"

class ASTvisitor;

class ASTnode {
public:
	ASTnode(): location("??") {}
	virtual ~ASTnode() = default;

	void set_location(const std::string& loc);
	void set_location(const Decaf::location& loc);

	virtual void accept(ASTvisitor& V) = 0;

	std::string location;
};

// literals.hh
class Literal;
class IntegerLiteral;
class BooleanLiteral;
class StringLiteral;

// variables.hh
class Location;
class VariableLocation;
class ArrayLocation;
class VariableDeclaration; 
class ArrayDeclaration; 

enum class ValueType;

// operators.hh
class UnaryOperator;
class BinaryOperator;
class ArithBinOperator;
class CondBinOperator;
class RelBinOperator;
class EqBinOperator;
class UnaryMinus;
class UnaryNot;

enum class OperatorType;

// statements.hh
class ReturnStatement;
class BreakStatement;
class ContinueStatement;
class IfStatement;
class ForStatement;
class AssignStatement;

// blocks.hh
class StatementBlock;

// methods.hh
class MethodDeclaration;
class MethodCall;
class CalloutCall;

// program.hh
class Program;