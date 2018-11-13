#include <cstring>
#include <cstdarg>
#include <iostream>

#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include "codegen.hh"
#include "../ast/ast.hh"
#include "../ast/literals.hh"
#include "../ast/operators.hh"
#include "../ast/variables.hh"
#include "../ast/statements.hh"
#include "../ast/blocks.hh"
#include "../ast/methods.hh"
#include "../ast/program.hh"
#include "../exceptions.hh"

/*** CodeGenerator ***/
CodeGenerator::CodeGenerator(std::string name)
: builder(context) {
	module = new llvm::Module(name, context);
}
CodeGenerator::~CodeGenerator() {
	delete module;
}

void CodeGenerator::generate(BaseAST& root) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}

void CodeGenerator::print(std::ostream& out) {
	module->print(llvm::errs(), nullptr);
	throw unimplemented_error(__PRETTY_FUNCTION__);
}

void CodeGenerator::error(const std::string& fmt, ...) {
	static const int SIZE = 300;
	std::string err(SIZE, '\0');

	va_list args;
	va_start(args, fmt);
	vsnprintf(&err[0], SIZE, fmt.c_str(), args);
	va_end(args);

	std::cerr << err << '\n';
}

// visits:
void CodeGenerator::visit(BaseAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}

// literals.hh
void CodeGenerator::visit(LiteralAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(IntegerLiteralAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(BooleanLiteralAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(StringLiteralAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}

// variables.hh
void CodeGenerator::visit(LocationAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(VariableLocationAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(ArrayLocationAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}

// operators.hh
void CodeGenerator::visit(UnaryOperatorAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(BinaryOperatorAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(ArithBinOperatorAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(CondBinOperatorAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(RelBinOperatorAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(EqBinOperatorAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(UnaryMinusAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(UnaryNotAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}

// statements.hh
void CodeGenerator::visit(ReturnStatementAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(BreakStatementAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(ContinueStatementAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(IfStatementAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(ForStatementAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(AssignStatementAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}

// blocks.hh
void CodeGenerator::visit(StatementBlockAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}

// methods.hh
void CodeGenerator::visit(MethodDeclarationAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(MethodCallAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}
void CodeGenerator::visit(CalloutCallAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}

// program.hh
void CodeGenerator::visit(ProgramAST& node) {
	throw unimplemented_error(__PRETTY_FUNCTION__);
}