#include "repl.h"

/*
 * src/repl.c:run_string
 * buildyourownlisp.com correspondence: main
 *
 * Execute a string of Lye code. This function abstracts away the origin of the
 * source code: a file, REPL entry or string passed as a command-line argument.
 *
 */
void run_string(Env *env, Parser *parser, char *source) {
  Value *value = parse(env, parser, source);
  print_value(value);
  delete_value(value);
}

/*
 * src/repl.c:repl
 * buildyourownlisp.com correspondence: main
 *
 * Run a REPL (Read-Eval-Print Loop); read inputted Lye code, evaluate the
 * expressions within it and print the final value, then loop to the start.
 *
 */
void repl(Env *env, Parser *parser) {
  for (;;) {
    char *input = readline("lye> ");
    add_history(input);
    run_string(env, parser, input);
    free(input);
  }
}
