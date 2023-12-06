/*
 * src/repl.h
 *
 * Define the Read-Eval-Print Loop and expose `run_string`, the function that
 * takes in source code and runs it.
 *
 */
#ifndef lye_repl_h
#define lye_repl_h

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

#include "parser.h"

void run_string(Env *env, char *source);
void repl(Env *env);

#endif
