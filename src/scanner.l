%{
#include "parser.tab.h"
int lineno = 0; 
%}


%%
"int" 					{printf("int"); return INT;} 
"boolean"				{return BOOL;}
[0-9]*					{return INT_LIT;}
0x[0-9a-fA-F]*			{return INT_LIT;}
\'.\'					{return CHAR_LIT;}
\".*\"					{return STRING_LIT;}
(true|false) 			{return BOOL_LIT;}

[a-zA-Z_][a-zA-Z0-9_]*	{return ID;}

 /**** Operators ****/
	/* Arithmetic */
"+"						{return ADD;}
"-"						{return SUB;}
"*"						{return MUL;}
"/"						{return DIV;}

	/* Logical */
"&&"					{return AND;}
"||"					{return OR;}
"!"						{return NOT;}

	/* Relational */
">"						{return GT;}
"<"						{return LT;}
">="					{return GE;}
"<="					{return LE;}
"=="					{return EQ;}
"!="					{return NE;}

	/* Assignment */
"="						{return ASSIGN;}
"+="					{return ASSIGN_ADD;}
"-="					{return ASSIGN_SUB;}

 /*** Brackets ***/
"("						{return PAR_OPEN;}
")"						{return PAR_CLOSE;}
"["						{return SQR_OPEN;}
"]"						{return SQR_CLOSE;}
"{"						{return BRACE_OPEN;}
"}"						{return BRACE_CLOSE;}

 /*** Separators ***/
";"						{return SEMICOLON;}
","						{return COMMA;}

 /*** Whitespaces ***/
[ \t]					{ }
\n						{++lineno;}
.						{ printf("Line No %d: Unrecognized Character\n", lineno); }

%%

