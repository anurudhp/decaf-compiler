#pragma once

#include <string>

#include "ast.hh"
#include "variables.hh"

class LiteralAST : public BaseAST {
public:
  LiteralAST(ValueType _type) : type(_type) {}
  virtual ~LiteralAST() = default;

  virtual void accept(ASTvisitor &V);

  ValueType type;
};

class IntegerLiteralAST : public LiteralAST {
public:
  IntegerLiteralAST(int _value) : LiteralAST(ValueType::INT), value(_value) {}
  virtual ~IntegerLiteralAST() = default;

  virtual void accept(ASTvisitor &V);

  int value;
};

class BooleanLiteralAST : public LiteralAST {
public:
  BooleanLiteralAST(bool _value) : LiteralAST(ValueType::BOOL), value(_value) {}
  virtual ~BooleanLiteralAST() = default;

  virtual void accept(ASTvisitor &V);

  bool value;
};

class StringLiteralAST : public LiteralAST {
public:
  StringLiteralAST(std::string _value)
      : LiteralAST(ValueType::STRING), value(_value) {}
  virtual ~StringLiteralAST() = default;

  virtual void accept(ASTvisitor &V);

  std::string value;
};