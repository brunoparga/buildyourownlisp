#include "list.h"

#include "eval.h"
#include "value.h"

#define LIST_ASSERT(list, condition, function, error_message)                  \
  if (!(condition)) {                                                          \
    delete_value(list);                                                        \
    return make_error(ERROR_MESSAGE(function, error_message));                 \
  }

#define ERROR_MESSAGE(function_name, message)                                  \
  "function '" #function_name "' must be passed " #message

#define ASSERT_ARGC(list, argc, function, error_message)                       \
  LIST_ASSERT(list, count(list) == argc, function,                             \
              ERROR_MESSAGE(function_name, error_message))

#define ASSERT_ONE_ARG(list, function)                                         \
  ASSERT_ARGC(list, 1, function, "exactly one argument.")
#define ASSERT_TWO_ARGS(list, function)                                        \
  ASSERT_ARGC(list, 2, function, "exactly two arguments.")

#define ASSERT_IS_LIST(list, index, function)                                  \
  LIST_ASSERT(list, IS_QEXPR(element_at(list, index)), function,               \
              ERROR_MESSAGE(function, "a list."))
#define ASSERT_CONTAINS_VALUES(list, function)                                 \
  LIST_ASSERT(list, count(element_at(list, 0)) > 0, function,                  \
              ERROR_MESSAGE(function, "a list with at least one element."))

Value *builtin_list(Value *value) {
  value->type = QEXPR;
  return value;
}

Value *builtin_head(Value *value) {
  /* Check error conditions */
  char *function = "head";
  ASSERT_ONE_ARG(value, function)
  ASSERT_IS_LIST(value, 0, function)
  ASSERT_CONTAINS_VALUES(value, function)

  /* Otherwise take the first element. */
  Value *result = take_value(value, 0)->sexpr.cell[0];

  /* Delete all elements that are not head and return */
  while (result->sexpr.count > 1) {
    delete_value(pop_value(result, 1));
  }

  return result;
}

Value *builtin_tail(Value *value) {
  /* Check error conditions */
  char *function = "tail";
  ASSERT_ONE_ARG(value, function)
  ASSERT_IS_LIST(value, 0, function)
  ASSERT_CONTAINS_VALUES(value, function)

  /* Take first argument */
  Value *result = take_value(value, 0);

  /* Delete first argument and return */
  delete_value(pop(result));
  return result;
}

Value *builtin_join(Value *value) {
  for (int index = 0; index < count(value); index++) {
    ASSERT_IS_LIST(value, index, "join")
  }

  // Build result from the first list contained in the sexpr argument
  Value *result = pop(value);

  // As long as there are other lists
  while (count(value) > 0) {
    // and elements in the first list
    while (count(value->sexpr.cell[0]) > 0) {
      // append the first element of the first list to the result
      Value *next = pop(value->sexpr.cell[0]);
      result = append_value(result, next);
    }
    // We pop from the S-expr the Q-expr being appended, which is now empty
    pop(value);
  }

  delete_value(value);
  return result;
}

Value *builtin_eval(Value *value) {
  char *function = "eval";
  ASSERT_ONE_ARG(value, function)
  ASSERT_IS_LIST(value, 0, function)

  Value *sexpr = take_value(value, 0);
  sexpr->type = SEXPR;
  return evaluate(sexpr);
}
