#include <iostream>
#include <string>

#include "driver.hh"
#include "parser.tab.hh"
#include "scanner.hh"

#include "ast/ast.hh"
#include "ast/blocks.hh"
#include "ast/literals.hh"
#include "ast/methods.hh"
#include "ast/operators.hh"
#include "ast/program.hh"
#include "ast/statements.hh"
#include "ast/variables.hh"

using Decaf::Driver;

void Driver::syntax_error(const std::string &loc, const std::string &err) {
  std::cerr << "[" << loc << "] "
            << "error: " << err << std::endl;
}
