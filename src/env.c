#include "env.h"

#define BUILTINS_COUNT 20
char builtin_names[BUILTINS_COUNT][10] = {
    "def",  "\\",   "print-env", "list",    "eval", "head", "tail",
    "join", "cons", "length",    "reverse", "init", "+",    "-",
    "*",    "/",    "^",         "%",       "min",  "max"};

// ===========================
// Constructors and destructor
// ===========================

/*
 * src/env.c:make_env
 * buildyourownlisp.com correspondence: lenv_new
 *
 * Create an empty Environment.
 *
 */
Env *make_env() {
  Env *env = malloc(sizeof(Env));
  env->parent = NULL;
  env->count = 0;
  env->keys = NULL;
  env->values = NULL;
  env->is_builtin = NULL;
  return env;
}

/*
 * src/env.c:copy_env
 * buildyourownlisp.com correspondence: lenv_copy
 *
 * Copy an existing Environment. Useful when copying Values that have Envs as
 * properties.
 *
 */
Env *copy_env(Env const *old) {
  Env *new = malloc(sizeof(Env));

  new->parent = old->parent;
  new->count = old->count;
  new->keys = malloc(sizeof(Symbol) * new->count);
  new->values = malloc(sizeof(Value *) * new->count);
  new->is_builtin = malloc(sizeof(bool) * new->count);

  for (size_t index = 0; index < old->count; index++) {
    new->keys[index] = malloc(strlen(old->keys[index]) + 1);
    strcpy(new->keys[index], old->keys[index]);
    new->values[index] = copy_value(old->values[index]);
  }

  return new;
}

/*
 * src/env.c:delete_env
 * buildyourownlisp.com correspondence: lenv_del
 *
 * Release the memory used by an Env, at the end of the program.
 *
 */
void delete_env(Env *env) {
  for (size_t index = 0; index < env->count; index++) {
    free(env->keys[index]);
    delete_value(env->values[index]);
  }
  free(env->keys);
  free(env->values);
  free(env->is_builtin);
  free(env);
}

// ==================
// Store and retrieve
// ==================

/*
 * src/env.c:get_value
 * buildyourownlisp.com correspondence: lenv_get
 *
 * Return the value associated with the given key in the given Env.
 *
 */
Value *get_value(Env *env, Value *key) {
  /* Must be called with a Symbol key, or everything crashes */
  if (!IS_SYMBOL(key)) {
    exit(EX_SOFTWARE);
  }

  /* Iterate over all items in the environment */
  for (size_t index = 0; index < env->count; index++) {
    /* Check if the stored string matches the symbol string */
    if (strcmp(env->keys[index], key->data.symbol) == 0) {
      /* If it does, return a copy of the value */
      return copy_value(env->values[index]);
    }
  }

  /* Check the parent environment. If no matching key is found all the way up
   * to the root environment, return an error */
  if (env->parent) {
    return get_value(env->parent, key);
  }

  return make_error("unbound symbol '%s'.", key->data.symbol);
}

/*
 * src/env.c:put_local_value
 * buildyourownlisp.com correspondence: lenv_put
 *
 * Insert a value into the given Environment at the corresponding key, updating
 * it if already there.
 *
 */
Value *put_local_value(Env *env, Value *key, Value *value, bool is_builtin) {
  // Must be called with a Symbol key, or everything crashes
  if (!IS_SYMBOL(key)) {
    exit(EX_SOFTWARE);
  }

  Value *value_copy = copy_value(value);

  for (size_t index = 0; index < env->count; index++) {
    /* First, check if the key is already present */
    if (strcmp(env->keys[index], key->data.symbol) == 0) {
      /* If it is... */
      if (env->is_builtin[index] || strcmp(key->data.symbol, "quit") == 0) {
        /* And it is not a builtin... */
        delete_value(value);
        delete_value(value_copy);
        return make_error("cannot redefine builtin function %s.",
                          key->data.symbol);
      } else {
        /* Substitute the provided one */
        env->values[index] = value_copy;
        env->is_builtin[index] = is_builtin;
        return value;
      }
    }
  }

  /* If the key is not found, make space for the new key-value pair */
  env->count++;
  env->keys = realloc(env->keys, sizeof(Symbol) * env->count);
  env->values = realloc(env->values, sizeof(Value *) * env->count);
  env->is_builtin = realloc(env->is_builtin, sizeof(bool) * env->count);

  /* Insert the new key and value */
  env->keys[env->count - 1] = malloc(strlen(key->data.symbol) + 1);
  strcpy(env->keys[env->count - 1], key->data.symbol);
  env->values[env->count - 1] = value_copy;
  env->is_builtin[env->count - 1] = is_builtin;

  /* Return the inserted Value */
  return value;
}

/*
 * src/env.c:put_global_value
 * buildyourownlisp.com correspondence: lenv_def
 *
 * Insert a value intothe global environment at the corresponding key, updating
 * it if already there.
 *
 */
Value *put_global_value(Env *env, Value *key, Value *value, bool is_builtin) {
  while (env->parent) {
    env = env->parent;
  }
  return put_local_value(env, key, value, is_builtin);
}

// =================================
// Lye environment-related functions
// =================================

/*
 * src/env.c:print_env
 * buildyourownlisp.com correspondence: none
 *
 * Print all the variables in the environment and their corresponding Values.
 *
 */
static void print_env(Env *env) {
  printf("Current environment:\n");
  for (size_t index = 0; index < env->count; index++) {
    printf("    %s: ", env->keys[index]);
    println_value(env->values[index]);
  }
  printf("There are a total of %zu variables defined.\n", env->count);
}

/*
 * src/env.c:builtin_print_env
 * buildyourownlisp.com correspondence: none
 *
 * A Lye interface to the C function `print_env`.
 *
 */
Value *builtin_print_env(Env *env, Value *value) {
  print_env(env);
  delete_value(value);
  return make_symbol("print-env");
}

// =======================
// Lye builtin registering
// =======================

/*
 * src/env.c:register_builtin
 * buildyourownlisp.com correspondence: lenv_add_builtin
 *
 * Make the given built-in available for any Lye program.
 *
 */
static void register_builtin(Env *env, Symbol name, Builtin builtin) {
  Value *key = make_symbol(name);
  Value *function = make_builtin(name, builtin);
  put_global_value(env, key, function, true);
  delete_value(key);
  delete_value(function);
}

/*
 * src/env.c:register_builtins
 * buildyourownlisp.com correspondence: lenv_add_builtins
 *
 * Register the set of all built-ins.
 *
 */
void register_builtins(Env *env) {
  Builtin builtin_functions[BUILTINS_COUNT] = {
      /* Environment and function operations */
      builtin_def, builtin_lambda, builtin_print_env,

      /* List operations */
      builtin_list, builtin_eval, builtin_head, builtin_tail, builtin_join,
      builtin_cons, builtin_length, builtin_reverse, builtin_init,

      /* Arithmetical operations*/
      builtin_add, builtin_subtract, builtin_multiply, builtin_divide,
      builtin_exp, builtin_modulo, builtin_min, builtin_max};

  for (size_t index = 0; index < BUILTINS_COUNT; index++) {
    register_builtin(env, builtin_names[index], builtin_functions[index]);
  }

  /* `quit` is a fake builtin */
  Value *quit = make_symbol("quit");
  put_global_value(env, quit, quit, true);
  delete_value(quit);
}
