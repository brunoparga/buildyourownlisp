/*
 * src/main.c
 *
 * The main module. Process the command line arguments passed to the Lye
 * interpreter, if any, and kick off evaluation of the code therein.
 *
 */
#include "../utils/file.h"

#include "repl.h"

static void run_file(Env *env, Parser *parser, char *filename) {
  char *source = read_file(filename);
  run_string(env, parser, source);
  free(source);
}

int main(int argc, char **argv) {
  Parser *parser = create_parser();
  Env *environment = make_env();
  register_builtins(environment);

  if (argc == 1) {
    puts("Lye Version 0.0.0.4");
    puts("Press Ctrl+c to Exit\n");
    repl(environment, parser);
  } else if (argc == 2) {
    run_file(environment, parser, argv[1]);
  } else if (argc == 3 && strcmp(argv[1], "-s") == 0) {
    run_string(environment, parser, argv[2]);
  }

  cleanup_parser(parser);
  delete_env(environment);

  return 0;
}
