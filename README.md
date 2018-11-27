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
- compiling code: `bin/compile <path/to/code> [clang-opts]`
	- Sample usage: `bin/compile test-programs/arraysum.dcf -o a.out -O2`
