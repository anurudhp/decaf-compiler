#pragma once

#include <string>

#include "ast.hh"
#include "variables.hh"

enum class OperatorType {
  // unary
  UMINUS,
  NOT,

  // binary (arithmetic)
  ADD,
  SUB,
  MUL,
  DIV,
  MOD,

  // binary (logical)
  AND,
  OR,

  // binary (relational)
  LE,
  LT,
  GE,
  GT,
  EQ,
  NE,

  // binary (assignment)
  ASSIGN,
  ASSIGN_ADD,
  ASSIGN_SUB
};

std::string operator_type_to_string(const OperatorType &op);

/*** Parent Operator Classes ***/

class UnaryOperatorAST : public BaseAST {
public:
  UnaryOperatorAST(OperatorType _op, BaseAST *_val) : op(_op), val(_val) {}
  virtual ~UnaryOperatorAST();

  virtual void accept(ASTvisitor &V);

  OperatorType op;
  BaseAST *val;
};

class BinaryOperatorAST : public BaseAST {
public:
  BinaryOperatorAST(OperatorType _op, BaseAST *_lval, BaseAST *_rval)
      : op(_op), lval(_lval), rval(_rval) {}
  virtual ~BinaryOperatorAST();

  virtual void accept(ASTvisitor &V);

  OperatorType op;
  BaseAST *lval, *rval;
};

/*** Derived Operator Classes ***/

// ADD SUB MUL DIV MOD
class ArithBinOperatorAST : public BinaryOperatorAST {
public:
  ArithBinOperatorAST(OperatorType _op, BaseAST *_lval, BaseAST *_rval)
      : BinaryOperatorAST(_op, _lval, _rval) {}
  virtual ~ArithBinOperatorAST() = default;

  virtual void accept(ASTvisitor &V);
};

// AND OR
class CondBinOperatorAST : public BinaryOperatorAST {
public:
  CondBinOperatorAST(OperatorType _op, BaseAST *_lval, BaseAST *_rval)
      : BinaryOperatorAST(_op, _lval, _rval) {}
  virtual ~CondBinOperatorAST() = default;

  virtual void accept(ASTvisitor &V);
};

// LE LT GE GT
class RelBinOperatorAST : public BinaryOperatorAST {
public:
  RelBinOperatorAST(OperatorType _op, BaseAST *_lval, BaseAST *_rval)
      : BinaryOperatorAST(_op, _lval, _rval) {}
  virtual ~RelBinOperatorAST() = default;

  virtual void accept(ASTvisitor &V);
};

// EQ NE
class EqBinOperatorAST : public BinaryOperatorAST {
public:
  EqBinOperatorAST(OperatorType _op, BaseAST *_lval, BaseAST *_rval)
      : BinaryOperatorAST(_op, _lval, _rval) {}
  virtual ~EqBinOperatorAST() = default;

  virtual void accept(ASTvisitor &V);
};

// UMINUS
class UnaryMinusAST : public UnaryOperatorAST {
public:
  UnaryMinusAST(BaseAST *val) : UnaryOperatorAST(OperatorType::UMINUS, val) {}
  virtual ~UnaryMinusAST() = default;

  virtual void accept(ASTvisitor &V);
};

// NOT
class UnaryNotAST : public UnaryOperatorAST {
public:
  UnaryNotAST(BaseAST *val) : UnaryOperatorAST(OperatorType::NOT, val) {}
  virtual ~UnaryNotAST() = default;

  virtual void accept(ASTvisitor &V);
};
