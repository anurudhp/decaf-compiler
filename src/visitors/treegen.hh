#pragma once

#include <map>
#include <ostream>
#include <stack>
#include <string>
#include <vector>

#include "visitor.hh"

class TreeGenerator : public ASTvisitor {
public:
  TreeGenerator() = default;
  virtual ~TreeGenerator() = default;

  void generate(BaseAST &root, std::ostream &out);

protected:
  int add_node(std::string name);
  void add_edge(int u, int v);
  void add_edge_top(int u);
  void add_edge_implicit(int u, const std::string &v);

private:
  std::vector<std::vector<int>> out_edges;
  std::vector<std::string> node_names;

  std::stack<int> stack;

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
  virtual void visit(ArrayAddressAST &node) {}
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
