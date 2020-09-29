#pragma once

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL                                                                \
  Decaf::Parser::token_type Decaf::Scanner::yylex(                             \
      Decaf::Parser::semantic_type *yylval,                                    \
      Decaf::Parser::location_type *yylloc)

#include "location.hh"
#include "parser.tab.hh"

namespace Decaf {

class Scanner : public yyFlexLexer {
public:
  Scanner(std::istream *arg_yyin = 0) : yyFlexLexer(arg_yyin) {}

  virtual ~Scanner() {}

  using FlexLexer::yylex;

  virtual Parser::token_type yylex(Parser::semantic_type *yylval,
                                   Parser::location_type *yylloc);

  Decaf::Parser::semantic_type *yylval = nullptr;
};

} // namespace Decaf
