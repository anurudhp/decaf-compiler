#include "variables.hh"
#include "../visitors/visitor.hh"

void Location::accept(ASTvisitor& V) {
	V.visit(*this);
}

void VariableLocation::accept(ASTvisitor& V) {
	V.visit(*this);
}
void ArrayLocation::accept(ASTvisitor& V) {
	V.visit(*this);
}
