%{
#include <cstdio>
#include <iostream>

#include "scanner.hh"

using token = Decaf::Parser::token;

#define yyterminate() return token::END;

#define YY_USER_ACTION yylloc->columns(yyleng);

%}

%option c++
%option yyclass="Decaf::Scanner"
%option noyywrap
%option nodefault

%%
%{
	yylloc->step();	
%}

[0-9]*					{printf("<dec>"); return token::INT_LIT;}
0x[0-9a-fA-F]*			{printf("<hex>"); return token::INT_LIT;}
\'.\'					{printf("<char>"); return token::CHAR_LIT;}
\".*\"					{printf("<string[%lu]: %s>", strlen(yytext), yytext); return token::STRING_LIT;}
(true|false) 			{printf("<bool>"); return token::BOOL_LIT;}

 /*** keywords ***/
"int" 					{printf("<keyword, %s>", yytext); return token::INT;} 
"boolean"				{printf("<keyword, %s>", yytext); return token::BOOL;}
"void"					{printf("<keyword, %s>", yytext); return token::VOID;}
"class"					{printf("<%s>", yytext); return token::CLASS;}
"if"					{printf("<keyword, %s>", yytext); return token::IF;}
"else"					{printf("<keyword, %s>", yytext); return token::ELSE;}
"for"					{printf("<keyword, %s>", yytext); return token::FOR;}
"break"					{printf("<keyword, %s>", yytext); return token::BREAK;}
"continue"				{printf("<keyword, %s>", yytext); return token::CONTINUE;}
"return"				{printf("<keyword, %s>", yytext); return token::RETURN;}
"callout"				{printf("<keyword, %s>", yytext); return token::CALLOUT;}

[a-zA-Z_][a-zA-Z0-9_]*	{printf("<ID>"); return token::ID;}

 /**** Operators ****/
	/* Arithmetic */
"+"						{printf("<op, %s>", yytext); return token::ADD;}
"-"						{printf("<op, %s>", yytext); return token::SUB;}
"*"						{printf("<op, %s>", yytext); return token::MUL;}
"/"						{printf("<op, %s>", yytext); return token::DIV;}
"%"						{printf("<op, %s>", yytext); return token::MOD;}

	/* Logical */
"&&"					{printf("<op, %s>", yytext); return token::AND;}
"||"					{printf("<op, %s>", yytext); return token::OR;}
"!"						{printf("<op, %s>", yytext); return token::NOT;}

	/* Relational */
">"						{printf("<op, %s>", yytext); return token::GT;}
"<"						{printf("<op, %s>", yytext); return token::LT;}
">="					{printf("<op, %s>", yytext); return token::GE;}
"<="					{printf("<op, %s>", yytext); return token::LE;}

"=="					{printf("<op, %s>", yytext); return token::EQ;}
"!="					{printf("<op, %s>", yytext); return token::NE;}

	/* Assignment */
"="						{printf("<assign, %s>", yytext); return token::ASSIGN;}
"+="					{printf("<assign, %s>", yytext); return token::ASSIGN_ADD;}
"-="					{printf("<assign, %s>", yytext); return token::ASSIGN_SUB;}

 /*** Brackets ***/
"("						{printf("%s", yytext); return token::PAR_OPEN;}
")"						{printf("%s", yytext); return token::PAR_CLOSE;}
"["						{printf("%s", yytext); return token::SQR_OPEN;}
"]"						{printf("%s", yytext); return token::SQR_CLOSE;}
"{"						{printf("%s", yytext); return token::BRACE_OPEN;}
"}"						{printf("%s", yytext); return token::BRACE_CLOSE;}

 /*** Separators ***/
";"						{printf("%s", yytext); return token::SEMICOLON;}
","						{printf("%s", yytext); return token::COMMA;}

 /*** Whitespaces/comments ***/
[ \t]					{yylloc->step(); printf("%s", yytext);}
"//".*					{}
\n 						{yylloc->lines(yyleng); yylloc->step(); printf("\n");}
.						{ printf("Line No %d: Unrecognized Character `%s`\n", lineno(), yytext); }

%%
