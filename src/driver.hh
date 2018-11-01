#pragma once

namespace Decaf {
	class Driver {
	public:
		class Scanner *scanner;
		class Parser *parser;

		class ASTnode *root;
		void error(const std::string& loc, const std::string& err) {
			std::cerr << "[" << loc << "] " 
				  << "error: " << err << std::endl;
		}
	};
}
