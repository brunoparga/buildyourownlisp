#include "calc.h"

static int is_integer(double x) {
  double epsilon = 0.000001; // Enough for e^pi - pi != 20
  double difference = round(x) - x;
  return difference > -epsilon && difference < epsilon;
}

Value *numeric_error(Value *x, Value *y, ErrorMsg message) {
  delete_value(x);
  delete_value(y);
  return make_error(message);
}

/* Calculate numerical expressions */
static Value *builtin_op(Value *value, char *op) {
  /* Ensure all arguments are numbers */
  for (int index = 0; index < count(value); index++) {
    if (!IS_NUMBER(element_at(value, index))) {
      delete_value(value);
      return make_error(
          "can only operate on numbers. Found a value that is not a number.");
    }
  }

  Value *result = pop(value);

#define IS_OP(str) (strcmp(op, str) == 0)

  /* Check for unary negation */
  if (IS_OP("-") && count(value) == 0) {
    result->number = -result->number;
  }

  while (count(value) > 0) {
    /* Pop the next element */
    Value *operand = pop(value);

    if (IS_OP("+")) {
      result->number += operand->number;
    } else if (IS_OP("-")) {
      result->number -= operand->number;
    } else if (IS_OP("*")) {
      result->number *= operand->number;
    } else if (IS_OP("/")) {
      if (operand->number == 0) {
        result = numeric_error(result, operand, "cannot divide by zero.");
        break;
      }
      result->number /= operand->number;
    } else if (IS_OP("%")) {
      if (operand->number == 0) {
        result = numeric_error(result, operand, "modulus cannot be zero.");
        break;
      }
      if (is_integer(result->number) && is_integer(operand->number)) {
        long int_result = (long)result->number;
        long int_operand = (long)operand->number;
        int_result %= int_operand;
        if (int_result < 0) {
          int_result += int_operand;
        }
        result->number = (double)int_result;
      } else {
        result = numeric_error(result, operand, "operands of modulo must be integers.");
        break;
      }
    } else if (IS_OP("^")) {
      if (result->number == 0 && operand->number < 0) {
        result = numeric_error(result, operand, "cannot raise 0 to a negative power (requires dividing by 0).");
        break;
      }
      result->number = pow(result->number, operand->number);
    } else if (IS_OP("max")) {
      result->number = fmax(result->number, operand->number);
    } else if (IS_OP("min")) {
      result->number = fmin(result->number, operand->number);
    }

    delete_value(operand);
  }

#undef IS_OP

  delete_value(value);
  return result;
}

Value *builtin_add(__attribute__((unused)) Env *env, Value *value) {
  return builtin_op(value, "+");
}
Value *builtin_subtract(__attribute__((unused)) Env *env, Value *value) {
  return builtin_op(value, "-");
}
Value *builtin_multiply(__attribute__((unused)) Env *env, Value *value) {
  return builtin_op(value, "*");
}
Value *builtin_divide(__attribute__((unused)) Env *env, Value *value) {
  return builtin_op(value, "/");
}
Value *builtin_modulo(__attribute__((unused)) Env *env, Value *value) {
  return builtin_op(value, "%");
}
Value *builtin_exp(__attribute__((unused)) Env *env, Value *value) {
  return builtin_op(value, "^");
}
Value *builtin_min(__attribute__((unused)) Env *env, Value *value) {
  return builtin_op(value, "min");
}
Value *builtin_max(__attribute__((unused)) Env *env, Value *value) {
  return builtin_op(value, "max");
}
