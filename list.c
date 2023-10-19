#include "list.h"

#include "eval.h"
#include "value.h"

#define LIST_ASSERT(list, condition, error_message)                            \
  if (!(condition)) {                                                          \
    delete_value(list);                                                        \
    return make_error(error_message);                                          \
  }

Value *builtin_list(Value *value) {
  value->type = QEXPR;
  return value;
}

Value *builtin_head(Value *value) {
  /* Check error conditions */
  LIST_ASSERT(value, count(value) != 1,
              "function 'head' passed too many arguments.")
  LIST_ASSERT(value, !IS_QEXPR(element_at(value, 0)),
              "function 'head' passed incorrect types.")
  LIST_ASSERT(value, count(element_at(value, 0)) == 0,
              "function 'head' passed invalid empty Q-expression.")

  /* Otherwise take the first argument. */
  Value *result = take_value(value, 0);

  /* Delete all elements that are not head and return */
  while (result->sexpr.count > 1) {
    delete_value(pop_value(result, 1));
  }

  return result;
}

Value *builtin_tail(Value *value) {
  /* Check error conditions */
  LIST_ASSERT(value, count(value) != 1,
              "function 'tail' passed too many arguments.")
  LIST_ASSERT(value, !IS_QEXPR(element_at(value, 0)),
              "function 'tail' passed incorrect types.")
  LIST_ASSERT(value, count(element_at(value, 0)) == 0,
              "function 'tail' passed invalid empty Q-expression.")

  /* Take first argument */
  Value *result = take_value(value, 0);

  /* Delete first argument and return */
  delete_value(pop(result));
  return result;
}

Value *builtin_join(Value *value) {
  for (int index = 0; index < count(value); index++) {
    LIST_ASSERT(value, IS_QEXPR(element_at(value, index)),
                "function 'join' passed incorrect type.")
  }

  Value *result = pop(value);

  while (count(value) > 0) {
    result = append_value(result, pop(value));
  }

  delete_value(value);
  return result;
}

Value *builtin_eval(Value *value) {
  LIST_ASSERT(value, count(value) != 1,
              "function 'eval' passed too many arguments.")
  LIST_ASSERT(value, !IS_QEXPR(element_at(value, 0)),
              "function 'eval' passed incorrect types.")

  Value *sexpr = take_value(value, 0);
  sexpr->type = SEXPR;
  return evaluate(sexpr);
}
