#include "statements.hh"
#include "../visitors/visitor.hh"

ReturnStatement::~ReturnStatement() {
	delete ret_expr;
}
void ReturnStatement::accept(ASTvisitor& V) {
	V.visit(*this);
}
void BreakStatement::accept(ASTvisitor& V) {
	V.visit(*this);
}
void ContinueStatement::accept(ASTvisitor& V) {
	V.visit(*this);
}

IfStatement::~IfStatement() {
	delete cond_expr;
	delete then_block;
	delete else_block;
}
void IfStatement::accept(ASTvisitor& V) {
	V.visit(*this);
}

ForStatement::~ForStatement() {
	delete start_expr;
	delete end_expr;
	delete block;
}
void ForStatement::accept(ASTvisitor& V) {
	V.visit(*this);
}

AssignStatement::~AssignStatement() {
	delete lloc;
	delete rval;
}
void AssignStatement::accept(ASTvisitor& V) {
	V.visit(*this);
}