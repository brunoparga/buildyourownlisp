#include "repl.h"

void run_string(Env *env, Parser *parser, char *source) {
  Value *value = parse(env, parser, source);
  print_value(value);
  delete_value(value);
}

void repl(Env *env, Parser *parser) {
  for (;;) {
    char *input = readline("lye> ");
    add_history(input);
    run_string(env, parser, input);
    free(input);
  }
}
