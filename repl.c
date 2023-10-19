#include "repl.h"

void repl(Parser* parser) {
  for (;;) {
    /* Now in either case readline will be correctly defined */
    char *input = readline("lispy> ");
    add_history(input);
    Value *value = parse(parser, input);
    print_value(value);
    delete_value(value);
    free(input);
  }
}
