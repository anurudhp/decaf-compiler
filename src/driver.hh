#pragma once

namespace Decaf {
	// class Scanner;
	// class Parser;
	// class ASTnode;s

	class Driver {
	public:
		class Scanner *scanner;
		class Parser *parser;

		class ASTnode *root;
	};
}