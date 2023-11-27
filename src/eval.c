#include "eval.h"

/*
 * src/eval.c:evaluate_sexpr
 * buildyourownlisp.com correspondence: lval_eval_sexpr
 *
 * Evaluate an S-expression, which is in effect a function call.
 *
 */
static Value *evaluate_sexpr(Env *env, Value *value) {
  /* Evaluate children */
  for (int index = 0; index < count(value); index++) {
    value->sexpr.cell[index] = evaluate(env, element_at(value, index));
    if (IS_ERROR(element_at(value, index))) {
      return take_value(value, index);
    }
  }

  /* Empty expression, () */
  if (count(value) == 0) {
    return value;
  }

  /* Singleton expression */
  if (count(value) == 1) {
    return take_value(value, 0);
  }

  /* Ensure first element is a function after evaluation */
  Value *first = pop(value);
  if (!IS_FUNCTION(first)) {
    Value *error =
        make_error("S-expression must start with a function, found type %s.",
                   get_type(first));
    delete_value(first);
    delete_value(value);
    return error;
  }

  /* Call function */
  value = first->function.body(env, value);
  delete_value(first);
  return value;
}

/*
 * src/eval.c:evaluate
 * buildyourownlisp.com correspondence: lval_eval
 *
 * Evaluate a Lye expression.
 *
 */
Value *evaluate(Env *env, Value *value) {
  switch (value->type) {
  case SYMBOL: {
    /* Variable access */
    Value *variable = get_value(env, value);
    delete_value(value);
    return variable;
  }
  case SEXPR:
    /* Expand S-expressions */
    return evaluate_sexpr(env, value);
  default:
    /* All other Value types remain the same. Notably, since Q-Expressions
    are meant to be just quoted but not evaluated, they fall within this
    case. */
    return value;
  }
}
