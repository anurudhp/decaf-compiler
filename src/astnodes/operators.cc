#include "operators.hh"
#include "../visitors/visitor.hh"

void UnaryOperator::accept(ASTvisitor& V) {
	V.visit(*this);
}

void BinaryOperator::accept(ASTvisitor& V) {
	V.visit(*this);
}

//  Derived classes

void ArithBinOperator::accept(ASTvisitor& V) {
	V.visit(*this);
}
void CondBinOperator::accept(ASTvisitor& V) {
	V.visit(*this);
}
void RelBinOperator::accept(ASTvisitor& V) {
	V.visit(*this);
}
void EqBinOperator::accept(ASTvisitor& V) {
	V.visit(*this);
}

void UnaryMinus::accept(ASTvisitor& V) {
	V.visit(*this);
}
void UnaryNot::accept(ASTvisitor& V) {
	V.visit(*this);
}

void AssignOperator::accept(ASTvisitor& V) {
	V.visit(*this);
}