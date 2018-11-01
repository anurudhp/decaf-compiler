#pragma once

#include <string>

#include "ast.hh"

class ReturnStatement : public ASTnode {
public:
	ReturnStatement(ASTnode *expr = NULL)
	: ret_expr(expr) {}

	virtual void accept(ASTvisitor& V);

	ASTnode *ret_expr;
};

class BreakStatement : public ASTnode {
public:
	BreakStatement() {}

	virtual void accept(ASTvisitor& V);
};	

class ContinueStatement : public ASTnode {
public:
	ContinueStatement() {}

	virtual void accept(ASTvisitor& V);
};

class IfStatement : public ASTnode {
public:
	IfStatement(ASTnode *cond, ASTnode *tb, ASTnode *eb)
	: cond_expr(cond), then_block(tb), else_block(eb) {}

	virtual void accept(ASTvisitor& V);
	
	ASTnode *cond_expr, *then_block, *else_block;
};

class ForStatement : public ASTnode {
public:
	ForStatement(const std::string _id, ASTnode *st, ASTnode *en, ASTnode *b)
	: iterator_id(_id), start_expr(st), end_expr(en), block(b) {}

	virtual void accept(ASTvisitor& V);

	std::string iterator_id;
	ASTnode *start_expr, *end_expr, *block;
};

class AssignStatement : public ASTnode {
public:
	AssignStatement(class Location *loc, ASTnode *expr)
	: location(loc), rval(expr) {}

	virtual void accept(ASTvisitor& V);

	class Location *location;
	ASTnode *rval;
};