#pragma once

class ASTvisitor;

class ASTnode {
public:
	virtual ~ASTnode() = default;

	virtual void accept(ASTvisitor& V) = 0;
};