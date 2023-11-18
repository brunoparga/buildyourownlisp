#ifndef lye_calc_h
#define lye_calc_h

#include <math.h>
#include <stdarg.h>

#include "assert.h"
#include "value.h"

Value *builtin_add(Env *env, Value *value);
Value *builtin_subtract(Env *env, Value *value);
Value *builtin_multiply(Env *env, Value *value);
Value *builtin_divide(Env *env, Value *value);
Value *builtin_modulo(Env *env, Value *value);
Value *builtin_exp(Env *env, Value *value);
Value *builtin_min(Env *env, Value *value);
Value *builtin_max(Env *env, Value *value);

#endif
