#include "variables.hh"
#include "../visitors/visitor.hh"

Location::~Location() {
	delete index_expr;
}
void Location::accept(ASTvisitor& V) {
	V.visit(*this);
}

void VariableLocation::accept(ASTvisitor& V) {
	V.visit(*this);
}
void ArrayLocation::accept(ASTvisitor& V) {
	V.visit(*this);
}
