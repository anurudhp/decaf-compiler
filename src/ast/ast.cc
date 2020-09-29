#include <sstream>

#include "ast.hh"

void BaseAST::set_location(const std::string &loc) { this->location = loc; }

void BaseAST::set_location(const Decaf::location &loc) {
  std::stringstream ss;
  ss << loc;
  this->location = ss.str();
}

std::string BaseAST::to_string() { return ""; }
