#pragma once

#include <string>
#include <fstream>

namespace Decaf {
	class Scanner;
	class Parser;

	struct Driver {
		Scanner *scanner;
		Parser *parser;
	};
}