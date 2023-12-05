/*
 * src/env.h
 *
 * Define the Environment (Env) where variables and functions are stored.
 * Expose functions to create and destroy Envs and store and retrieve values
 * within them.
 *
 */
#ifndef lye_env_h
#define lye_env_h

#include <stdlib.h>
#include <string.h>

#include "calc.h"
#include "function.h"
#include "list.h"
#include "value.h"

/*
 * buildyourownlisp.com correspondence: lenv
 *
 * Define the Env struct. It is implemented as a pair of lists plus their
 * length, with the invariant that the length is equal for both.
 *
 */
struct Env {
  Env *parent;
  size_t count;
  Symbol *keys;
  Value **values;
  bool *is_builtin;
};

/* Environment constructors and destructor */
Env *make_env();
Env *copy_env(Env const *old);
void delete_env(Env *env);

/* Store and retrieve */
Value *get_value(Env *env, Value *key);
Value *put_local_value(Env *env, Value *key, Value *value, bool is_builtin);
Value *put_global_value(Env *env, Value *key, Value *value, bool is_builtin);

/* Register language built-ins */
void register_builtins(Env *env);

#endif