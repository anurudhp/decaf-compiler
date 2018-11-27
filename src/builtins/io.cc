#include <iostream>

extern "C" {
	// read_int: reads one integer from stdin
	int read_int() {
		int val;
		std::cin >> val;
		return val;
	}

	// write_int: prints one integer to stdout
	int write_int(int val) {
		std::cerr << val;
		return 0;
	}

	// write_bool: prints one boolean value to stdout
	int write_bool(bool val) {
		std::cerr << std::boolalpha << val << std::noboolalpha;
		return 0;
	}

	// write_char: prints one character to stdout
	int write_char(char val) {
		std::cerr << val;
		return 0;
	}

	// write_string: prints a string literal to stdout
	int write_string(const char* val) {
		std::cerr << val;
		return 0;
	}
}
