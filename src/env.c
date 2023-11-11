#include "env.h"
#include "value.h"

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
  return make_error("no such key found.");
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