#pragma once

#include <string>
#include "../location.hh"

class ASTvisitor;

class ASTnode {
public:
	ASTnode(): location("??") {}
	virtual ~ASTnode() = default;

	void set_location(const std::string& loc);
	void set_location(const Decaf::location& loc);

	virtual void accept(ASTvisitor& V) = 0;

	std::string location;
};