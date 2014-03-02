/*
reset; flex -osample.flex.cc sample.l; g++ -osample.flex sample.flex.cc
*/

%option noyywrap nodefault bison-bridge bison-locations yylineno outfile="sample.flex.cc"
%x COMMENT

%{
#include <cstdlib>
#include <cstring>
#include <tools/nycr/str.h>
#include "sample.bison.hh"
void yyerror(const YYLTYPE *, char const *);
void YYLLOC_DEFAULT(YYLTYPE &lhs, YYLTYPE *rhs, int n) {
  if (n) {
    lhs.first_line   = rhs[1].first_line;
    lhs.first_column = rhs[1].first_column;
    lhs.last_line    = rhs[n].last_line;
    lhs.last_column  = rhs[n].last_column;
  } else {
    lhs.first_line   = rhs[0].first_line;
    lhs.first_column = rhs[0].first_column;
    lhs.last_line    = rhs[0].last_line;
    lhs.last_column  = rhs[0].last_column;
  }
}
int comment_depth = 0;
int yycolumn = 1;
#define YY_USER_ACTION \
  yylloc->first_line = yylineno; \
  yylloc->first_column = yycolumn; \
  yylloc->last_line = yylineno; \
  yylloc->last_column = yycolumn + yyleng; \
  yycolumn += yyleng;
%}

%%

"+" {
  return PLUS;
}

";" {
  return SEMI;
}

"print" {
  return PRINT;
}

[0-9]+ {
  yylval->num = atoi(yytext);
  return INT;
}

\"[^\n"]*\" {
  yylval->str.text = yytext + 1;
  yylval->str.leng = yyleng - 2;
  return STR;
}

[ \t]+ {}

\n+ {
  yycolumn = 1;
}

"/*" {
  comment_depth = 0;
  BEGIN(COMMENT);
}

<COMMENT>"/*" {
  ++comment_depth;
}

<COMMENT>"*/" {
  --comment_depth;
  if (comment_depth <= 0) {
    BEGIN(INITIAL);
  }
}

<COMMENT>\n {
  yycolumn = 1;
}

<COMMENT>. {}

<COMMENT><<EOF>> {
  yyerror(yylloc, "EOF in comment");
}

. {
  yyerror(yylloc, "unknown char");
}

