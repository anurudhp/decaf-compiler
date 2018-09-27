%{
#include "parser.tab.h"
int lineno = 1; 
%}


%%
[0-9]*					{printf("dec\n"); return INT_LIT;}
0x[0-9a-fA-F]*			{printf("hex\n"); return INT_LIT;}
\'.\'					{printf("char\n"); return CHAR_LIT;}
\".*\"					{printf("string[%lu]: %s\n", strlen(yytext), yytext); return STRING_LIT;}
(true|false) 			{printf("bool\n"); return BOOL_LIT;}

 /*** keywords ***/
"int" 					{printf("%s\n", yytext); return INT;} 
"boolean"				{printf("%s\n", yytext); return BOOL;}
"void"					{printf("%s\n", yytext); return VOID;}
"class"					{printf("%s\n", yytext); return CLASS;}
"if"					{printf("%s\n", yytext); return IF;}
"else"					{printf("%s\n", yytext); return ELSE;}
"for"					{printf("%s\n", yytext); return FOR;}
"break"					{printf("%s\n", yytext); return BREAK;}
"continue"				{printf("%s\n", yytext); return CONTINUE;}
"return"				{printf("%s\n", yytext); return RETURN;}
"callout"				{printf("%s\n", yytext); return CALLOUT;}

[a-zA-Z_][a-zA-Z0-9_]*	{printf("ID\n"); return ID;}

 /**** Operators ****/
	/* Arithmetic */
"+"						{printf("%s\n", yytext); return ADD;}
"-"						{printf("%s\n", yytext); return SUB;}
"*"						{printf("%s\n", yytext); return MUL;}
"/"						{printf("%s\n", yytext); return DIV;}
"%"						{printf("%s\n", yytext); return MOD;}

	/* Logical */
"&&"					{printf("%s\n", yytext); return AND;}
"||"					{printf("%s\n", yytext); return OR;}
"!"						{printf("%s\n", yytext); return NOT;}

	/* Relational */
">"						{printf("%s\n", yytext); return GT;}
"<"						{printf("%s\n", yytext); return LT;}
">="					{printf("%s\n", yytext); return GE;}
"<="					{printf("%s\n", yytext); return LE;}

"=="					{printf("%s\n", yytext); return EQ;}
"!="					{printf("%s\n", yytext); return NE;}

	/* Assignment */
"="						{printf("%s\n", yytext); return ASSIGN;}
"+="					{printf("%s\n", yytext); return ASSIGN_ADD;}
"-="					{printf("%s\n", yytext); return ASSIGN_SUB;}

 /*** Brackets ***/
"("						{printf("%s\n", yytext); return PAR_OPEN;}
")"						{printf("%s\n", yytext); return PAR_CLOSE;}
"["						{printf("%s\n", yytext); return SQR_OPEN;}
"]"						{printf("%s\n", yytext); return SQR_CLOSE;}
"{"						{printf("%s\n", yytext); return BRACE_OPEN;}
"}"						{printf("%s\n", yytext); return BRACE_CLOSE;}

 /*** Separators ***/
";"						{printf("%s\n", yytext); return SEMICOLON;}
","						{printf("%s\n", yytext); return COMMA;}

 /*** Whitespaces/comments ***/
[ \t]					{ }
\/\/.*\n				{printf("comment?: %s\n", yytext); ++lineno;}
\n 						{++lineno;}
.						{ printf("Line No %d: Unrecognized Character `%s`\n", lineno, yytext); }

%%

