#include "repl.h"

void repl(Parser* parser) {
  for (;;) {
    /* Now in either case readline will be correctly defined */
    char *input = readline("lispy> ");
    add_history(input);
    parse(parser, input);
    free(input);
  }
}
