#include "literals.hh"
#include "../visitors/visitor.hh"

#include "variables.hh"

void LiteralAST::accept(ASTvisitor &V) { V.visit(*this); }

void IntegerLiteralAST::accept(ASTvisitor &V) { V.visit(*this); }

void BooleanLiteralAST::accept(ASTvisitor &V) { V.visit(*this); }

void StringLiteralAST::accept(ASTvisitor &V) { V.visit(*this); }
