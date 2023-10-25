#include "repl.h"

void repl(Parser *parser) {
  for (;;) {
    char *input = readline("lye> ");
    add_history(input);
    Value *value = parse(parser, input);
    print_value(value);
    delete_value(value);
    free(input);
  }
}
