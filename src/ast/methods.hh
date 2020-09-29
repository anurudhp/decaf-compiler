#pragma once

#include "ast.hh"
#include "blocks.hh"
#include "variables.hh"

// Method declarations
class MethodDeclarationAST : public BaseAST {
public:
  MethodDeclarationAST(ValueType _rtype, const std::string &_name,
                       const std::vector<VariableDeclarationAST *> &_params,
                       StatementBlockAST *_body)
      : name(_name), return_type(_rtype), parameters(_params), body(_body) {}
  virtual ~MethodDeclarationAST();

  virtual void accept(ASTvisitor &V);
  virtual std::string to_string();

  std::string name;
  ValueType return_type;
  std::vector<VariableDeclarationAST *> parameters;
  StatementBlockAST *body;
};

// Method calls
class MethodCallAST : public BaseAST {
public:
  MethodCallAST(std::string _id, std::vector<BaseAST *> args)
      : id(_id), arguments(args) {}
  virtual ~MethodCallAST();

  virtual void accept(ASTvisitor &V);

  std::string id;
  std::vector<BaseAST *> arguments;
};

class CalloutCallAST : public MethodCallAST {
public:
  CalloutCallAST(std::string _id, std::vector<BaseAST *> args)
      : MethodCallAST(_id, args), arg_types(0) {}
  virtual ~CalloutCallAST() = default;

  virtual void accept(ASTvisitor &V);

  std::vector<ValueType> arg_types;
};