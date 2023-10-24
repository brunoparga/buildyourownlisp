#include "list.h"

#include "eval.h"

#define LIST_ASSERT(list, condition, error_message)                            \
  if (!(condition)) {                                                          \
    delete_value(list);                                                        \
    return make_error(error_message);                                          \
  }

#define ASSERT_ONE_ARG(list, error_message)                                    \
  LIST_ASSERT(list, count(list) == 1, error_message)
#define ASSERT_CONTAINS_VALUES(list, error_message)                            \
  LIST_ASSERT(list, count(element_at(list, 0)) > 0, error_message)

Value *builtin_list(Value *value) {
  value->type = QEXPR;
  return value;
}

Value *builtin_head(Value *value) {
  /* Check error conditions */
  ASSERT_ONE_ARG(value, "function 'head' passed too many arguments.")
  LIST_ASSERT(value, IS_QEXPR(element_at(value, 0)),
              "function 'head' passed incorrect type.")
  ASSERT_CONTAINS_VALUES(value,
                         "function 'head' passed invalid empty list.")

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
  ASSERT_ONE_ARG(value, "function 'tail' passed too many arguments.")
  LIST_ASSERT(value, IS_QEXPR(element_at(value, 0)),
              "function 'tail' passed incorrect type.")
  ASSERT_CONTAINS_VALUES(value,
                         "function 'tail' passed invalid empty list.")

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
  ASSERT_ONE_ARG(value, "function 'eval' passed too many arguments.")
  LIST_ASSERT(value, IS_QEXPR(element_at(value, 0)),
              "function 'eval' passed incorrect types.")

  Value *sexpr = take_value(value, 0);
  sexpr->type = SEXPR;
  return evaluate(sexpr);
}
