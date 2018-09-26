%{
#include <stdio.h>	
%}

%token INT BOOL 
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


%nonassoc NOT
%left EQ NE LE LT GE GT
%left AND OR
%left ADD SUB
%left MUL DIV
%left MOD
%left BINOP

%%

expr : location
	 | method_call
	 | literal
	 | expr bin_op expr %prec BINOP
	 | SUB expr
	 | NOT expr
	 | PAR_OPEN expr PAR_CLOSE
	 ;

location : ID
		 | ID SQR_OPEN expr SQR_CLOSE ;
		 
/* method calls */
method_call : method_name PAR_OPEN params PAR_CLOSE;

method_name : ID;

params : param_list | ;
param_list : param | param COMMA param_list ;
param: expr;

/* types/literals */
//  type : INT | BOOL

literal : INT_LIT 
		| BOOL_LIT 
		| CHAR_LIT ;

/* operators */
bin_op  : arith_op 
		| rel_op 
		| cond_op 
		| comp_op
		;
arith_op : ADD 
		 | SUB 
		 | MUL 
		 | DIV 
		 | MOD
		 ;
rel_op  : LE 
		| LT 
		| GE 
		| GT 
		;
comp_op : EQ 
		| NE
		;
cond_op : AND 
		| OR 
		;
%%

int main(int argc, char **argv)
{
	yyparse();
}

int yyerror(char *s)
{
	(void) fprintf(stderr, "error: %s\n", s);
}