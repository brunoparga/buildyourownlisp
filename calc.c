#include "calc.h"

/* Ensure all arguments are numbers; if so, dispatch for calculation */
// This function is mostly stubbed out due to the code reorganization
Value *compute(Value *value, Symbol *op) {
  for (int index = 0; index < count(value); index++) {
    if (element_at(value, index)->type != NUMBER) {
      delete_value(value);
      return make_error(
          "can only operate on numbers. Found a value that is not a number.");
    }
  }

// #define IS_OP(str) (strcmp(*op, str) == 0)

//   Value *x = pop_value(value, 0)

//       double result;

//   if (IS_OP("+")) {
//     result = x + y;
//   } else if (IS_OP("-")) {
//     result = x - y;
//   } else if (IS_OP("*")) {
//     result = x * y;
//   } else if (IS_OP("/")) {
//     result = x / y;
//   } else if (IS_OP("%")) {
//     // Values are assured to be integers.
//     result = (long)x % (long)y;
//   } else if (IS_OP("^")) {
//     result = pow(x, y);
//   } else if (IS_OP("min")) {
//     result = fmin(x, y);
//   } else if (IS_OP("max")) {
//     result = fmax(x, y);
//   } else {
// #undef IS_OP
//     return make_error("unknown operator.");
//   }

// #undef IS_OP

//   return make_number(result);

  return value;
}
