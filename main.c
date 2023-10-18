#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "mpc.h"

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

/* Create Enumeration of possible Value types */
typedef enum { NUMBER, ERROR } ValueType;

/* Create Enumeration of possible error types */
typedef enum {
  ERROR_DIVISION_BY_ZERO,     // Division by zero.
  ERROR_INVALID_OPERAND,      // Invalid number (e.g. non-integer modulus).
  ERROR_NUMBER_OUT_OF_BOUNDS, // Number out of bounds.
  ERROR_UNKNOWN_OPERATOR,     // Unknown operator.
} Error;

/* Declare new Value struct */
typedef struct Value {
  ValueType type;
  union {
    double number;
    Error error;
  };
} Value;

/* Create a new number Value */
Value make_number(double number) {
  Value value;
  value.type = NUMBER;
  value.number = number;
  return value;
}

/* Create a new error Value */
Value make_error(Error error) {
  Value value;
  value.type = ERROR;
  value.error = error;
  return value;
}

/* Print a Value */
void print_value(Value value) {
  switch (value.type) {
  case NUMBER:
    if (roundf(value.number) == value.number) {
      printf("%li", (long)value.number);
    } else {
      printf("%f", value.number);
    }
    break;
  case ERROR:
    switch (value.error) {
    case ERROR_DIVISION_BY_ZERO:
      printf("Error: cannot divide by zero.");
      break;
    case ERROR_INVALID_OPERAND:
      printf("Error: invalid number for this operation.");
      break;
    case ERROR_NUMBER_OUT_OF_BOUNDS:
      printf("Error: number outside of valid bounds.");
      break;
    case ERROR_UNKNOWN_OPERATOR:
      printf("Error: unknown operator.");
      break;
    }
    break;
  }
}

/* Print a Value followed by a line break */
void println_value(Value value) {
  print_value(value);
  putchar('\n');
}

/* Perform numeric operations */
Value compute(double x, char *op, double y) {
  double result;
  if (strcmp(op, "+") == 0) {
    result = x + y;
  } else if (strcmp(op, "-") == 0) {
    result = x - y;
  } else if (strcmp(op, "*") == 0) {
    result = x * y;
  } else if (strcmp(op, "/") == 0) {
    result = x / y;
  } else if (strcmp(op, "%") == 0) {
    result = (long)x % (long)y;
  } else if (strcmp(op, "^") == 0) {
    result = pow(x, y);
  } else if (strcmp(op, "min") == 0) {
    result = fmin(x, y);
  } else if (strcmp(op, "max") == 0) {
    result = fmax(x, y);
  } else {
    return make_error(ERROR_UNKNOWN_OPERATOR);
  }

  return make_number(result);
}

/* Test if a NUMBER is round */
int isInt(Value value) { return roundf(value.number) == value.number; }

/* Check for errors and if there are none dispatch for calculation */
Value eval_op(Value x, char *op, Value y) {
  /* If either value is an error return it */
  if (x.type == ERROR) {
    return x;
  }
  if (y.type == ERROR) {
    return y;
  }
  if (strcmp(op, "/") == 0 && y.number == 0) {
    return make_error(ERROR_DIVISION_BY_ZERO);
  }
  if (strcmp(op, "%") == 0 && (!isInt(x) || !isInt(y))) {
    return make_error(ERROR_INVALID_OPERAND);
  }

  return compute(x.number, op, y.number);
}

Value eval(mpc_ast_t *t) {
  /* If tagged as number return it directly */
  if (strstr(t->tag, "number")) {
    errno = 0;
    double number = strtod(t->contents, NULL);
    return errno == ERANGE ? make_error(ERROR_NUMBER_OUT_OF_BOUNDS)
                           : make_number(number);
  }

  /* The operator is always the second child */
  char *op = t->children[1]->contents;

  /* We store the third child in `x` */
  Value x = eval(t->children[2]);

  /* Iterate the remaining children and combine */
  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }

  /* Check for unary minus */
  if (strcmp(op, "-") == 0 && i == 3) {
    return make_number(-x.number);
  }

  return x;
}

int main(int argc, char **argv) {
  /* Create some parsers */
  mpc_parser_t *Number = mpc_new("number");
  mpc_parser_t *Operator = mpc_new("operator");
  mpc_parser_t *Expr = mpc_new("expr");
  mpc_parser_t *Lispy = mpc_new("lispy");

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
    char *input = readline("lispy> ");
    add_history(input);

    /* Attempt to parse the user input */
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      /* On success print the result */
      Value result = eval(r.output);
      println_value(result);
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
