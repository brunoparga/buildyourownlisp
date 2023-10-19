#ifndef lispy_calc_h
#define lispy_calc_h

#include <math.h>
#include <string.h>

#include "value.h"

Value *compute(Value *value, Symbol *op);

#endif
