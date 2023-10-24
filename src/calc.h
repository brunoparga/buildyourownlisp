#ifndef lispy_calc_h
#define lispy_calc_h

#include <math.h>
#include <string.h>

#include "value.h"

Value *builtin_op(Value *value, Symbol *op);

#endif
