#include "repl.h"

void run_string(Parser *parser, char *source) {
  Value *value = parse(parser, source);
  print_value(value);
  delete_value(value);
}

void repl(Parser *parser) {
  for (;;) {
    char *input = readline("lye> ");
    add_history(input);
    run_string(parser, input);
    free(input);
  }
}
