#include "repl.h"

/*
 * src/repl.c:run_string
 * buildyourownlisp.com correspondence: main
 *
 * Execute a string of Lye code. This function abstracts away the origin of the
 * source code: a file, REPL entry or string passed as a command-line argument.
 *
 */
void run_string(Env *env, char *source) {
  Parser *parser = create_parser();
  Value *value = parse(env, parser, source);
  println_value(value);
  delete_value(value);
  cleanup_parser(parser);
}

/*
 * src/repl.c:repl
 * buildyourownlisp.com correspondence: main
 *
 * Run a REPL (Read-Eval-Print Loop); read inputted Lye code, evaluate the
 * expressions within it and print the final value, then loop to the start.
 *
 */
void repl(Env *env) {
  for (;;) {
    char *input = readline("lye> ");
    if (strcmp(input, "quit") == 0) {
      free(input);
      return;
    }
    add_history(input);
    run_string(env, input);
    free(input);
  }
}
