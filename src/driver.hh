#pragma once

namespace Decaf {
	class Driver {
	public:
		class Scanner *scanner;
		class Parser *parser;

		class ASTnode *root;

		Driver() = default;
		~Driver() = default;

		void syntax_error(const std::string& loc, const std::string& err);
	};
}
