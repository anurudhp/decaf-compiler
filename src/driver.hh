#pragma once

namespace Decaf {
	class Scanner;
	class Parser;

	class Driver {
	public:
		Scanner *scanner;
		Parser *parser;
	};
}