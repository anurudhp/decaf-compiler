#include "blocks.hh"
#include "../visitors/visitor.hh"

void StatementBlock::accept(ASTvisitor& V) {
	V.visit(*this);
}
