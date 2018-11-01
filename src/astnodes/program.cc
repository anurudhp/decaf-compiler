#include "program.hh"
#include "../visitors/visitor.hh"

void Program::accept(ASTvisitor& V) {
	V.visit(*this);
}
