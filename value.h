#ifndef lispy_value_h
#define lispy_value_h

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Create Enumeration of possible Value types */
typedef enum { NUMBER, SYMBOL, SEXPR, QEXPR, ERROR } ValueType;

typedef char *Symbol;
typedef char *ErrorMsg;

/* Declare new S-expression struct */
typedef struct Sexpr {
  int count;
  struct Value **cell;
} Sexpr;

/* Declare new Value struct */
typedef struct Value {
  ValueType type;
  union {
    double number;
    Symbol symbol;
    ErrorMsg error;
    struct Sexpr sexpr;
  };
} Value;

/* Value constructors and destructor */
Value *make_number(double number);
Value *make_symbol(Symbol symbol);
Value *make_sexpr(void);
Value *make_qexpr(void);
Value *make_error(ErrorMsg error);
void delete_value(Value *value);

/* Utility functions for working with Values */
int count(Value *sexpr_value);
Value *element_at(Value *sexpr_value, int index);
void print_value(Value *value);
Value *pop_value(Value *value, int index);
Value *take_value(Value *value, int index);

#endif
