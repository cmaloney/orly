%{
#include <tools/nycr/str.h>
%}

%code {
  int yylex(YYSTYPE *, YYLTYPE *);
  void yyerror(const YYLTYPE *, char const *);
}

%output "sample.bison.cc"
%defines
%locations
%pure_parser
%glr-parser
%error-verbose

%union {
  int num;
  str_t str;
}

%token PRINT SEMI
%token <num> INT
%token <str> STR
%left PLUS

%start prog

%type <num> expr
%type <str> stmt

%%

prog
  : opt_stmt_seq
;

stmt_seq
  : opt_stmt_seq stmt
;

opt_stmt_seq
  : stmt_seq
  |
;

stmt
  : PRINT expr SEMI {
    printf("expr = <%d>\n", $2);
  }
  | PRINT STR SEMI {
    printf("stmt = <");
    const char *cursor = $2.text, *limit = cursor + $2.leng;
    for (; cursor < limit; ++cursor) {
      printf("%c", *cursor);
    }
    printf(">\n");
  }
  | error SEMI {
    yyerror(&@1, "weird stmt")
  }
;

expr
  : expr PLUS expr {
    $$ = $1 + $3;
  }
  | INT {
    $$ = $1;
  }
;

%%

#include <iostream>

inline std::ostream &operator<<(std::ostream &strm, const YYLTYPE &that) {
  return strm << '(' << that.first_line << ':' << that.first_column << '-' << that.last_line << ':' << that.last_column << ')';
}

void yyerror(const YYLTYPE *loc, char const *msg) {
  std::cout << *loc << ' ' << msg << std::endl;
}

