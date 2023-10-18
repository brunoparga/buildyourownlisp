#include <math.h>
#include <string.h>

#include "value.h"

/* Perform numeric operations */
Value compute(double x, char *op, double y) {
#define IS_OP(str) (strcmp(op, str) == 0)

  double result;

  if (IS_OP("+")) {
    result = x + y;
  } else if (IS_OP("-")) {
    result = x - y;
  } else if (IS_OP("*")) {
    result = x * y;
  } else if (IS_OP("/")) {
    result = x / y;
  } else if (IS_OP("%")) {
    // Values are assured to be integers.
    result = (long)x % (long)y;
  } else if (IS_OP("^")) {
    result = pow(x, y);
  } else if (IS_OP("min")) {
    result = fmin(x, y);
  } else if (IS_OP("max")) {
    result = fmax(x, y);
  } else {
#undef IS_OP
    return make_error(ERROR_UNKNOWN_OPERATOR);
  }

#undef IS_OP

  return make_number(result);
}
