#include "calc.h"

/*
 * src/calc.c:is_integer
 * buildyourownlisp.com correspondence: none
 *
 * Check if a float is close enough to an integer to be considered equal to it.
 * Used to make sure operands to `%` are valid, since all numbers are
 * represented as floats in Lye.
 *
 */
static int is_integer(double x) {
  double epsilon = 0.000001; // Enough for e^pi - pi != 20
  double difference = round(x) - x;
  return difference > -epsilon && difference < epsilon;
}

/*
 * src/calc.c:numeric_error
 * buildyourownlisp.com correspondence: contained within builtin_op
 *
 * Replace numeric values that are invalid for a given operation with an error
 * value.
 *
 */
static Value *numeric_error(Value *x, Value *y, char *format, ...) {
  delete_value(x);
  delete_value(y);

  va_list pieces;
  va_start(pieces, format);
  Value *error = va_list_make_error(format, pieces);
  va_end(pieces);

  return error;
}

/*
 * src/calc.c:calculate
 * buildyourownlisp.com correspondence: builtin_op
 *
 * Calculate numerical expressions. All built-in operations delegate to this
 * big function, which in turn does a switch-like series of conditionals; in
 * between, there is type checking and handling of the edge case of unary
 * negation.
 *
 */
static Value *calculate(Value *value, char *op) {
  /* Ensure all arguments are numbers */
  for (int index = 0; index < count(value); index++) {
    ASSERT_IS_NUMBER(value, index, op);
  }

  Value *result = pop(value);

#define IS_OP(str) (strcmp(op, str) == 0)

  /* Check for unary negation; the count is zero because the number
  has been popped a few lines above */
  if (IS_OP("-") && count(value) == 0) {
    result->number = -result->number;
  }

  /* Since we're using Polish Notation, each operation can take any number of
  operands, hence a while loop */
  while (count(value) > 0) {
    /* Pop the next element */
    Value *operand = pop(value);

    /* Check which operation is being performed and execute it */
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
      /* Since the modulo is the remainder of a division, its second operand
      cannot be zero */
      if (operand->number == 0) {
        result = numeric_error(result, operand, "modulus cannot be zero.");
        break;
      }
      if (is_integer(result->number) && is_integer(operand->number)) {
        /* We perform the operation only on integers */
        long int_result = (long)result->number;
        long int_operand = (long)operand->number;
        int_result %= int_operand;
        if (int_result < 0) {
          int_result += int_operand;
        }
        result->number = (double)int_result;
      } else {
        char *x = stringify(result);
        char *y = stringify(operand);
        result = numeric_error(
            result, operand,
            "operands of modulo must be integers, found %s and %s.", x, y);
        free(x);
        free(y);
        break;
      }
    } else if (IS_OP("^")) {
      /* No raising zero to a negative power */
      if (result->number == 0 && operand->number < 0) {
        char *x = stringify(operand);
        result = numeric_error(
            result, operand,
            "cannot raise 0 to negative power %s (requires dividing by 0).", x);
        free(x);
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

/*
 * src/calc.c:builtin_add
 * buildyourownlisp.com correspondence: builtin_add
 *
 * Add numbers.
 *
 */
Value *builtin_add(__attribute__((unused)) Env *env, Value *value) {
  return calculate(value, "+");
}

/*
 * src/calc.c:builtin_subtract
 * buildyourownlisp.com correspondence: builtin_sub
 *
 * Either subtract one or more numbers from the first one given, or change the
 * sign of a number given as a single argument.
 *
 */
Value *builtin_subtract(__attribute__((unused)) Env *env, Value *value) {
  return calculate(value, "-");
}

/*
 * src/calc.c:builtin_multiply
 * buildyourownlisp.com correspondence: builtin_mul
 *
 * Multiply numbers.
 *
 */
Value *builtin_multiply(__attribute__((unused)) Env *env, Value *value) {
  return calculate(value, "*");
}

/*
 * src/calc.c:builtin_divide
 * buildyourownlisp.com correspondence: builtin_div
 *
 * Divide numbers.
 *
 */
Value *builtin_divide(__attribute__((unused)) Env *env, Value *value) {
  return calculate(value, "/");
}

/*
 * src/calc.c:builtin_modulo
 * buildyourownlisp.com correspondence: none
 *
 * Return the remainder of the division of the first argument by the second.
 * Arguments must be integers and the result is always non-negative and less
 * than the second operand.
 *
 */
Value *builtin_modulo(__attribute__((unused)) Env *env, Value *value) {
  return calculate(value, "%");
}

/*
 * src/calc.c:builtin_exp
 * buildyourownlisp.com correspondence: none
 *
 * Perform exponentiation.
 *
 */
Value *builtin_exp(__attribute__((unused)) Env *env, Value *value) {
  return calculate(value, "^");
}

/*
 * src/calc.c:builtin_min
 * buildyourownlisp.com correspondence: none
 *
 * Return the smallest number in a list.
 *
 */
Value *builtin_min(__attribute__((unused)) Env *env, Value *value) {
  return calculate(value, "min");
}

/*
 * src/calc.c:builtin_max
 * buildyourownlisp.com correspondence: none
 *
 * Return the largest number in a list.
 *
 */
Value *builtin_max(__attribute__((unused)) Env *env, Value *value) {
  return calculate(value, "max");
}
