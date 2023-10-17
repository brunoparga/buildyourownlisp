#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "mpc.h"

/* If we are compiling on Windows compile these functions */
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

/* Fake readline function */
char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer) + 1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy) - 1] = '\0';
  return cpy;
}

/* Fake add_history function */
void add_history(char* unused) {}

#else
#include <editline/readline.h>
#endif

/* Use operator string to see which operation to perform */
double eval_op(double x, char* op, double y) {
  if (strcmp(op, "+") == 0) {
    return x + y;
  }
  if (strcmp(op, "-") == 0) {
    return x - y;
  }
  if (strcmp(op, "*") == 0) {
    return x * y;
  }
  if (strcmp(op, "/") == 0) {
    return x / y;
  }
  if (strcmp(op, "%") == 0) {
    if (roundf(x) == x && roundf(y) == y) {
      return (long)x % (long)y;
    }
  }
  if (strcmp(op, "^") == 0) {
    return pow(x, y);
  }
  if (strcmp(op, "min") == 0) {
    return fmin(x, y);
  }
  if (strcmp(op, "max") == 0) {
    return fmax(x, y);
  }
  return NAN;
}

double eval(mpc_ast_t* t) {

  /* If tagged as number return it directly */
  if(strstr(t->tag, "number")) {
    return atof(t->contents);
  }

  /* The operator is always the second child */
  char* op = t->children[1]->contents;

  /* We store the third child in `x` */
  double x = eval(t->children[2]);

  /* Iterate the remaining children and combine */
  int i = 3;
  while(strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }

  /* Check for unary minus */
  if (strcmp(op, "-") == 0 && i == 3) {
    return -x;
  }

  return x;
}

int main(int argc, char** argv) {
  /* Create some parsers */
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Lispy = mpc_new("lispy");

  char *language = "                                                      \
    number   : /-?[0-9]+(\\.[0-9]+)?/ ;                                   \
    operator : '+' | '-' | '*' | '/' | \"%\" | '^' | \"min\" | \"max\" ;  \
    expr     : <number> | '(' <operator> <expr>+ ')' ;                    \
    lispy    : /^/ <operator> <expr>+ /$/ ;                               \
  ";

  /* Define them with the following Language */
  mpca_lang(MPCA_LANG_DEFAULT, language, Number, Operator, Expr, Lispy);

  puts("Lispy Version 0.0.0.4");
  puts("Press Ctrl+c to Exit\n");

  while (1) {

    /* Now in either case readline will be correctly defined */
    char* input = readline("lispy> ");
    add_history(input);

    /* Attempt to parse the user input */
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      /* On success print the result */
      double result = eval(r.output);
      if (roundf(result) == result) {
        printf("%li\n", (long)result);
      } else {
        printf("%f\n", result);
      }
      mpc_ast_delete(r.output);
    } else {
      /* Otherwise print the error */
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);
  }

  /* Undefine and Delete our Parsers */
  mpc_cleanup(4, Number, Operator, Expr, Lispy);
  return 0;
}
