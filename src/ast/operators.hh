#pragma once

#include <string>

#include "ast.hh"
#include "variables.hh"

enum class OperatorType {
	// unary
	UMINUS, NOT,

	// binary (arithmetic)
	ADD, SUB, MUL, DIV, MOD,

	// binary (logical)
	AND, OR,

	// binary (relational)
	LE, LT, GE, GT, 
	EQ, NE,

	// binary (assignment)
	ASSIGN, ASSIGN_ADD, ASSIGN_SUB
};

std::string operator_type_to_string(const OperatorType& op);

/*** Parent Operator Classes ***/

class UnaryOperator : public ASTnode {
public:
	UnaryOperator(OperatorType _op, ASTnode *_val)
	: op(_op), val(_val) {}
	virtual ~UnaryOperator();

	virtual void accept(ASTvisitor& V);
	
	OperatorType op;
	ASTnode *val;
};

class BinaryOperator : public ASTnode {
public:
	BinaryOperator(OperatorType _op, ASTnode *_lval, ASTnode *_rval)
	: op(_op), lval(_lval), rval(_rval) {}
	virtual ~BinaryOperator();

	virtual void accept(ASTvisitor& V);
	
	OperatorType op;
	ASTnode *lval, *rval;
};

/*** Derived Operator Classes ***/

// ADD SUB MUL DIV MOD
class ArithBinOperator : public BinaryOperator {
public:
	ArithBinOperator(OperatorType _op, ASTnode *_lval, ASTnode *_rval)
	: BinaryOperator(_op, _lval, _rval) {}
	virtual ~ArithBinOperator() = default;

	virtual void accept(ASTvisitor& V);
};

// AND OR
class CondBinOperator : public BinaryOperator {
public:
	CondBinOperator(OperatorType _op, ASTnode *_lval, ASTnode *_rval)
	: BinaryOperator(_op, _lval, _rval) {}
	virtual ~CondBinOperator() = default;

	virtual void accept(ASTvisitor& V);
};

// LE LT GE GT
class RelBinOperator : public BinaryOperator {
public:
	RelBinOperator(OperatorType _op, ASTnode *_lval, ASTnode *_rval)
	: BinaryOperator(_op, _lval, _rval) {}
	virtual ~RelBinOperator() = default;

	virtual void accept(ASTvisitor& V);
};

// EQ NE
class EqBinOperator : public BinaryOperator {
public:
	EqBinOperator(OperatorType _op, ASTnode *_lval, ASTnode *_rval)
	: BinaryOperator(_op, _lval, _rval) {}
	virtual ~EqBinOperator() = default;

	virtual void accept(ASTvisitor& V);
};

// UMINUS
class UnaryMinus : public UnaryOperator {
public:
	UnaryMinus(ASTnode *val)
	: UnaryOperator(OperatorType::UMINUS, val) {}
	virtual ~UnaryMinus() = default;

	virtual void accept(ASTvisitor& V);
};

// NOT
class UnaryNot : public UnaryOperator {
public:
	UnaryNot(ASTnode *val)
	: UnaryOperator(OperatorType::NOT, val) {}
	virtual ~UnaryNot() = default;

	virtual void accept(ASTvisitor& V);
};
