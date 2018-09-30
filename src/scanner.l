%{
#include "parser.tab.h"
int lineno = 1; 
%}


%%
[0-9]*					{printf("<dec>"); return INT_LIT;}
0x[0-9a-fA-F]*			{printf("<hex>"); return INT_LIT;}
\'.\'					{printf("<char>"); return CHAR_LIT;}
\".*\"					{printf("<string[%lu]: %s>", strlen(yytext), yytext); return STRING_LIT;}
(true|false) 			{printf("<bool>"); return BOOL_LIT;}

 /*** keywords ***/
"int" 					{printf("<keyword, %s>", yytext); return INT;} 
"boolean"				{printf("<keyword, %s>", yytext); return BOOL;}
"void"					{printf("<keyword, %s>", yytext); return VOID;}
"class"					{printf("<%s>", yytext); return CLASS;}
"if"					{printf("<keyword, %s>", yytext); return IF;}
"else"					{printf("<keyword, %s>", yytext); return ELSE;}
"for"					{printf("<keyword, %s>", yytext); return FOR;}
"break"					{printf("<keyword, %s>", yytext); return BREAK;}
"continue"				{printf("<keyword, %s>", yytext); return CONTINUE;}
"return"				{printf("<keyword, %s>", yytext); return RETURN;}
"callout"				{printf("<keyword, %s>", yytext); return CALLOUT;}

[a-zA-Z_][a-zA-Z0-9_]*	{printf("<ID>"); return ID;}

 /**** Operators ****/
	/* Arithmetic */
"+"						{printf("<op, %s>", yytext); return ADD;}
"-"						{printf("<op, %s>", yytext); return SUB;}
"*"						{printf("<op, %s>", yytext); return MUL;}
"/"						{printf("<op, %s>", yytext); return DIV;}
"%"						{printf("<op, %s>", yytext); return MOD;}

	/* Logical */
"&&"					{printf("<op, %s>", yytext); return AND;}
"||"					{printf("<op, %s>", yytext); return OR;}
"!"						{printf("<op, %s>", yytext); return NOT;}

	/* Relational */
">"						{printf("<op, %s>", yytext); return GT;}
"<"						{printf("<op, %s>", yytext); return LT;}
">="					{printf("<op, %s>", yytext); return GE;}
"<="					{printf("<op, %s>", yytext); return LE;}

"=="					{printf("<op, %s>", yytext); return EQ;}
"!="					{printf("<op, %s>", yytext); return NE;}

	/* Assignment */
"="						{printf("<assign, %s>", yytext); return ASSIGN;}
"+="					{printf("<assign, %s>", yytext); return ASSIGN_ADD;}
"-="					{printf("<assign, %s>", yytext); return ASSIGN_SUB;}

 /*** Brackets ***/
"("						{printf("%s", yytext); return PAR_OPEN;}
")"						{printf("%s", yytext); return PAR_CLOSE;}
"["						{printf("%s", yytext); return SQR_OPEN;}
"]"						{printf("%s", yytext); return SQR_CLOSE;}
"{"						{printf("%s", yytext); return BRACE_OPEN;}
"}"						{printf("%s", yytext); return BRACE_CLOSE;}

 /*** Separators ***/
";"						{printf("%s", yytext); return SEMICOLON;}
","						{printf("%s", yytext); return COMMA;}

 /*** Whitespaces/comments ***/
[ \t]					{printf("%s", yytext);}
"//".*					{}
\n 						{printf("\n"); ++lineno;}
.						{ printf("Line No %d: Unrecognized Character `%s`\n", lineno, yytext); }

%%

