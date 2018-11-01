#include "statements.hh"
#include "../visitors/visitor.hh"

void ReturnStatement::accept(ASTvisitor& V) {
	V.visit(*this);
}
void BreakStatement::accept(ASTvisitor& V) {
	V.visit(*this);
}
void ContinueStatement::accept(ASTvisitor& V) {
	V.visit(*this);
}

void IfStatement::accept(ASTvisitor& V) {
	V.visit(*this);
}
void ForStatement::accept(ASTvisitor& V) {
	V.visit(*this);
}
void AssignStatement::accept(ASTvisitor& V) {
	V.visit(*this);
}
