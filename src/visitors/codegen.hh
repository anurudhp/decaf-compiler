#pragma once

#include <vector>
#include <string>
#include <stack>
#include <map>
#include <ostream>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/TargetSelect.h>

#include "visitor.hh"

class CodeGenerator : public ASTvisitor {
public:
	CodeGenerator(std::string name);
	virtual ~CodeGenerator();

	void generate(BaseAST& root);
	void print(const std::string& outf);

private:
	// LLVM objects
	llvm::LLVMContext context;
	llvm::Module *module;
	llvm::IRBuilder<> builder;

	// symbol table
	class SymbolTable {
		std::vector<std::map<std::string, llvm::AllocaInst*>> variables;

	public:
		void block_start();
		void block_end();
		void add_variable(std::string id, llvm::AllocaInst *alloca);
		llvm::AllocaInst *lookup_variable(std::string id);

		bool is_global_scope();
	} symbol_table;

	std::stack<llvm::Value *> return_stack;
	llvm::Value* get_return_stack_top(bool pop = true);
	llvm::Value* get_return(BaseAST& node);

	// jump blocks inside for: <increment-block, after-block>
	std::stack<std::pair<llvm::BasicBlock *, llvm::BasicBlock *>> for_jump_blocks;

	llvm::Type* get_llvm_type(ValueType ty);
	void add_builtin(std::string name, std::vector<ValueType> _params, ValueType ret);

	void error(const std::string& fmt, ...);

public:
	// visits:
	virtual void visit(BaseAST& node);

	// literals.hh
	virtual void visit(LiteralAST& node);
	virtual void visit(IntegerLiteralAST& node);
	virtual void visit(BooleanLiteralAST& node);
	virtual void visit(StringLiteralAST& node);

	// variables.hh
	virtual void visit(LocationAST& node);
	virtual void visit(VariableLocationAST& node);
	virtual void visit(ArrayLocationAST& node);
	virtual void visit(VariableDeclarationAST& node);
	virtual void visit(ArrayDeclarationAST& node);

	// operators.hh
	virtual void visit(UnaryOperatorAST& node);
	virtual void visit(BinaryOperatorAST& node);
	virtual void visit(ArithBinOperatorAST& node);
	virtual void visit(CondBinOperatorAST& node);
	virtual void visit(RelBinOperatorAST& node);
	virtual void visit(EqBinOperatorAST& node);
	virtual void visit(UnaryMinusAST& node);
	virtual void visit(UnaryNotAST& node);

	// statements.hh
	virtual void visit(ReturnStatementAST& node);
	virtual void visit(BreakStatementAST& node);
	virtual void visit(ContinueStatementAST& node);
	virtual void visit(IfStatementAST& node);
	virtual void visit(ForStatementAST& node);
	virtual void visit(AssignStatementAST& node);

	// blocks.hh
	virtual void visit(StatementBlockAST& node);

	// methods.hh
	virtual void visit(MethodDeclarationAST& node);
	virtual void visit(MethodCallAST& node);
	virtual void visit(CalloutCallAST& node);

	// program.hh
	virtual void visit(ProgramAST& node);
};