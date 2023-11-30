#include "list.h"

// Included here and not in header file to avoid circular dependency
#include "eval.h"

/*
 * src/list.c:builtin_list
 * buildyourownlisp.com correspondence: builtin_list
 *
 * Take any number of arguments and return a list containing them.
 * While conceptually the Lye function may take any number of arguments, the
 * C implementation takes only one – an S-Expression with the values as
 * children. Therefore, all that is needed is to change the type of this Value.
 *
 */
Value *builtin_list(__attribute__((unused)) Env *env, Value *value) {
  value->type = QEXPR;
  return value;
}

/*
 * src/list.c:builtin_head
 * buildyourownlisp.com correspondence: builtin_head
 *
 * Take a list and return its first element. The semantics differ from BYOL
 * in that the actual element is returned, not a list containing it.
 *
 */
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

  /* Return the actual first element */
  Value *result = pop(value);
  delete_value(value);
  return result;
}

/*
 * src/list.c:builtin_tail
 * buildyourownlisp.com correspondence: builtin_tail
 *
 * Take a list and return a list containing all of its elements but the first
 * one.
 *
 */
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

/*
 * src/list.c:builtin_join
 * buildyourownlisp.com correspondence: builtin_join
 *
 * Take one or more lists and join them together into a single list, preserving
 * any nesting.
 *
 */
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

/*
 * src/list.c:builtin_eval
 * buildyourownlisp.com correspondence: builtin_eval
 *
 * Convert a Q-Expression into an S-Expression and evaluate it.
 *
 */
Value *builtin_eval(Env *env, Value *value) {
  ASSERT_ARGC(value, 1, "eval");
  ASSERT_IS_LIST(value, 0, "eval");

  Value *sexpr = take_value(value, 0);
  sexpr->type = SEXPR;
  return evaluate(env, sexpr);
}

/*
 * src/list.c:builtin_cons
 * buildyourownlisp.com correspondence: none
 *
 * Take a Value and a list and prepend the Value to the list.
 *
 */
Value *builtin_cons(__attribute__((unused)) Env *env, Value *value) {
  ASSERT_ARGC(value, 2, "cons");
  ASSERT_IS_LIST(value, 1, "cons");

  Value *new_element = copy_value(element_at(value, 0));
  Value *list = copy_value(element_at(value, 1));
  delete_value(value);

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

  return list;
}

/*
 * src/list.c:builtin_length
 * buildyourownlisp.com correspondence: none
 *
 * Take a list and return the number of its elements.
 *
 */
Value *builtin_length(__attribute__((unused)) Env *env, Value *value) {
  ASSERT_ARGC(value, 1, "length");
  ASSERT_IS_LIST(value, 0, "length");

  /* What we want is the Q-expr contained within this S-expr */
  Value *list = value->sexpr.cell[0];
  Value *result = make_number((double)count(list));
  delete_value(value);
  return result;
}

/*
 * src/list.c:builtin_reverse
 * buildyourownlisp.com correspondence: none
 *
 * Take a list and return all its elements in reverse order.
 *
 */
Value *builtin_reverse(__attribute__((unused)) Env *env, Value *value) {
  ASSERT_ARGC(value, 1, "reverse");
  ASSERT_IS_LIST(value, 0, "reverse");

  Value *list = copy_value(element_at(value, 0));
  int length = count(list);

  for (int index = 0; index < length / 2; index++) {
    Value *tmp = list->sexpr.cell[index];
    list->sexpr.cell[index] = list->sexpr.cell[length - index - 1];
    list->sexpr.cell[length - index - 1] = tmp;
  }

  delete_value(value);
  return list;
}

/*
 * src/list.c:builtin_init
 * buildyourownlisp.com correspondence: none
 *
 * Take a list and return a new list with all its elements but the last one.
 * Conceptually equivalent to reverse-tail-reverse.
 *
 */
Value *builtin_init(__attribute__((unused)) Env *env, Value *value) {
  ASSERT_ARGC(value, 1, "init");
  ASSERT_IS_LIST(value, 0, "init");
  ASSERT_CONTAINS_VALUES(value, "init");

  Value *result = copy_value(element_at(value, 0));
  delete_value(value);
  delete_value(pop_value(result, count(result) - 1));

  return result;
}
