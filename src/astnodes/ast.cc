#include <sstream>

#include "ast.hh"

void ASTnode::set_location(const std::string& loc) {
	this->location = loc;
}

void ASTnode::set_location(const Decaf::location& loc) {
	std::stringstream ss;
	ss << loc;
	this->location = ss.str();
}
