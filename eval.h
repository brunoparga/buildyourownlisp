#ifndef lispy_eval_h
#define lispy_eval_h

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "lib/mpc.h"

#include "calc.h"
#include "value.h"

/* Test if a Value's number is round */
static inline int isInt(Value value) {
  return roundf(value.number) == value.number;
}

Value eval_op(Value x, char *op, Value y);
Value eval(mpc_ast_t *t);

#endif
