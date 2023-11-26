/*
 * src/expressionizer.h
 *
 * Expose a function to convert an Abstract Syntax Tree (AST) to a Lye Value.
 *
 */
#ifndef lye_expressionizer_h
#define lye_expressionizer_h

#include "../lib/mpc.h"
#include "value.h"

Value *expressionize(mpc_ast_t *ast);

#endif
