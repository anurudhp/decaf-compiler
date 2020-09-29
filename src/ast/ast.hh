#pragma once

#include "../location.hh"
#include <string>

class ASTvisitor;

class BaseAST {
public:
  BaseAST() : location("??") {}
  virtual ~BaseAST() = default;

  void set_location(const std::string &loc);
  void set_location(const Decaf::location &loc);

  virtual void accept(ASTvisitor &V) = 0;
  virtual std::string to_string();

  std::string location;
};

// literals.hh
class LiteralAST;
class IntegerLiteralAST;
class BooleanLiteralAST;
class StringLiteralAST;

// variables.hh
class LocationAST;
class VariableLocationAST;
class ArrayLocationAST;
class ArrayAddressAST;

class VariableDeclarationAST;
class ArrayDeclarationAST;

enum class ValueType;

// operators.hh
class UnaryOperatorAST;
class BinaryOperatorAST;
class ArithBinOperatorAST;
class CondBinOperatorAST;
class RelBinOperatorAST;
class EqBinOperatorAST;
class UnaryMinusAST;
class UnaryNotAST;

enum class OperatorType;

// statements.hh
class ReturnStatementAST;
class BreakStatementAST;
class ContinueStatementAST;
class IfStatementAST;
class ForStatementAST;
class AssignStatementAST;

// blocks.hh
class StatementBlockAST;

// methods.hh
class MethodDeclarationAST;
class MethodCallAST;
class CalloutCallAST;

// program.hh
class ProgramAST;