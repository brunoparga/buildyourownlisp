#ifndef lispy_value_h
#define lispy_value_h

/* Create Enumeration of possible Value types */
typedef enum { NUMBER, ERROR } ValueType;

/* Create Enumeration of possible error types */
typedef enum {
  ERROR_DIVISION_BY_ZERO,     // Division by zero.
  ERROR_INVALID_OPERAND,      // Invalid number (e.g. non-integer modulus).
  ERROR_NUMBER_OUT_OF_BOUNDS, // Number out of bounds.
  ERROR_UNKNOWN_OPERATOR,     // Unknown operator.
} Error;

/* Declare new Value struct */
typedef struct Value {
  ValueType type;
  union {
    double number;
    Error error;
  };
} Value;

Value make_number(double number);
Value make_error(Error error);
void print_value(Value value);

#endif
