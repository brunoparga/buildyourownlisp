#include "env.h"

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

/*
 * src/env.c:register_builtin
 * buildyourownlisp.com correspondence: lenv_add_builtin
 *
 * Make the given built-in available for any Lye program.
 *
 */
void register_builtin(Env *env, Symbol name, Builtin builtin) {
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
  /* Variable definition */
  register_builtin(env, "def", builtin_def);

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
