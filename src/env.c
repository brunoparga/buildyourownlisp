#include "env.h"
#include "value.h"

// ==========================
// Constructor and destructor
// ==========================

/*
 * src/env.c:make_env
 * buildyourownlisp.com correspondence: lenv_new
 *
 * Create an empty Environment.
 *
 */
Env *make_env() {
  Env *env = malloc(sizeof(Env));
  env->count = 0;
  env->keys = NULL;
  env->values = NULL;
  return env;
}

/*
 * src/env.c:delete_env
 * buildyourownlisp.com correspondence: lenv_del
 *
 * Release the memory used by an Env, at the end of the program.
 *
 */
void delete_env(Env *env) {
  for (int index = 0; index < env->count; index++) {
    free(env->keys[index]);
    delete_value(env->values[index]);
  }
  free(env->keys);
  free(env->values);
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
  for (int index = 0; index < env->count; index++) {
    /* Check if the stored string matches the symbol string */
    if (strcmp(env->keys[index], key->symbol) == 0) {
      /* If it does, return a copy of the value */
      return copy_value(env->values[index]);
    }
  }

  /* If no matching key found, return an error */
  return make_error("unbound symbol '%s'.", key->symbol);
}

/*
 * src/env.c:put_value
 * buildyourownlisp.com correspondence: lenv_put
 *
 * Insert a value into the corresponding key, updating it if already there.
 *
 */
void put_value(Env *env, Value *key, Value *value) {
  // Must be called with a Symbol key, or everything crashes
  if (!IS_SYMBOL(key)) {
    exit(EX_SOFTWARE);
  }

  /* First, check if the key is already present */
  for (int index = 0; index < env->count; index++) {
    /* If it is... */
    if (strcmp(env->keys[index], key->symbol) == 0) {
      /* Delete the existing value... */
      delete_value(env->values[index]);
      /* And substitute the provided one */
      env->values[index] = copy_value(value);
      return;
    }
  }

  /* If the key is not found, make space for the new key-value pair */
  env->count++;
  env->keys = realloc(env->keys, sizeof(Symbol) * env->count);
  env->values = realloc(env->values, sizeof(Value *) * env->count);

  /* Insert the new key and value */
  env->keys[env->count - 1] = malloc(strlen(key->symbol) + 1);
  strcpy(env->keys[env->count - 1], key->symbol);
  env->values[env->count - 1] = copy_value(value);
}

// =================================
// Lye environment-related functions
// =================================

/*
 * src/list.c:builtin_def
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
  ASSERT(value, count(symbols) == count(value) - 1, "def",
         "the same number of symbols and values.");

  /* Assign copies of values to symbols */
  for (int index = 0; index < count(symbols); index++) {
    put_value(env, element_at(symbols, index), element_at(value, index + 1));
  }

  delete_value(value);
  return make_symbol("def");
}

/*
 * src/env.c:print_env
 * buildyourownlisp.com correspondence: none
 *
 * Print all the variables in the environment and their corresponding Values.
 *
 */
static void print_env(Env *env) {
  printf("Current environment:\n");
  for (int index = 0; index < env->count; index++) {
    printf("    %s: ", env->keys[index]);
    println_value(env->values[index]);
  }
  printf("There are a total of %d variables defined.\n", env->count);
}

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
  Value *function = make_function(builtin, name);
  put_value(env, key, function);
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
  /* Environment operations */
  register_builtin(env, "def", builtin_def);
  register_builtin(env, "print-env", builtin_print_env);

  /* List operations */
  register_builtin(env, "list", builtin_list);
  register_builtin(env, "head", builtin_head);
  register_builtin(env, "tail", builtin_tail);
  register_builtin(env, "join", builtin_join);
  register_builtin(env, "eval", builtin_eval);
  register_builtin(env, "cons", builtin_cons);
  register_builtin(env, "length", builtin_length);
  register_builtin(env, "reverse", builtin_reverse);
  register_builtin(env, "init", builtin_init);

  /* Arithmetical operations*/
  register_builtin(env, "+", builtin_add);
  register_builtin(env, "-", builtin_subtract);
  register_builtin(env, "*", builtin_multiply);
  register_builtin(env, "/", builtin_divide);
  register_builtin(env, "%", builtin_modulo);
  register_builtin(env, "^", builtin_exp);
  register_builtin(env, "min", builtin_min);
  register_builtin(env, "max", builtin_max);
}
