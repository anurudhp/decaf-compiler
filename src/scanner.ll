%{
#include <cstdio>
#include <iostream>

#include "scanner.hh"

using token = Decaf::Parser::token;
using token_type = Decaf::Parser::token_type;

#define yyterminate() return token::END;

#define YY_USER_ACTION yylloc->columns(yyleng);

%}

%option c++
%option yyclass="Decaf::Scanner"
%option noyywrap
%option nodefault
%option debug

%%
%{
	yylloc->step();	
%}

 /*** literals ***/
[0-9]*  { 
	yylval->ival = atoi(yytext);
	return token::INT_LIT;
}
0x[0-9a-fA-F]*	{
	yylval->ival = strtol(yytext, NULL, 16);
	return token::INT_LIT;
}
\'.\' {
	yylval->ival = *yytext;
	return token::CHAR_LIT;
}
\"(\\n|\\t|\\\'|\\\"|\\\\|[^\\\'\"])*\"	{
	yylval->sval = strdup(yytext);
	return token::STRING_LIT;
}
(true|false)  {
	yylval->bval = strcmp(yytext, "true") == 0;
	return token::BOOL_LIT;
}

 /*** keywords ***/
"int" 					{return token::INT;} 
"boolean"				{return token::BOOL;}
"void"					{return token::VOID;}
"class"					{return token::CLASS;}
"if"					{return token::IF;}
"else"					{return token::ELSE;}
"for"					{return token::FOR;}
"break"					{return token::BREAK;}
"continue"				{return token::CONTINUE;}
"return"				{return token::RETURN;}
"callout"				{return token::CALLOUT;}

[a-zA-Z_][a-zA-Z0-9_]*	{
	yylval->sval = strdup(yytext);
	return token::ID;
}

 /**** Operators ****/
	/* Arithmetic */
"+"						{return token::ADD;}
"-"						{return token::SUB;}
"*"						{return token::MUL;}
"/"						{return token::DIV;}
"%"						{return token::MOD;}

	/* Logical */
"&&"					{return token::AND;}
"||"					{return token::OR;}
"!"						{return token::NOT;}

	/* Relational */
">"						{return token::GT;}
"<"						{return token::LT;}
">="					{return token::GE;}
"<="					{return token::LE;}

"=="					{return token::EQ;}
"!="					{return token::NE;}

	/* Assignment */
"="						{return token::ASSIGN;}
"+="					{return token::ASSIGN_ADD;}
"-="					{return token::ASSIGN_SUB;}

 /*** Brackets and Separators ***/
\(|\)|\[|\]|\{|\}|;|,	{return static_cast<token_type>(*yytext);}

 /*** Whitespaces/comments ***/
[ \t]					{yylloc->step();}
"//".*					{}
\n 						{yylloc->lines(yyleng); yylloc->step();}
.	{ 
	std::cerr << "Line No " << lineno() 
			  << ": Unrecognized Character "
			  << yytext << std::endl; 
}

%%
