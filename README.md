# Compiler for Decaf
Sai Anurudh Reddy Peduri (201502215)  
CSE419 Compilers (Monsoon 2018)  

### Phases
- [x] Phase 1: Grammar
- [x] Phase 2: AST construction, basic semantic rules
- [x] Phase 3: LLVM IR generation
- [x] optional: Semantic Analysis (full)

### Usage
- Build decaf: `make clean && make`
- generating IR: `bin/decaf <path/to/code.dcf> [--output=<path/to/output>]`
	- If no output file is specified, writes to stdout
- compiling code: `bin/compile <path/to/code.dcf> [clang-opts]`
	- Sample usage: `bin/compile test-programs/arraysum.dcf -o arraysum.out -O2`
	- Compiles using `clang++`

### Structure
- `scanner.hh`: header file for Flex Scanner class
- `scanner.ll`: Flex scanner
- `parser.yy`: Bison parser, and main function
- `compile.sh`: Wrapper script for compiling
- `driver.[hh, cc]`: driver class, passed to Bison parser
- `exceptions.hh`: Some exception classes for error handling in implementation
- `ast/`
	- `ast.[hh, cc]`: BaseAST abstract class, and forward declarations of all ASTnodes (for visitors)
	- `literals.[hh, cc]`: Int, bool and string literal nodes
	- `variables.[hh, cc]`: variable/array declaration and location ASTs
	- `operators.[hh, cc]`: unary/binary operator ASTs
	- `statements.[hh, cc]`: non expression statement ASTs: if, for, return, continue, break, assign
	- `blocks.[hh, cc]`: statement block AST
	- `methods.[hh, cc]`: method declaration, call, and callout AST
	- `program.[hh, cc]`: program AST: contains the full program
- `visitors`
	- `visitor.[hh, cc]`: ASTVisitor abstract class
	- `treegen.[hh, cc]`: Generates AST graph in mermaid.js format
	- `semantic_analyzer.[hh, cc]`: Semantic analyzer module
	- `codegen.[hh, cc]`: LLVM IR generation module
- `builtins`: Contains builtin functions, linked at runtime.
	- `io.cc`: Basic I/O functions

### Description
Uses visitor design pattern to achieve double dispatch. 

Todo:
- Improved runtime error messages, with locations
- Generalized callouts, supporting functions that take varargs