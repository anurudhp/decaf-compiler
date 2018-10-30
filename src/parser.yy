%skeleton "lalr1.cc"
%defines
%define api.namespace {Decaf}
%define parser_class_name {Parser}

%locations 

%code requires {
	namespace Decaf {
		class Scanner;
	}
}

%parse-param {Scanner& scanner}

%code {
#include <iostream>	
#include <fstream>
#include <string>

#include "scanner.hh"

#undef yylex
#define yylex scanner.yylex
}

%token END 0


%token CLASS IF ELSE FOR BREAK CONTINUE RETURN CALLOUT
%token INT BOOL VOID
%token INT_LIT BOOL_LIT STRING_LIT CHAR_LIT
%token ID
%token ADD SUB MUL DIV MOD
%token AND OR NOT
%token GT GE LT LE EQ NE
%token ASSIGN ASSIGN_ADD ASSIGN_SUB
%token PAR_OPEN PAR_CLOSE
%token SQR_OPEN SQR_CLOSE
%token BRACE_OPEN BRACE_CLOSE
%token SEMICOLON COMMA

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
program : CLASS ID BRACE_OPEN field_decl_list method_decl_list BRACE_CLOSE
		;

/* Field(data) declarations */

type : INT 
	 | BOOL 
	 ;

field_decl_list : field_decl_list field_decl 
		  		| %empty
		  		;
field_decl : type glob_var_decl_list SEMICOLON 
		   ;

glob_var_decl_list : glob_var_decl 
				   | glob_var_decl COMMA glob_var_decl_list
				   ;
glob_var_decl : ID 
			  | ID SQR_OPEN INT_LIT SQR_CLOSE
			  ;

/* Function(method) declarations */
method_decl_list : method_decl_list method_decl 
				 | method_decl
				 ;
method_decl : type ID PAR_OPEN param_list PAR_CLOSE block
			| VOID ID PAR_OPEN param_list PAR_CLOSE block
			;

param_list : param_list_non_empty
		   | %empty
		   ;
param_list_non_empty : param 
		   			 | param COMMA param_list_non_empty
		   			 ;
param : type ID
	  ;

// Statement block
block : BRACE_OPEN var_decl_list statement_list BRACE_CLOSE 
	  ;

var_decl_list : var_decl var_decl_list
			  | %empty
			  ;
var_decl : type var_list SEMICOLON
		 ;
var_list : ID 
		 | ID COMMA var_list
		 ;


statement_list : statement statement_list
			   | %empty
			   ;
statement : location assign_op expr SEMICOLON
		  | method_call SEMICOLON
		  | IF PAR_OPEN expr PAR_CLOSE block else_block 
		  | FOR ID ASSIGN expr COMMA expr block
		  | BREAK SEMICOLON
		  | CONTINUE SEMICOLON
		  | RETURN expr SEMICOLON
		  | RETURN SEMICOLON
		  | block 
		  ;

else_block : ELSE block
		   | %empty
		   ;

assign_op : ASSIGN
		  | ASSIGN_ADD
		  | ASSIGN_SUB
		  ;

/* Expressions */
expr : location
	 | method_call
	 | literal

	 | PAR_OPEN expr PAR_CLOSE
	 
	 | SUB expr %prec UMINUS
	 | NOT expr
	 
	 | expr ADD expr 
	 | expr SUB expr 
	 | expr MUL expr 
	 | expr DIV expr 
	 | expr MOD expr 
	 
	 | expr AND expr 
	 | expr OR expr 

	 | expr LE expr 
	 | expr LT expr 
	 | expr GE expr
	 | expr GT expr
	 | expr EQ expr
	 | expr NE expr
	 ;

location : ID
		 | ID SQR_OPEN expr SQR_CLOSE ;
		 
/* method calls */
method_call : method_name PAR_OPEN args PAR_CLOSE
			| CALLOUT PAR_OPEN STRING_LIT callout_arg_list PAR_CLOSE
			;

callout_arg_list : COMMA callout_arg callout_arg_list 
				 | %empty
				 ;
callout_arg : arg 
			| STRING_LIT 
			; 

method_name : ID;

args : arg_list 
	 | %empty 
	 ;
arg_list : arg
		 | arg COMMA arg_list
		 ;
arg: expr;

/* literals */
literal : INT_LIT 
		| BOOL_LIT 
		| CHAR_LIT ;

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

	Decaf::Scanner *scanner = new Decaf::Scanner(&fin);
	Decaf::Parser *parser = new Decaf::Parser(*scanner);
	if (parser->parse() != 0) {
		std::cerr << "Parsing failed!\n";
		return 1;
	}


	return 0;
}

void Decaf::Parser::error(const location_type& loc, const std::string& err) {
	std::cerr << "[" << loc << "] " 
			  << "error: " << err << std::endl;
}