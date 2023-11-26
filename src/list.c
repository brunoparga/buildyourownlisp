#include "list.h"

// Included here and not in header file to avoid circular dependency
#include "eval.h"

Value *builtin_list(__attribute__((unused)) Env *env, Value *value) {
  value->type = QEXPR;
  return value;
}

Value *builtin_head(__attribute__((unused)) Env *env, Value *value) {
  /* Check error conditions */
  ASSERT_ARGC(value, 1, "head");
  ASSERT_IS_LIST(value, 0, "head");
  ASSERT_CONTAINS_VALUES(value, "head");

  /* Otherwise take the first element. */
  value = take_value(value, 0);

  /* Delete all elements that are not head and return */
  while (value->sexpr.count > 1) {
    delete_value(pop_value(value, 1));
  }

  /* Return the actual first value, not a list containing it */
  Value *result = pop(value);
  delete_value(value);
  return result;
}

Value *builtin_tail(__attribute__((unused)) Env *env, Value *value) {
  /* Check error conditions */
  ASSERT_ARGC(value, 1, "tail");
  ASSERT_IS_LIST(value, 0, "tail");
  ASSERT_CONTAINS_VALUES(value, "tail");

  /* Take first argument */
  Value *result = take_value(value, 0);

  /* Delete first argument and return */
  delete_value(pop(result));
  return result;
}

Value *builtin_join(__attribute__((unused)) Env *env, Value *value) {
  for (int index = 0; index < count(value); index++) {
    ASSERT_IS_LIST(value, index, "join");
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
    delete_value(pop(value));
  }

  delete_value(value);
  return result;
}

Value *builtin_eval(Env *env, Value *value) {
  ASSERT_ARGC(value, 1, "eval");
  ASSERT_IS_LIST(value, 0, "eval");

  Value *sexpr = take_value(value, 0);
  sexpr->type = SEXPR;
  return evaluate(env, sexpr);
}

Value *builtin_cons(__attribute__((unused)) Env *env, Value *value) {
  ASSERT_ARGC(value, 2, "cons");
  ASSERT_IS_LIST(value, 1, "cons");

  Value *new_element = element_at(value, 0);
  Value *list = element_at(value, 1);

  /* Set new size of Q-expr */
  list->sexpr.count++;

  /* Allocate larger memory */
  list->sexpr.cell = realloc(list->sexpr.cell, sizeof(Value *) * count(list));

  /* Shift the list one element to the right */
  for (int index = count(list) - 1; index > 0; index--) {
    list->sexpr.cell[index] = list->sexpr.cell[index - 1];
  }

  /* Set the correct pointers */
  list->sexpr.cell[0] = new_element;
  value = list;

  return value;
}

Value *builtin_length(__attribute__((unused)) Env *env, Value *value) {
  ASSERT_ARGC(value, 1, "length");
  ASSERT_IS_LIST(value, 0, "length");

  /* What we want is the Q-expr contained within this S-expr */
  Value *list = value->sexpr.cell[0];
  Value *result = make_number((double)count(list));
  delete_value(value);
  return result;
}

Value *builtin_reverse(__attribute__((unused)) Env *env, Value *value) {
  ASSERT_ARGC(value, 1, "reverse");
  ASSERT_IS_LIST(value, 0, "reverse");

  Value *list = element_at(value, 0);
  int length = count(list);

  for (int index = 0; index < length / 2; index++) {
    Value *tmp = list->sexpr.cell[index];
    list->sexpr.cell[index] = list->sexpr.cell[length - index - 1];
    list->sexpr.cell[length - index - 1] = tmp;
  }

  free(value);
  return list;
}

Value *builtin_init(__attribute__((unused)) Env *env, Value *value) {
  ASSERT_ARGC(value, 1, "init");
  ASSERT_IS_LIST(value, 0, "init");
  ASSERT_CONTAINS_VALUES(value, "init");

  value = element_at(value, 0);
  delete_value(pop_value(value, count(value) - 1));

  return value;
}

Value *builtin_def(Env *env, Value *value) {
  ASSERT_IS_LIST(value, 0, "def");

  /* The first argument is a list of symbols */
  Value *symbols = element_at(value, 0);

  /* Ensure all the elements are indeed symbols */
  for (int index = 0; index < count(symbols); index++) {
    ASSERT_IS_SYMBOL(value, element_at(symbols, index), "def");
  }

  /* Ensure the number of symbols and values matches */
  ASSERT(value, count(symbols) == count(value) - 1, "def",
         "the same number of symbols and values.");

  /* Assign copies of values to symbols */
  for (int index = 0; index < count(symbols); index++) {
    put_value(env, element_at(symbols, index), element_at(value, index + 1));
  }

  delete_value(value);
  return make_sexpr();
}
