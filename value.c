#include <math.h>
#include <stdio.h>

#include "value.h"

/* Create a new number Value */
Value make_number(double number) {
  Value value;
  value.type = NUMBER;
  value.number = number;
  return value;
}

/* Create a new error Value */
Value make_error(Error error) {
  Value value;
  value.type = ERROR;
  value.error = error;
  return value;
}

/* Print a Value */
void print_value(Value value) {
  switch (value.type) {
  case NUMBER:
    if (roundf(value.number) == value.number) {
      printf("%li", (long)value.number);
    } else {
      printf("%f", value.number);
    }
    break;
  case ERROR:
    switch (value.error) {
    case ERROR_DIVISION_BY_ZERO:
      printf("Error: cannot divide by zero.");
      break;
    case ERROR_INVALID_OPERAND:
      printf("Error: invalid number for this operation.");
      break;
    case ERROR_NUMBER_OUT_OF_BOUNDS:
      printf("Error: number outside of valid bounds.");
      break;
    case ERROR_UNKNOWN_OPERATOR:
      printf("Error: unknown operator.");
      break;
    }
    break;
  }

  putchar('\n');
}
