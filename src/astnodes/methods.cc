#include "methods.hh"
#include "../visitors/visitor.hh"

void MethodDeclaration::accept(ASTvisitor& V) {
	V.visit(*this);
}
