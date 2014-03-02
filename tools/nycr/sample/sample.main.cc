#include <cstdio>
#include <cstdlib>
#include <set>

extern FILE *yyin;
void yyparse();

int main(int /*argc*/, char *argv[]) {
  yyin = fopen(argv[1], "r");
  if (!yyin) {
    perror(argv[1]);
    return EXIT_FAILURE;
  }
  yyparse();
  return EXIT_SUCCESS;
}

