#include "env.h"

Env *make_env() {
  Env *env = malloc(sizeof(Env));
  env->count = 0;
  env->keys = NULL;
  env->values = NULL;
  return env;
}

void delete_env(Env *env) {
  for (int index = 0; index < env->count; index++) {
    free(env->keys[index]);
    delete_value(env->values[index]);
  }
  free(env->keys);
  free(env->values);
  free(env);
}

Value *get_value(Env *env, Value *key) {
  if (!IS_SYMBOL(key)) {
    return make_error("environment keys must be Symbols.");
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
  return make_error("this symbol was not found to be bound to a value.");
}

void put_value(Env *env, Value *key, Value *value) {
  if (!IS_SYMBOL(key)) {
    printf("Error: environment keys must be Symbols.");
    return;
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

/* Make the given built-in available for any Lye program */
void register_builtin(Env *env, Symbol name, Builtin builtin) {
  Value *key = make_symbol(name);
  Value *function = make_function(builtin);
  put_value(env, key, function);
  delete_value(key);
  delete_value(function);
}

/* Register the set of built-ins */
void register_builtins(Env *env) {
  /* Function definition */
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
