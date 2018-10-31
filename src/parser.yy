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
	class ASTnode;
	enum class OperatorType;
	enum class ValueType;
}

%parse-param {Driver& driver}

%code {
#include <iostream>	
#include <fstream>
#include <string>

#include "scanner.hh"
#include "driver.hh"

// AST node classes
#include "astnodes/ast.hh"
#include "astnodes/literals.hh"
#include "astnodes/operators.hh"
#include "astnodes/variables.hh"
#include "astnodes/statements.hh"

#undef yylex
#define yylex driver.scanner->yylex

}

%union {
	// primitive types
	int ival;
	bool bval;
	char *sval;

	// complex types
	ASTnode *ast;
	OperatorType op;
	ValueType vt;

	// vectors

}

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
%token PAR_OPEN PAR_CLOSE
%token SQR_OPEN SQR_CLOSE
%token BRACE_OPEN BRACE_CLOSE
%token SEMICOLON COMMA

 /* non-terminals */
%type <ast> literal location
%type <ast> expr statement  
%type <op> assign_op
%type <vt> type

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
program : CLASS ID BRACE_OPEN field_decl_list method_decl_list BRACE_CLOSE {}
		;

/* Field(data) declarations */

type : INT { $$ = ValueType::INT; }
	 | BOOL { $$ = ValueType::BOOL; }
	 ;

field_decl_list : field_decl_list field_decl {}
		  		| %empty {}
		  		;
field_decl : type glob_var_decl_list SEMICOLON {}
		   ;

glob_var_decl_list : glob_var_decl {}
				   | glob_var_decl COMMA glob_var_decl_list {}
				   ;
glob_var_decl : ID {}
			  | ID SQR_OPEN INT_LIT SQR_CLOSE {}
			  ;

/* Function(method) declarations */
method_decl_list : method_decl_list method_decl {}
				 | method_decl {}
				 ;
method_decl : type ID PAR_OPEN param_list PAR_CLOSE block {}
			| VOID ID PAR_OPEN param_list PAR_CLOSE block {}
			;

param_list : param_list_non_empty {}
		   | %empty {}
		   ;
param_list_non_empty : param {}
		   			 | param COMMA param_list_non_empty {}
		   			 ;
param : type ID {}
	  ;

// Statement block
block : BRACE_OPEN var_decl_list statement_list BRACE_CLOSE {}
	  ;

var_decl_list : var_decl var_decl_list {}
			  | %empty {}
			  ;
var_decl : type var_list SEMICOLON {}
		 ;
var_list : ID {}
		 | ID COMMA var_list {}
		 ;


statement_list : statement statement_list {}
			   | %empty {}
			   ;
statement : location assign_op expr SEMICOLON {}
		  | method_call SEMICOLON {}
		  | IF PAR_OPEN expr PAR_CLOSE block else_block {} 
		  | FOR ID ASSIGN expr COMMA expr block {}
		  | BREAK SEMICOLON { $$ = new BreakStatement(); }
		  | CONTINUE SEMICOLON { $$ = new ContinueStatement(); }
		  | RETURN expr SEMICOLON { $$ = new ReturnStatement($2); }
		  | RETURN SEMICOLON { $$ = new ReturnStatement(); }
		  | block {}
		  ;

else_block : ELSE block {}
		   | %empty {}
		   ;

assign_op : ASSIGN     { $$ = OperatorType::ASSIGN; }
		  | ASSIGN_ADD { $$ = OperatorType::ASSIGN_ADD; }
		  | ASSIGN_SUB { $$ = OperatorType::ASSIGN_SUB; }
		  ;

/* Expressions */
expr : location { $$ = $1; }
	 | method_call { $$ = NULL; }
	 | literal { $$ = $1; }

	 | PAR_OPEN expr PAR_CLOSE { $$ = $2; }
	 
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
		 | ID SQR_OPEN expr SQR_CLOSE { std::string id($1); $$ = new ArrayLocation(id, $3); }
		 ;
		 
/* method calls */
method_call : method_name PAR_OPEN args PAR_CLOSE {}
			| CALLOUT PAR_OPEN STRING_LIT callout_arg_list PAR_CLOSE {}
			;

callout_arg_list : COMMA callout_arg callout_arg_list {}
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
		 | arg COMMA arg_list {}
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

	return 0;
}

void Decaf::Parser::error(const location_type& loc, const std::string& err) {
	std::cerr << "[" << loc << "] " 
				  << "error: " << err << std::endl;
}