#pragma once

#include <string>

#include "ast.hh"
#include "operators.hh"
#include "variables.hh"

class ReturnStatementAST : public BaseAST {
public:
  ReturnStatementAST(BaseAST *expr = NULL) : ret_expr(expr) {}
  virtual ~ReturnStatementAST();

  virtual void accept(ASTvisitor &V);

  BaseAST *ret_expr;
};

class BreakStatementAST : public BaseAST {
public:
  BreakStatementAST() {}
  virtual ~BreakStatementAST() = default;

  virtual void accept(ASTvisitor &V);
};

class ContinueStatementAST : public BaseAST {
public:
  ContinueStatementAST() {}
  virtual ~ContinueStatementAST() = default;

  virtual void accept(ASTvisitor &V);
};

class IfStatementAST : public BaseAST {
public:
  IfStatementAST(BaseAST *cond, BaseAST *tb, BaseAST *eb)
      : cond_expr(cond), then_block(tb), else_block(eb) {}
  virtual ~IfStatementAST();

  virtual void accept(ASTvisitor &V);

  BaseAST *cond_expr, *then_block, *else_block;
};

class ForStatementAST : public BaseAST {
public:
  ForStatementAST(const std::string _id, BaseAST *st, BaseAST *en, BaseAST *b)
      : iterator_id(_id), start_expr(st), end_expr(en), block(b) {}
  virtual ~ForStatementAST();

  virtual void accept(ASTvisitor &V);

  std::string iterator_id;
  BaseAST *start_expr, *end_expr, *block;
};

class AssignStatementAST : public BaseAST {
public:
  AssignStatementAST(OperatorType _op, LocationAST *_lloc, BaseAST *_rval)
      : op(_op), lloc(_lloc), rval(_rval) {
    lloc->is_lvalue = true;
  }
  virtual ~AssignStatementAST();

  virtual void accept(ASTvisitor &V);

  OperatorType op;
  LocationAST *lloc;
  BaseAST *rval;
};
