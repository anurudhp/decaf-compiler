#include "blocks.hh"
#include "../visitors/visitor.hh"

StatementBlock::~StatementBlock() {
	for (auto statement: statements) {
		delete statement;
	}
	for (auto var: variable_declarations) {
		delete var;
	}
}
void StatementBlock::accept(ASTvisitor& V) {
	V.visit(*this);
}
