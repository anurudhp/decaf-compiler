#include "literals.hh"
#include "../visitors/visitor.hh"

#include "variables.hh"

void Literal::accept(ASTvisitor& V) {
	V.visit(*this);
}

void IntegerLiteral::accept(ASTvisitor& V) {
	V.visit(*this);
}

void BooleanLiteral::accept(ASTvisitor& V) {
	V.visit(*this);
}

void StringLiteral::accept(ASTvisitor& V) {
	V.visit(*this);
}
