#ifndef lye_expressionizer_h
#define lye_expressionizer_h

#include "../lib/mpc.h"
#include "value.h"

Value *expressionize(mpc_ast_t *t);
#endif
