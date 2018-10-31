#include "literals.hh"
#include "../visitors/visitor.hh"

// void IntegerLiteral::IntegerLiteral(

void IntegerLiteral::accept(ASTvisitor& V) {
	V.visit(*this);
}

void BooleanLiteral::accept(ASTvisitor& V) {
	V.visit(*this);
}

void StringLiteral::accept(ASTvisitor& V) {
	V.visit(*this);
}
