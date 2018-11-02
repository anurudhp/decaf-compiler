#pragma once

#include <string>

#include "ast.hh"
#include "variables.hh"
#include "operators.hh"

class ReturnStatement : public ASTnode {
public:
	ReturnStatement(ASTnode *expr = NULL)
	: ret_expr(expr) {}
	virtual ~ReturnStatement();

	virtual void accept(ASTvisitor& V);

	ASTnode *ret_expr;
};

class BreakStatement : public ASTnode {
public:
	BreakStatement() {}
	virtual ~BreakStatement() = default;

	virtual void accept(ASTvisitor& V);
};	

class ContinueStatement : public ASTnode {
public:
	ContinueStatement() {}
	virtual ~ContinueStatement() = default;

	virtual void accept(ASTvisitor& V);
};

class IfStatement : public ASTnode {
public:
	IfStatement(ASTnode *cond, ASTnode *tb, ASTnode *eb)
	: cond_expr(cond), then_block(tb), else_block(eb) {}
	virtual ~IfStatement();

	virtual void accept(ASTvisitor& V);
	
	ASTnode *cond_expr, *then_block, *else_block;
};

class ForStatement : public ASTnode {
public:
	ForStatement(const std::string _id, ASTnode *st, ASTnode *en, ASTnode *b)
	: iterator_id(_id), start_expr(st), end_expr(en), block(b) {}
	virtual ~ForStatement();

	virtual void accept(ASTvisitor& V);

	std::string iterator_id;
	ASTnode *start_expr, *end_expr, *block;
};

class AssignStatement : public ASTnode {
	public:
	AssignStatement(OperatorType _op, Location *_lloc, ASTnode *_rval)
	: op(_op), lloc(_lloc), rval(_rval) {}
	virtual ~AssignStatement();

	virtual void accept(ASTvisitor& V);

	OperatorType op;
	Location *lloc;
	ASTnode *rval;
};
