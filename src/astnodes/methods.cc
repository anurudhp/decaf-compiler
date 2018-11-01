#include "methods.hh"
#include "../visitors/visitor.hh"

void MethodDeclaration::accept(ASTvisitor& V) {
	V.visit(*this);
}

void MethodCall::accept(ASTvisitor& V) {
	V.visit(*this);
}
void CalloutCall::accept(ASTvisitor& V) {
	V.visit(*this);
}
