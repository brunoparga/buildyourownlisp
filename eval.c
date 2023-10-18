#include "eval.h"

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
  if (strcmp(op, "%") == 0) {
    if (!isInt(x) || !isInt(y)) {
      return make_error(ERROR_INVALID_OPERAND);
    } else if (y.number == 0) {
      return make_error(ERROR_DIVISION_BY_ZERO);
    }
  }

  return compute(x.number, op, y.number);
}

Value eval(mpc_ast_t *t) {
  /* If the AST node is a number return it directly */
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
  for (int i = 3; strstr(t->children[i]->tag, "expr"); i++) {
    x = eval_op(x, op, eval(t->children[i]));
  }

  /* Check for unary minus */
  if (strcmp(op, "-") == 0 && i == 3) {
    return make_number(-x.number);
  }

  return x;
}
