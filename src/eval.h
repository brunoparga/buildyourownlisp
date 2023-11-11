#ifndef lye_eval_h
#define lye_eval_h

#include "calc.h"
#include "env.h"
#include "list.h"
#include "value.h"

Value *evaluate(Env *env, Value *value);

#endif
