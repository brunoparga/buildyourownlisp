#include "eval.h"

static Value *builtin(Value *value, Symbol *function) {
  if (strcmp("list", *function) == 0) {
    return builtin_list(value);
  }
  if (strcmp("head", *function) == 0) {
    return builtin_head(value);
  }
  if (strcmp("tail", *function) == 0) {
    return builtin_tail(value);
  }
  if (strcmp("join", *function) == 0) {
    return builtin_join(value);
  }
  if (strcmp("eval", *function) == 0) {
    return builtin_eval(value);
  }
  if (strstr("+-*/^%", *function) || strcmp(*function, "max") == 0 ||
      strcmp(*function, "min")) {
    return builtin_op(value, function);
  }

  delete_value(value);
  return make_error("undefined function.");
}

static Value *evaluate_sexpr(Value *value) {
  /* Evaluate children */
  for (int index = 0; index < count(value); index++) {
    value->sexpr.cell[index] = evaluate(element_at(value, index));
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

  /* Ensure first element is a Symbol */
  Value *first = pop(value);
  if (!IS_SYMBOL(first)) {
    delete_value(first);
    delete_value(value);
    return make_error("S-expression does not start with a symbol.");
  }

  /* Pass operator for calculation */
  Value *result = builtin(value, &first->symbol);
  delete_value(first);
  return result;
}

Value *evaluate(Value *value) {
  /* Evaluate S-expressions */
  if (IS_SEXPR(value)) {
    return evaluate_sexpr(value);
  }

  /* All other Value types remain the same */
  return value;
}
