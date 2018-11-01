#include "blocks.hh"
#include "../visitors/visitor.hh"

StatementBlock::~StatementBlock() {
	for (auto statement: statements) {
		delete statement;
	}
}
void StatementBlock::accept(ASTvisitor& V) {
	V.visit(*this);
}
