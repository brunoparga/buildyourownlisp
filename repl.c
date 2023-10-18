/* If we are compiling on Windows compile these functions */
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

/* Fake readline function */
char *readline(char *prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char *cpy = malloc(strlen(buffer) + 1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy) - 1] = '\0';
  return cpy;
}

/* Fake add_history function */
void add_history(char *unused) {}

#else
#include <editline/readline.h>
#endif

#include "lib/mpc.h"

#include "eval.h"
#include "parser.h"
#include "value.h"

void repl(Parser* parser) {
  for (;;) {
    /* Now in either case readline will be correctly defined */
    char *input = readline("lispy> ");
    add_history(input);
    parse(parser, input);
    free(input);
  }
}
