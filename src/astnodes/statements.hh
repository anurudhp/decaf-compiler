#pragma once

#include "ast.hh"
#include <string>

class Statement : public ASTnode {
public:
	Statement(ASTnode *_expr)
	: expr(_expr) {}

	virtual void accept(ASTvisitor& V);

	ASTnode *expr;
};

class ReturnStatement : public Statement {
public:
	ReturnStatement(ASTnode *expr = NULL)
	: Statement(NULL), ret_expr(expr) {}

	virtual void accept(ASTvisitor& V);

	ASTnode *ret_expr;
};

class BreakStatement : public Statement {
public:
	BreakStatement()
	: Statement(NULL) {}

	virtual void accept(ASTvisitor& V);
};	

class ContinueStatement : public Statement {
public:
	ContinueStatement()
	: Statement(NULL) {}

	virtual void accept(ASTvisitor& V);
};

class IfStatement : public Statement {
public:
	IfStatement(ASTnode *cond, ASTnode *tb, ASTnode *eb)
	: Statement(NULL),
	 cond_expr(cond), then_block(tb), else_block(eb) {}

	virtual void accept(ASTvisitor& V);
	
	ASTnode *cond_expr, *then_block, *else_block;
};

class ForStatement : public Statement {
public:
	ForStatement(std::string _id, ASTnode *st, ASTnode *en, ASTnode *b)
	: Statement(NULL), iterator_id(_id), 
	 start_expr(st), end_expr(en), block(b) {}

	virtual void accept(ASTvisitor& V);

	std::string iterator_id;
	ASTnode *start_expr, *end_expr, *block;
};

class AssignStatement : public Statement {
public:
	AssignStatement(class Location *loc, ASTnode *expr)
	: Statement(NULL), location(loc), rval(expr) {}

	virtual void accept(ASTvisitor& V);

	class Location *location;
	ASTnode *rval;
};