#pragma once

class ASTnode;

class ASTvisitor {
public:
	ASTvisitor() = default;
	virtual ~ASTvisitor();

	virtual void visit(ASTnode& node) = 0;
};