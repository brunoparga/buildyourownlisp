#include "function.h"

#include "env.h"

/*
 * src/function.c:call
 * buildyourownlisp.com correspondence: lval_call
 *
 * Call a function, whether builtin or user-defined.
 */
Value *call(Env *env, Value *fun, Value *args) {
  /* If the function is a builtin we simply call that */
  if (fun->function->builtin) {
    return fun->function->builtin(env, args);
  }

  int argc = count(args);
  int paramc = count(fun->function->params);

  /* Check if the function was given too many parameters */
  if (argc > paramc) {
    Value *error = make_error(
        "function %s passed too many arguments: expected %d but got %d.",
        fun->function->name, paramc, argc);
    delete_value(fun);
    delete_value(args);
    return error;
  }

  /* Assign arguments to parameters */
  for (int index; index < argc; index++) {
    put_value(fun->function->env, fun->function->params->sexpr.cell[index],
              args->sexpr.cell[index], 0);
  }
  delete_value(args);

  if (argc == paramc) {
    /* Evaluate a fully applied function */
    fun->function->env->parent = env;
    return builtin_eval(
        fun->function->env,
        append_value(make_sexpr(), copy_value(fun->function->body)));
  } else {
    /* Return a partially applied function */
    return copy_value(fun);
  }
}

/*
 * src/function.c:builtin_def
 * buildyourownlisp.com correspondence: builtin_def
 *
 * Syntax: (def {list of length n} a b c ... n)
 * Put values in the environment. The elements of the first argument list are
 * keys, and their corresponding values are the subsequent arguments, in the
 * same order as the list.
 *
 */
Value *builtin_def(Env *env, Value *value) {
  ASSERT_IS_LIST(value, 0, "def");

  /* The first argument is a list of symbols */
  Value *symbols = element_at(value, 0);

  /* Ensure all the elements are indeed symbols */
  for (int index = 0; index < count(symbols); index++) {
    ASSERT_IS_SYMBOL(value, element_at(symbols, index), "def");
  }

  /* Ensure the number of symbols and values matches */
  ASSERT(value, count(symbols) == count(value) - 1, BASE_FORMAT, "def",
         "the same number of symbols and values.");

  /* Assign copies of values to symbols. The operation might fail if the user
  tries to redefine a Lye builtin. */
  Value *maybe_error;
  int index;
  for (index = 0; index < count(symbols); index++) {
    maybe_error = put_value(env, element_at(symbols, index),
                            element_at(value, index + 1), 0);
  }

  return IS_ERROR(maybe_error) ? maybe_error : value->sexpr.cell[index];
}