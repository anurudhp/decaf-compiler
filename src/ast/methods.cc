#include "methods.hh"
#include "../visitors/visitor.hh"

MethodDeclaration::~MethodDeclaration() {
	delete this->body;
	for (auto param: parameters) {
		delete param;
	}
}
void MethodDeclaration::accept(ASTvisitor& V) {
	V.visit(*this);
}

MethodCall::~MethodCall() {
	for (ASTnode* arg: this->arguments) {
		delete arg;
	}
}
void MethodCall::accept(ASTvisitor& V) {
	V.visit(*this);
}

void CalloutCall::accept(ASTvisitor& V) {
	V.visit(*this);
}
