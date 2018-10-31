#pragma once

enum class static_type {
	INT, BOOL, STRING, VOID, NONE 
};

class ASTvisitor;

class ASTnode {
public:
	virtual ~ASTnode() = default;

	virtual void accept(ASTvisitor& V) = 0;
};