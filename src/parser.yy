%debug 
%skeleton "lalr1.cc"
%defines
%define api.namespace {Decaf}
%define parser_class_name {Parser}
%locations 

%code requires {
	namespace Decaf {
		class Scanner;
		class Driver;
	}
	enum class OperatorType;
	enum class ValueType;

	class BaseAST;
	class LocationAST;
	class VariableDeclarationAST;
	class StatementBlockAST;
	class MethodDeclarationAST;
}

%parse-param {Driver& driver}

%code {
	#include <iostream>	
	#include <fstream>
	#include <string>
	#include <cassert>
	#include <sstream>

	#include "scanner.hh"
	#include "driver.hh"

	// AST node classes
	#include "ast/ast.hh"
	#include "ast/literals.hh"
	#include "ast/operators.hh"
	#include "ast/variables.hh"
	#include "ast/statements.hh"
	#include "ast/blocks.hh"
	#include "ast/methods.hh"
	#include "ast/program.hh"

	// Visitor classes
	#include "visitors/visitor.hh"
	#include "visitors/treegen.hh"
	#include "visitors/semantic_analyzer.hh"
	#include "visitors/codegen.hh"

	#undef yylex
	#define yylex driver.scanner->yylex
}

%union {
	// primitive types
	int ival;
	bool bval;
	char *sval;

	// enum/struct types
	OperatorType op;
	ValueType vt;

	// AST types
	BaseAST *ast;
	VariableDeclarationAST *decl;
	LocationAST *loc;
	StatementBlockAST *block;
	MethodDeclarationAST *method;

	// vectors
	std::vector<VariableDeclarationAST *> *var_decl;
	std::vector<BaseAST *> *nodes;
	std::vector<MethodDeclarationAST *> *methods;
}

 /**********************
 ***    TOKENS       *** 
 **********************/
%token END 0

 /* keywords */
%token CLASS IF ELSE FOR BREAK CONTINUE RETURN CALLOUT
%token INT BOOL VOID

 /* literals */
%token <ival> INT_LIT CHAR_LIT 
%token <bval> BOOL_LIT 
%token <sval> STRING_LIT

%token <sval> ID

 /* operators */
%token ADD SUB MUL DIV MOD
%token AND OR NOT
%token GT GE LT LE EQ NE
%token ASSIGN ASSIGN_ADD ASSIGN_SUB
%token '(' ')' '[' ']' '{' '}' ',' ';'

 /* non-terminals */
%type <op> assign_op
%type <vt> type
%type <decl> param glob_var_decl

%type <ast> literal expr statement method_call
%type <ast> arg callout_arg
%type <block> block else_block
%type <loc> location
%type <method> method_decl

%type <var_decl> var_decl_list var_decl var_list 
%type <var_decl> param_list param_list_non_empty
%type <var_decl> glob_var_decl_list field_decl field_decl_list
%type <nodes> statement_list callout_arg_list arg_list args
%type <methods> method_decl_list

 /* priorities and precedence */
%left OR
%left AND
%nonassoc EQ NE 
%nonassoc LE LT GE GT
%left ADD SUB
%left MUL DIV MOD
%nonassoc NOT
%nonassoc UMINUS 

%start program

%%
program : CLASS ID '{' field_decl_list method_decl_list '}' {
																if (std::string($2) != "Program") {
																	driver.parser->error(@2, "Class name has to be `Program`");
																	driver.root = NULL;
																	return 1;
																}

																driver.root = new ProgramAST(*$4, *$5);
																delete $4;
																delete $5;
															}
        ;

/* Field(data) declarations */

type : INT { $$ = ValueType::INT; }
	 | BOOL { $$ = ValueType::BOOL; }
	 ;

field_decl_list : field_decl_list field_decl {
												$$ = $1;
												$$->insert($$->end(), $2->rbegin(), $2->rend());
												delete $2;
											}
		  		| %empty {
		  					$$ = new std::vector<VariableDeclarationAST *>();
		  				}
		  		;
field_decl : type glob_var_decl_list ';' {
											$$ = $2;
											for (auto& elem: *$$) {
												elem->type = $1;
											}
										}
		   ;

glob_var_decl_list : glob_var_decl {
										$$ = new std::vector<VariableDeclarationAST *>();
										$$->push_back($1); 
								}
				   | glob_var_decl ',' glob_var_decl_list {
				   											  $$ = $3;
				   											  $$->push_back($1);
														}
				   ;
glob_var_decl : ID { 
						$$ = new VariableDeclarationAST(std::string($1)); 
						$$->set_location(@$);
				}
			  | ID '[' INT_LIT ']' { 
			  							$$ = new ArrayDeclarationAST(std::string(std::string($1)), $3); 
			  							$$->set_location(@$);
			  					}
			  ;

/* Function(method) declarations */
method_decl_list : method_decl_list method_decl {
													$$ = $1;
													$$->push_back($2);
												}
				 | method_decl 	{
				 					$$ = new std::vector<MethodDeclarationAST *>();
				 					$$->push_back($1);
				 				}
				 ;
method_decl : type ID '(' param_list ')' block {
													std::reverse($4->begin(), $4->end());
													$$ = new MethodDeclarationAST($1, $2, *$4, $6);
													$$->set_location(@2);
													delete $4;
											}
			| VOID ID '(' param_list ')' block {
													std::reverse($4->begin(), $4->end());
													$$ = new MethodDeclarationAST(ValueType::VOID, $2, *$4, $6);
													$$->set_location(@2);
													delete $4;
											}
			;

param_list : param_list_non_empty { $$ = $1; }
		   | %empty { $$ = new std::vector<VariableDeclarationAST *>(); }
		   ;
param_list_non_empty : param {
								$$ = new std::vector<VariableDeclarationAST *>();
								$$->push_back($1);
							 }
		   			 | param ',' param_list_non_empty { 
		   			 									$$ = $3; 
		   			 									$$->push_back($1);
		   			 								}
		   			 ;
param : type ID { $$ = new VariableDeclarationAST(std::string($2), $1); $$->set_location(@$); }
	  ;

// Statement block
block : '{' var_decl_list statement_list '}' { 
												$$ = new StatementBlockAST(*$2, *$3); 
												$$->set_location(@$);
												delete $2;
												delete $3;
											 }
	  ;

var_decl_list : var_decl_list var_decl { 
											$$ = $1;
											$$->insert($$->end(), $2->rbegin(), $2->rend());
											delete $2;
										}
			  | %empty { $$ = new std::vector<VariableDeclarationAST *>(); }
			  ;
var_decl : type var_list ';' {
								$$ = $2;
								for (auto& elem: *$$) {
									elem->type = $1;
								}
							 }
		 ;
var_list : ID { 
				$$ = new std::vector<VariableDeclarationAST *>(); 
				$$->push_back(new VariableDeclarationAST($1, ValueType::NONE)); 
				$$->back()->set_location(@$);
			  }
		 | ID ',' var_list  {
								$$ = $3;
								$$->push_back(new VariableDeclarationAST($1, ValueType::NONE));
								$$->back()->set_location(@$);
							}
		 ;


statement_list : statement_list statement    {
												$$ = $1;
												$$->push_back($2);
											}
			   | %empty { $$ = new std::vector<BaseAST *>(); }
			   ;
statement : location assign_op expr ';' { $1->is_lvalue = true; $$ = new AssignStatementAST($2, $1, $3); $$->set_location(@$); }
		  | method_call ';' { $$ = $1; }
		  | IF '(' expr ')' block else_block { $$ = new IfStatementAST($3, $5, $6); $$->set_location(@$); }
		  | FOR ID ASSIGN expr ',' expr block { $$ = new ForStatementAST($2, $4, $6, $7); $$->set_location(@$); }
		  | BREAK ';' { $$ = new BreakStatementAST(); $$->set_location(@$); }
		  | CONTINUE ';' { $$ = new ContinueStatementAST(); $$->set_location(@$); }
		  | RETURN expr ';' { $$ = new ReturnStatementAST($2); $$->set_location(@$); }
		  | RETURN ';' { $$ = new ReturnStatementAST(); $$->set_location(@$); }
		  | block { $$ = $1; }
		  ;

else_block : ELSE block { $$ = $2; }
		   | %empty { $$ = NULL; }
		   ;

assign_op : ASSIGN     { $$ = OperatorType::ASSIGN; }
		  | ASSIGN_ADD { $$ = OperatorType::ASSIGN_ADD; }
		  | ASSIGN_SUB { $$ = OperatorType::ASSIGN_SUB; }
		  ;

/* Expressions */
expr : location { $$ = $1; }
	 | method_call { $$ = $1; }
	 | literal { $$ = $1; }

	 | '(' expr ')' { $$ = $2; }
	 
	 | SUB expr %prec UMINUS { $$ = new UnaryMinusAST($2); $$->set_location(@$); }
	 | NOT expr { $$ = new UnaryNotAST($2); $$->set_location(@$); }
	 
	 | expr ADD expr { $$ = new ArithBinOperatorAST(OperatorType::ADD, $1, $3); $$->set_location(@$); } 
	 | expr SUB expr { $$ = new ArithBinOperatorAST(OperatorType::SUB, $1, $3); $$->set_location(@$); }
	 | expr MUL expr { $$ = new ArithBinOperatorAST(OperatorType::MUL, $1, $3); $$->set_location(@$); }
	 | expr DIV expr { $$ = new ArithBinOperatorAST(OperatorType::DIV, $1, $3); $$->set_location(@$); }
	 | expr MOD expr { $$ = new ArithBinOperatorAST(OperatorType::MOD, $1, $3); $$->set_location(@$); }
	 
	 | expr AND expr { $$ = new CondBinOperatorAST(OperatorType::AND, $1, $3); $$->set_location(@$); }
	 | expr OR expr  { $$ = new CondBinOperatorAST(OperatorType::OR, $1, $3); $$->set_location(@$); }

	 | expr LE expr  { $$ = new RelBinOperatorAST(OperatorType::LE, $1, $3); $$->set_location(@$); } 
	 | expr LT expr  { $$ = new RelBinOperatorAST(OperatorType::LT, $1, $3); $$->set_location(@$); } 
	 | expr GE expr  { $$ = new RelBinOperatorAST(OperatorType::GE, $1, $3); $$->set_location(@$); }
	 | expr GT expr  { $$ = new RelBinOperatorAST(OperatorType::GT, $1, $3); $$->set_location(@$); }
	 | expr EQ expr  { $$ = new EqBinOperatorAST(OperatorType::EQ, $1, $3); $$->set_location(@$); }
	 | expr NE expr  { $$ = new EqBinOperatorAST(OperatorType::NE, $1, $3); $$->set_location(@$); }
	 ;

location : ID { std::string id($1); $$ = new VariableLocationAST(id); $$->set_location(@$); }
		 | ID '[' expr ']' { std::string id($1); $$ = new ArrayLocationAST(id, $3); $$->set_location(@$); }
		 ;
		 
/* method calls */
method_call : ID '(' args ')' { 
								std::reverse($3->begin(), $3->end());
								$$ = new MethodCallAST(std::string($1), *$3);
								$$->set_location(@$); 
								delete $3; 
							}
			| CALLOUT '(' STRING_LIT callout_arg_list ')' { 
															std::reverse($4->begin(), $4->end());
															$$ = new CalloutCallAST(std::string($3), *$4); 
															$$->set_location(@$); 
															delete $4;
														}
			;

callout_arg_list : ',' callout_arg callout_arg_list { ($$ = $3)->push_back($2); }
				 | %empty { $$ = new std::vector<BaseAST *>(); }
				 ;
callout_arg : arg { $$ = $1; }
			| STRING_LIT { 
							std::string lit($1);
							$$ = new StringLiteralAST(lit);
							$$->set_location(@$); 
						} 
			; 

args : arg_list { $$ = $1; }
	 | %empty { $$ = new std::vector<BaseAST *>(); } 
	 ;
arg_list : arg 	{
					$$ = new std::vector<BaseAST *>();
					$$->push_back($1);
				}
		 | arg ',' arg_list { ($$ = $3)->push_back($1); }
		 ;
arg: expr { $$ = $1; }
   ;

/* literals */
literal : INT_LIT { $$ = new IntegerLiteralAST($1); $$->set_location(@$); }
		| BOOL_LIT { $$ = new BooleanLiteralAST($1); $$->set_location(@$); }
		| CHAR_LIT { $$ = new IntegerLiteralAST($1); $$->set_location(@$); }

%%

void show_help(bool quit = true) {
	std::cerr << "Usage: decaf <file>.dcf [--output=<output-file>]\n";
	if (quit) exit(1);
}

int main(int argc, char **argv) {
	if (argc < 2) show_help();

	// open input file as stream
	std::string filename(argv[1]);
	if (filename.size() < 4 
		|| filename.substr(filename.size() - 4, 4) != ".dcf")
		show_help();

	std::string out_filename = "";
	if (argc >= 3) {
		out_filename = std::string(argv[2]);
		if (out_filename.size() < 9 || out_filename.substr(0, 9) != "--output=") {
			show_help();
		}
		out_filename = out_filename.substr(9, out_filename.size() - 9);
	}

	std::ifstream fin(filename);
	if (!fin.good()) {
		std::cerr << "Error: unable to read file " << filename << "\n";
		return 1;
	}

	// Make a driver
	Decaf::Driver driver;
	driver.scanner = new Decaf::Scanner(&fin);
	driver.parser = new Decaf::Parser(driver);
	// driver.parser->set_debug_level(1);

	// parse the code, stop on syntax error
	if (driver.parser->parse() != 0) {
		return 1;
	}

#ifdef DEBUG_ENABLED
	// Graph Generation (debugging/mermaidjs)
	TreeGenerator *tree = new TreeGenerator();
	std::ofstream tree_out("var/graph.mer");
	tree->generate(*(driver.root), tree_out);
	delete tree;
#endif

	// Semantic analysis
	SemanticAnalyzer *analyzer = new SemanticAnalyzer();
	
	if (!analyzer->check(*(driver.root))) {
#ifdef DEBUG_ENABLED
		analyzer->display(std::cerr, true);
#else
		analyzer->display(std::cerr);
#endif
		return 1;
	}

	delete analyzer;

	// code generation (LLVM IR)
	CodeGenerator *IR_gen = new CodeGenerator(filename);
	IR_gen->generate(*(driver.root));	
	IR_gen->print(out_filename);

	// cleanup
	delete driver.root;
	delete driver.parser;
	delete driver.scanner;

	return 0;
}

void Decaf::Parser::error(const location_type& loc, const std::string& err) {
	std::cerr << "[" << loc << "] " 
				  << "error: " << err << std::endl;
}