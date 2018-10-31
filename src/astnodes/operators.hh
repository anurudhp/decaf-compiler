#pragma once

#include <string>
#include "ast.hh"

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

/*** Parent Operator Classes ***/

class UnaryOperator : public ASTnode {
public:
	OperatorType op;
	ASTnode *val;

	UnaryOperator(OperatorType _op, ASTnode *_val)
	: op(_op), val(_val) {}

	virtual void accept(ASTvisitor& V);
};

class BinaryOperator : public ASTnode {
public:
	OperatorType op;
	ASTnode *lval, *rval;

	BinaryOperator(OperatorType _op, ASTnode *_lval, ASTnode *_rval)
	: op(_op), lval(_lval), rval(_rval) {}

	virtual void accept(ASTvisitor& V);
};

/*** Derived Operator Classes ***/

// ADD SUB MUL DIV MOD
class ArithBinOperator : public BinaryOperator {
public:
	ArithBinOperator(OperatorType _op, ASTnode *_lval, ASTnode *_rval)
	: BinaryOperator(_op, _lval, _rval) {}

	virtual void accept(ASTvisitor& V);
};

// AND OR
class CondBinOperator : public BinaryOperator {
public:
	CondBinOperator(OperatorType _op, ASTnode *_lval, ASTnode *_rval)
	: BinaryOperator(_op, _lval, _rval) {}

	virtual void accept(ASTvisitor& V);
};

// LE LT GE GT
class RelBinOperator : public BinaryOperator {
public:
	RelBinOperator(OperatorType _op, ASTnode *_lval, ASTnode *_rval)
	: BinaryOperator(_op, _lval, _rval) {}

	virtual void accept(ASTvisitor& V);
};

// EQ NE
class EqBinOperator : public BinaryOperator {
public:
	EqBinOperator(OperatorType _op, ASTnode *_lval, ASTnode *_rval)
	: BinaryOperator(_op, _lval, _rval) {}

	virtual void accept(ASTvisitor& V);
};

// UMINUS
class UnaryMinus : public UnaryOperator {
public:
	UnaryMinus(ASTnode *val)
	: UnaryOperator(OperatorType::UMINUS, val) {}

	virtual void accept(ASTvisitor& V);
};

// NOT
class UnaryNot : public UnaryOperator {
public:
	UnaryNot(ASTnode *val)
	: UnaryOperator(OperatorType::NOT, val) {}

	virtual void accept(ASTvisitor& V);
};

// Assignment 
class AssignOperator : public BinaryOperator {
	public:
	AssignOperator(OperatorType _op, ASTnode *_lval, ASTnode *_rval)
	: BinaryOperator(_op, _lval, _rval) {}

	virtual void accept(ASTvisitor& V);
};
