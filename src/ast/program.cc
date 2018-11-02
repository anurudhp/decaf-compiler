#include "program.hh"
#include "../visitors/visitor.hh"

Program::~Program() {
	for (auto method: methods) {
		delete method;
	}
	for (auto var: global_variables) {
		delete var;
	}
}
void Program::accept(ASTvisitor& V) {
	V.visit(*this);
}
