#include "program.hh"
#include "../visitors/visitor.hh"

Program::~Program() {
	for (auto& method: methods) {
		delete method;
	}
}
void Program::accept(ASTvisitor& V) {
	V.visit(*this);
}
