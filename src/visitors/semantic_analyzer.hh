#pragma once

#include <map>
#include <ostream>
#include <stack>
#include <string>
#include <vector>

#include "visitor.hh"

class SemanticAnalyzer : public ASTvisitor {
public:
  SemanticAnalyzer() = default;
  virtual ~SemanticAnalyzer() = default;

  bool check(BaseAST &root);
  void display(std::ostream &out, const bool show_rules = false);

protected:
  class SymbolTable {
  public:
    SymbolTable(SemanticAnalyzer &_analyzer)
        : analyzer(_analyzer), variables(0), methods(), scope_depth(0),
          hold_depth(0) {}
    ~SymbolTable() = default;

    SemanticAnalyzer &analyzer;

    // enter a new block, add an inner-most scope layer
    void block_start();
    // remove the inner-most scope layer
    void block_end();

    // add a variable to the table
    void add_variable(VariableDeclarationAST *variable);
    void add_array(ArrayDeclarationAST *array);
    // add a function to the table
    void add_method(MethodDeclarationAST *method);

    // lookup:
    VariableDeclarationAST *lookup_variable(LocationAST *varloc);
    ArrayDeclarationAST *lookup_array_element(LocationAST *arrloc);
    MethodDeclarationAST *lookup_method(MethodCallAST *mcall);

    std::vector<std::map<std::string, VariableDeclarationAST *>> variables;
    std::map<std::string, ArrayDeclarationAST *> arrays;
    std::map<std::string, MethodDeclarationAST *> methods;
    int scope_depth, hold_depth;
  };

  bool _silent;
  void silent(bool f);
  void log_error(const int error_type, const std::string &location,
                 const std::string &fmt, ...);

private:
  SymbolTable *symbol_table;
  int for_loop_depth;
  MethodDeclarationAST *current_method;

  std::stack<ValueType> type_stack;
  ValueType get_top_type(bool pop = true);

  std::vector<std::pair<int, std::string>> errors;
  const static int BUFFER_LENGTH = 100;
  char _buffer[BUFFER_LENGTH];

public:
  // visits:
  virtual void visit(BaseAST &node);

  // literals.hh
  virtual void visit(LiteralAST &node);
  virtual void visit(IntegerLiteralAST &node);
  virtual void visit(BooleanLiteralAST &node);
  virtual void visit(StringLiteralAST &node);

  // variables.hh
  virtual void visit(LocationAST &node);
  virtual void visit(VariableLocationAST &node);
  virtual void visit(ArrayLocationAST &node);
  virtual void visit(ArrayAddressAST &node);
  virtual void visit(VariableDeclarationAST &node) {}
  virtual void visit(ArrayDeclarationAST &node) {}

  // operators.hh
  virtual void visit(UnaryOperatorAST &node);
  virtual void visit(BinaryOperatorAST &node);
  virtual void visit(ArithBinOperatorAST &node);
  virtual void visit(CondBinOperatorAST &node);
  virtual void visit(RelBinOperatorAST &node);
  virtual void visit(EqBinOperatorAST &node);
  virtual void visit(UnaryMinusAST &node);
  virtual void visit(UnaryNotAST &node);

  // statements.hh
  virtual void visit(ReturnStatementAST &node);
  virtual void visit(BreakStatementAST &node);
  virtual void visit(ContinueStatementAST &node);
  virtual void visit(IfStatementAST &node);
  virtual void visit(ForStatementAST &node);
  virtual void visit(AssignStatementAST &node);

  // blocks.hh
  virtual void visit(StatementBlockAST &node);

  // methods.hh
  virtual void visit(MethodDeclarationAST &node);
  virtual void visit(MethodCallAST &node);
  virtual void visit(CalloutCallAST &node);

  // program.hh
  virtual void visit(ProgramAST &node);
};