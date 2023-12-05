#include "eval.h"

// We need to treat `print-env` different from other singleton values
static inline bool is_singleton(Value *value, Value *first) {
  return count(value) == 0 &&
         (!IS_FUNCTION(first) ||
          (first->data.function->builtin &&
           strcmp(first->data.function->name, "print-env") != 0));
}

/*
 * src/eval.c:evaluate_sexpr
 * buildyourownlisp.com correspondence: lval_eval_sexpr
 *
 * Evaluate an S-expression, which is in effect a function call.
 *
 */
static Value *evaluate_sexpr(Env *env, Value *value) {
  /* Empty expression, () */
  if (count(value) == 0) {
    return value;
  }

  /* Evaluate children */
  for (size_t index = 0; index < count(value); index++) {
    value->data.sexpr.cell[index] = evaluate(env, element_at(value, index));
    if (IS_ERROR(element_at(value, index))) {
      return take_value(value, index);
    }
  }

  /* Singleton expression */
  Value *first = pop(value);
  if (is_singleton(value, first)) {
    delete_value(value);
    return first;
  }

  /* Ensure first element is a function after evaluation */
  if (!IS_FUNCTION(first)) {
    Value *error =
        make_error("S-expression must start with a function, found type %s.",
                   get_type(first));
    delete_value(first);
    delete_value(value);
    return error;
  }

  /* Call function */
  value = call(env, first, value);
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
