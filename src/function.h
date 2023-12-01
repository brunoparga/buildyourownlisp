/*
 * src/function.h
 *
 * Define C functions related to Lye functions - defining and calling them.
 *
 */
#ifndef lye_function_h
#define lye_function_h

#include "assert.h"
#include "value.h"

/* Define the Function struct */
typedef struct Function {
  /* These two are only by builtins */
  Symbol name;
  Builtin builtin;
  /* These three are only used by user-defined functions */
  Env *env;
  Value *params;
  Value *body;
} Function;

Value *call(Env *env, Value *fun, Value *args);
Value *builtin_def(Env *env, Value *value);
Value *builtin_lambda(Env *env, Value *code);

#endif
