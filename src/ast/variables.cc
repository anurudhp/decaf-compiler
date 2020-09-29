#include "variables.hh"
#include "../visitors/visitor.hh"

std::string value_type_to_string(const ValueType &vt) {
  if (vt == ValueType::VOID)
    return "void";
  if (vt == ValueType::INT)
    return "int";
  if (vt == ValueType::BOOL)
    return "boolean";
  if (vt == ValueType::CHAR)
    return "char";
  if (vt == ValueType::STRING)
    return "string";
  if (vt == ValueType::INT_ARRAY)
    return "int_array";
  if (vt == ValueType::BOOL_ARRAY)
    return "bool_array";
  return "none";
}

LocationAST::~LocationAST() { delete index_expr; }
void LocationAST::accept(ASTvisitor &V) { V.visit(*this); }

void VariableLocationAST::accept(ASTvisitor &V) { V.visit(*this); }
void ArrayLocationAST::accept(ASTvisitor &V) { V.visit(*this); }
void ArrayAddressAST::accept(ASTvisitor &V) { V.visit(*this); }

void VariableDeclarationAST::accept(ASTvisitor &V) { V.visit(*this); }
std::string VariableDeclarationAST::to_string() {
  return value_type_to_string(type) + " " + id;
}

void ArrayDeclarationAST::accept(ASTvisitor &V) { V.visit(*this); }
std::string ArrayDeclarationAST::to_string() {
  return VariableDeclarationAST::to_string() + "[" + std::to_string(array_len) +
         "]";
}