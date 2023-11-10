#ifndef lye_list_h
#define lye_list_h

#include "value.h"

Value *builtin_list(Value *value);
Value *builtin_head(Value *value);
Value *builtin_tail(Value *value);
Value *builtin_join(Value *value);
Value *builtin_eval(Value *value);
Value *builtin_cons(Value *value);
Value *builtin_length(Value *value);
Value *builtin_reverse(Value *value);
Value *builtin_init(Value *value);

#endif
