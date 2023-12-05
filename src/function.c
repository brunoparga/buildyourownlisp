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
  if (fun->data.function->builtin) {
    return fun->data.function->builtin(env, args);
  }

  size_t argc = count(args);
  size_t paramc = count(fun->data.function->params);

  /* Check if the function was given too many parameters */
  if (argc > paramc) {
    Value *error = make_error(
        "function %s passed too many arguments: expected %d but got %d.",
        fun->data.function->name, paramc, argc);
    delete_value(fun);
    delete_value(args);
    return error;
  }

  /* Assign arguments to parameters */
  for (size_t index = 0; index < argc; index++) {
    put_local_value(fun->data.function->env,
                    fun->data.function->params->data.sexpr.cell[index],
                    args->data.sexpr.cell[index], false);
  }
  delete_value(args);

  if (argc == paramc) {
    /* Evaluate a fully applied function */
    fun->data.function->env->parent = env;
    return builtin_eval(
        fun->data.function->env,
        append_value(make_sexpr(), copy_value(fun->data.function->body)));
  }

  /* Return a partially applied function */
  return copy_value(fun);
}

/*
 * src/function.c:builtin_var
 * buildyourownlisp.com correspondence: builtin_var
 *
 * Put values in the corresponding Environment: local if `fun` is "=", global
 * if "def".
 *
 */
static Value *builtin_var(Env *env, Value *value, char *fun) {
  ASSERT_IS_LIST(value, 0, fun);

  /* The first argument is a list of symbols */
  Value *symbols = element_at(value, 0);

  /* Ensure all the elements are indeed symbols */
  ASSERT_ALL_SYMBOLS(symbols, fun);

  /* Ensure the number of symbols and values matches */
  ASSERT(value, count(symbols) == count(value) - 1, BASE_FORMAT, fun,
         "the same number of symbols and values.");

  /* Assign copies of values to symbols. The operation might fail if the user
  tries to redefine a Lye builtin. */
  Value *maybe_error;
  size_t index;
  for (index = 0; index < count(symbols); index++) {
    Value *(*put_value)(Env *, Value *, Value *, bool) =
        strcmp(fun, "def") == 0 ? put_global_value : put_local_value;
    maybe_error = put_value(env, element_at(symbols, index),
                            element_at(value, index + 1), false);
  }

  if (IS_ERROR(maybe_error)) {
    delete_value(value);
    return maybe_error;
  }
  return value->data.sexpr.cell[index];
}

/*
 * src/function.c:builtin_def
 * buildyourownlisp.com correspondence: builtin_def
 *
 * Syntax: (def {list of length n} a b c ... n)
 * Put values in the global environment. The elements of the first argument list
 * are keys, and their corresponding values are the subsequent arguments, in the
 * same order as the list.
 *
 */
Value *builtin_def(Env *env, Value *value) {
  return builtin_var(env, value, "def");
}

/*
 * src/function.c:builtin_lambda
 * buildyourownlisp.com correspondence: builtin_lambda
 *
 * Syntax: (def {list of params} {body as a Q-expr})
 * Define a new function and store it in the Environment.
 *
 */
Value *builtin_lambda(__attribute__((unused)) Env *env, Value *code) {
  ASSERT_ARGC(code, 2, "\\");
  ASSERT_IS_LIST(code, 0, "\\");
  ASSERT_IS_LIST(code, 1, "\\");

  Value *params = pop(code);
  ASSERT_ALL_SYMBOLS(params, "\\");
  Value *body = pop(code);
  delete_value(code);

  return make_lambda(params, body);
}
