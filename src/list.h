#ifndef lye_list_h
#define lye_list_h

#include "value.h"

Value *builtin_list(Env *env, Value *value);
Value *builtin_head(Env *env, Value *value);
Value *builtin_tail(Env *env, Value *value);
Value *builtin_join(Env *env, Value *value);
Value *builtin_eval(Env *env, Value *value);
Value *builtin_cons(Env *env, Value *value);
Value *builtin_length(Env *env, Value *value);
Value *builtin_reverse(Env *env, Value *value);
Value *builtin_init(Env *env, Value *value);

#endif
