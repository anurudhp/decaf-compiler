#pragma once

#include <string>
#include <vector>

#include "ast.hh"

enum class ValueType {
  VOID,
  NONE,
  INT,
  BOOL,
  CHAR,
  STRING,
  INT_ARRAY,
  BOOL_ARRAY
};
std::string value_type_to_string(const ValueType &vt);

/*** Locations ***/

class LocationAST : public BaseAST {
public:
  LocationAST(std::string _id, BaseAST *_index, bool _is_lvalue)
      : id(_id), index_expr(_index), is_lvalue(_is_lvalue) {}
  virtual ~LocationAST();

  virtual void accept(ASTvisitor &V);

  std::string id;
  BaseAST *index_expr;
  bool is_lvalue;
};

class VariableLocationAST : public LocationAST {
public:
  VariableLocationAST(std::string id, bool is_lvalue = false)
      : LocationAST(id, nullptr, is_lvalue) {}
  virtual ~VariableLocationAST() = default;

  virtual void accept(ASTvisitor &V);
};

class ArrayLocationAST : public LocationAST {
public:
  ArrayLocationAST(std::string id, BaseAST *index, bool is_lvalue = false)
      : LocationAST(id, index, is_lvalue) {}
  virtual ~ArrayLocationAST() = default;

  virtual void accept(ASTvisitor &V);
};

class ArrayAddressAST : public LocationAST {
public:
  ArrayAddressAST(std::string id) : LocationAST(id, nullptr, false) {}
  virtual ~ArrayAddressAST() = default;

  virtual void accept(ASTvisitor &V);
};

/*** Variable Declarations ***/

class VariableDeclarationAST : public BaseAST {
public:
  std::string id;
  ValueType type;

  VariableDeclarationAST(std::string _id, ValueType _type = ValueType::NONE)
      : id(_id), type(_type) {}
  virtual ~VariableDeclarationAST() = default;

  virtual void accept(ASTvisitor &V);

  virtual std::string to_string();
};

class ArrayDeclarationAST : public VariableDeclarationAST {
public:
  int array_len;

  ArrayDeclarationAST(std::string _id, int _len,
                      ValueType _type = ValueType::NONE)
      : VariableDeclarationAST(_id, _type), array_len(_len) {}
  virtual ~ArrayDeclarationAST() = default;

  virtual void accept(ASTvisitor &V);

  virtual std::string to_string();
};
