#pragma once

namespace Decaf {
	class Driver {
	public:
		class Scanner *scanner;
		class Parser *parser;

		class ASTnode *root;
	};
}
