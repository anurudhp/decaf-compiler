#include "operators.hh"
#include "../visitors/visitor.hh"

std::string operator_type_to_string(const OperatorType &op) {
  if (op == OperatorType::UMINUS)
    return "-";
  if (op == OperatorType::NOT)
    return "!";
  if (op == OperatorType::ADD)
    return "+";
  if (op == OperatorType::SUB)
    return "-";
  if (op == OperatorType::MUL)
    return "*";
  if (op == OperatorType::DIV)
    return "/";
  if (op == OperatorType::MOD)
    return "%";
  if (op == OperatorType::AND)
    return "&&";
  if (op == OperatorType::OR)
    return "||";
  if (op == OperatorType::LE)
    return "<=";
  if (op == OperatorType::LT)
    return "<";
  if (op == OperatorType::GE)
    return ">=";
  if (op == OperatorType::GT)
    return ">";
  if (op == OperatorType::EQ)
    return "==";
  if (op == OperatorType::NE)
    return "!=";
  if (op == OperatorType::ASSIGN)
    return "=";
  if (op == OperatorType::ASSIGN_ADD)
    return "+=";
  if (op == OperatorType::ASSIGN_SUB)
    return "-=";
  return "";
}

UnaryOperatorAST::~UnaryOperatorAST() { delete val; }
void UnaryOperatorAST::accept(ASTvisitor &V) { V.visit(*this); }

BinaryOperatorAST::~BinaryOperatorAST() {
  delete lval;
  delete rval;
}
void BinaryOperatorAST::accept(ASTvisitor &V) { V.visit(*this); }

//  Derived classes

void ArithBinOperatorAST::accept(ASTvisitor &V) { V.visit(*this); }
void CondBinOperatorAST::accept(ASTvisitor &V) { V.visit(*this); }
void RelBinOperatorAST::accept(ASTvisitor &V) { V.visit(*this); }
void EqBinOperatorAST::accept(ASTvisitor &V) { V.visit(*this); }

void UnaryMinusAST::accept(ASTvisitor &V) { V.visit(*this); }
void UnaryNotAST::accept(ASTvisitor &V) { V.visit(*this); }
