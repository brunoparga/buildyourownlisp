/*
 * src/main.c
 *
 * The main module. Process the command line arguments passed to the Lye
 * interpreter, if any, and kick off evaluation of the code therein.
 *
 */
#include "../utils/file.h"

#include "repl.h"

/*
 * src/main.c:run_file
 * buildyourownlisp.com correspondence: none
 *
 * Interpret code contained in a Lye source file.
 *
 */
static void run_file(Env *env, char *filename) {
  char *source = read_file(filename);
  run_string(env, source);
  free(source);
}

/*
 * src/main.c:main
 * buildyourownlisp.com correspondence: {changing filename}, function `main`
 *
 * Run the Lye interpreter. If the `lye` command is called with no arguments,
 * it opens a REPL. If a single argument is passed, it is assumed to be a source
 * file which is then interpreted. The `-s` command line option allows for Lye
 * code, enclosed within quote marks, to be passed directly as an argument to be
 * executed.
 *
 */
int main(int argc, char **argv) {
  Env *environment = make_env();
  register_builtins(environment);

  if (argc == 1) {
    puts("Lye Version 0.0.0.11");
    puts("Enter 'quit' to exit\n");
    repl(environment);
  } else if (argc == 2) {
    run_file(environment, argv[1]);
  } else if (argc == 3 && strcmp(argv[1], "-s") == 0) {
    run_string(environment, argv[2]);
  }

  delete_env(environment);

  return 0;
}
