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

	class ASTnode;
	class Location;
	class VariableDeclaration;
	class StatementBlock;
	class MethodDeclaration;
}

%parse-param {Driver& driver}

%code {
#include <iostream>	
#include <fstream>
#include <string>
#include <cassert>

#include "scanner.hh"
#include "driver.hh"

// AST node classes
#include "astnodes/ast.hh"
#include "astnodes/literals.hh"
#include "astnodes/operators.hh"
#include "astnodes/variables.hh"
#include "astnodes/statements.hh"
#include "astnodes/blocks.hh"
#include "astnodes/methods.hh"
#include "astnodes/program.hh"

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
	VariableDeclaration *decl;

	// AST types
	ASTnode *ast;
	Location *loc;
	StatementBlock *block;
	MethodDeclaration *method;

	// vectors
	std::vector<VariableDeclaration> *var_decl;
	std::vector<ASTnode *> *nodes;
	std::vector<MethodDeclaration *> *methods;
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

%type <ast> literal expr statement
%type <block> block else_block
%type <loc> location
%type <method> method_decl

%type <var_decl> var_decl_list var_decl var_list 
%type <var_decl> param_list param_list_non_empty
%type <var_decl> glob_var_decl_list field_decl field_decl_list
%type <nodes> statement_list
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
																	std::cerr << "! error: Class name has to be `Program`\n";
																	driver.root = NULL;
																	return 1;
																}

																driver.root = new Program(*$4, *$5);
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
												$$->insert($$->end(), $2->begin(), $2->end());
												delete $2;
											}
		  		| %empty {
		  					$$ = new std::vector<VariableDeclaration>();
		  				}
		  		;
field_decl : type glob_var_decl_list ';' {
											$$ = $2;
											for (auto& elem: *$$) {
												elem.type = $1;
											}
										}
		   ;

glob_var_decl_list : glob_var_decl {
										$$ = new std::vector<VariableDeclaration>();
										$$->push_back(*$1); 
										delete $1;
								}
				   | glob_var_decl ',' glob_var_decl_list {
				   											  $$ = $3;
				   											  $$->push_back(*$1);
				   											  delete $1;
														}
				   ;
glob_var_decl : ID { $$ = new VariableDeclaration(std::string($1)); }
			  | ID '[' INT_LIT ']' { $$ = new ArrayDeclaration(std::string(std::string($1)), $3); }
			  ;

/* Function(method) declarations */
method_decl_list : method_decl_list method_decl {
													$$ = $1;
													$$->push_back($2);
												}
				 | method_decl 	{
				 					$$ = new std::vector<MethodDeclaration *>();
				 					$$->push_back($1);
				 				}
				 ;
method_decl : type ID '(' param_list ')' block {
													$$ = new MethodDeclaration($1, $2, *$4, $6);
													delete $4;
											}
			| VOID ID '(' param_list ')' block {
													$$ = new MethodDeclaration(ValueType::VOID, $2, *$4, $6);
													delete $4;
											}
			;

param_list : param_list_non_empty { $$ = $1; }
		   | %empty { $$ = new std::vector<VariableDeclaration>(); }
		   ;
param_list_non_empty : param {
								$$ = new std::vector<VariableDeclaration>();
								$$->push_back(*$1);
								delete $1;
							 }
		   			 | param ',' param_list_non_empty { 
		   			 									$$ = $3; 
		   			 									$$->push_back(*$1);
		   			 									delete $1;
		   			 								}
		   			 ;
param : type ID { $$ = new VariableDeclaration(std::string($2), $1); }
	  ;

// Statement block
block : '{' var_decl_list statement_list '}' { 
												std::cerr << ">> Block\n";
												$$ = new StatementBlock(*$2, *$3); 
												delete $2;
												delete $3;
												std::cerr << "<< Block\n";
											 }
	  ;

var_decl_list : var_decl var_decl_list  { 
											std::cerr << ">> Merging variable list\n";
											$$ = $2;
											$$->insert($$->end(), $1->begin(), $1->end());
											delete $1;
											std::cerr << "<< Merging variable list\n";
										}
			  | %empty { $$ = new std::vector<VariableDeclaration>(); std::cerr << ">> Init. var list\n"; }
			  ;
var_decl : type var_list ';' {
								$$ = $2;
								for (auto& elem: *$$) {
									elem.type = $1;
								}
							 }
		 ;
var_list : ID { 
				$$ = new std::vector<VariableDeclaration>(); 
				$$->emplace_back($1, ValueType::NONE); 
			  }
		 | ID ',' var_list  {
								$$ = $3;
								$$->emplace_back($1, ValueType::NONE);
							}
		 ;


statement_list : statement statement_list   {
												$$ = $2;
												$$->push_back($1);
											}
			   | %empty { $$ = new std::vector<ASTnode *>(); }
			   ;
statement : location assign_op expr ';' { $$ = new AssignOperator($2, $1, $3); }
		  | method_call ';' {}
		  | IF '(' expr ')' block else_block {} 
		  | FOR ID ASSIGN expr ',' expr block {}
		  | BREAK ';' { $$ = new BreakStatement(); }
		  | CONTINUE ';' { $$ = new ContinueStatement(); }
		  | RETURN expr ';' { $$ = new ReturnStatement($2); }
		  | RETURN ';' { $$ = new ReturnStatement(); }
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
	 | method_call { $$ = NULL; }
	 | literal { $$ = $1; }

	 | '(' expr ')' { $$ = $2; }
	 
	 | SUB expr %prec UMINUS { $$ = new UnaryMinus($2); }
	 | NOT expr { $$ = new UnaryNot($2); }
	 
	 | expr ADD expr { $$ = new ArithBinOperator(OperatorType::ADD, $1, $3); } 
	 | expr SUB expr { $$ = new ArithBinOperator(OperatorType::SUB, $1, $3); }
	 | expr MUL expr { $$ = new ArithBinOperator(OperatorType::MUL, $1, $3); }
	 | expr DIV expr { $$ = new ArithBinOperator(OperatorType::DIV, $1, $3); }
	 | expr MOD expr { $$ = new ArithBinOperator(OperatorType::MOD, $1, $3); }
	 
	 | expr AND expr { $$ = new CondBinOperator(OperatorType::AND, $1, $3); }
	 | expr OR expr  { $$ = new CondBinOperator(OperatorType::OR, $1, $3); }

	 | expr LE expr  { $$ = new RelBinOperator(OperatorType::LE, $1, $3); } 
	 | expr LT expr  { $$ = new RelBinOperator(OperatorType::LT, $1, $3); } 
	 | expr GE expr  { $$ = new RelBinOperator(OperatorType::GE, $1, $3); }
	 | expr GT expr  { $$ = new RelBinOperator(OperatorType::GT, $1, $3); }
	 | expr EQ expr  { $$ = new EqBinOperator(OperatorType::EQ, $1, $3); }
	 | expr NE expr  { $$ = new EqBinOperator(OperatorType::NE, $1, $3); }
	 ;

location : ID { std::string id($1); $$ = new VariableLocation(id); }
		 | ID '[' expr ']' { std::string id($1); $$ = new ArrayLocation(id, $3); }
		 ;
		 
/* method calls */
method_call : method_name '(' args ')' {}
			| CALLOUT '(' STRING_LIT callout_arg_list ')' {}
			;

callout_arg_list : ',' callout_arg callout_arg_list {}
				 | %empty {}
				 ;
callout_arg : arg {}
			| STRING_LIT {} 
			; 

method_name : ID {}
			;

args : arg_list {}
	 | %empty {} 
	 ;
arg_list : arg {}
		 | arg ',' arg_list {}
		 ;
arg: expr {}
   ;

/* literals */
literal : INT_LIT { $$ = new IntegerLiteral($1); }
		| BOOL_LIT { $$ = new BooleanLiteral($1); }
		| CHAR_LIT { $$ = new IntegerLiteral($1); }

%%

int main(int argc, char **argv) {
	if (argc < 2) {
		std::cerr << "Usage: decaf <file>.dcf\n";
		return 1;
	}

	std::string filename(argv[1]);

	std::ifstream fin(filename);
	if (!fin.good()) {
		std::cerr << "Error: unable to read file " << filename << "\n";
		return 1;
	}

	Decaf::Driver driver;
	
	driver.scanner = new Decaf::Scanner(&fin);
	driver.parser = new Decaf::Parser(driver);

	if (driver.parser->parse() != 0) {
		std::cerr << "Parsing failed!\n";
		return 1;
	}

	// program at driver.root

	return 0;
}

void Decaf::Parser::error(const location_type& loc, const std::string& err) {
	std::cerr << "[" << loc << "] " 
				  << "error: " << err << std::endl;
}