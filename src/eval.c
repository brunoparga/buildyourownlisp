#include "eval.h"

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

  if (count(value) == 1) {
    return take_value(value, 0);
  }

  /* Ensure first element is a function after evaluation */
  Value *first = pop(value);
  if (!IS_FUNCTION(first)) {
    delete_value(first);
    delete_value(value);
    return make_error("S-expression must start with a function.");
  }

  /* Call function */
  Value *result = first->function(env, value);
  delete_value(first);
  return result;
}

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
    /* All other Value types remain the same */
    return value;
  }
}
