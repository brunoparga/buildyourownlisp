/*
 * src/value.h
 *
 * Define a Lye language Value. Macros are provided to assert the type of a
 * Value, as well as functions to create and destroy Values and utilities for
 * manipulating them.
 *
 */
#ifndef lye_value_h
#define lye_value_h

#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include "../utils/realloc_string.h"

/* Forward declarations */
typedef char *Symbol;
typedef char *ErrorMsg;
typedef struct Value Value;
typedef struct Env Env;
typedef struct Function Function;

/* Enumerate possible Value types */
typedef enum { NUMBER, SYMBOL, FUNCTION, SEXPR, QEXPR, ERROR } ValueType;

/* Declare the S-expression struct */
typedef struct Sexpr {
  size_t count;
  struct Value **cell;
} Sexpr;

/* Define the Builtin function pointer type */
typedef Value *(*Builtin)(Env *, Value *);

/* Define the Value struct */
struct Value {
  ValueType type;
  union {
    double number;
    Symbol symbol;
    ErrorMsg error;
    struct Sexpr sexpr;
    struct Function *function;
  } data;
};

/* Macros that assert the type of a Value */
#define IS_NUMBER(value) (value->type == NUMBER)
#define IS_SYMBOL(value) (value->type == SYMBOL)
#define IS_FUNCTION(value) (value->type == FUNCTION)
#define IS_SEXPR(value) (value->type == SEXPR)
#define IS_QEXPR(value) (value->type == QEXPR)
#define IS_ERROR(value) (value->type == ERROR)

/* Value constructors and destructor */
Value *make_number(double number);
Value *make_symbol(Symbol symbol);
Value *make_builtin(Symbol name, Builtin function);
Value *make_lambda(Value *params, Value *body);
Value *make_sexpr(void);
Value *make_qexpr(void);
Value *make_error(char *format, ...);
Value *va_list_make_error(char *format, va_list pieces);
void delete_value(Value *value);

/* Utility functions for working with Values */
size_t count(Value *sexpr_value);
Value *element_at(Value *sexpr_value, size_t index);
char *get_type(Value *value);
char *stringify(Value *value);
void println_value(Value *value);
Value *pop_value(Value *value, size_t index);
Value *pop(Value *value);
Value *take_value(Value *value, size_t index);
Value *append_value(Value *list, Value *new_value);
Value *join_values(Value *left, Value *right);
Value *copy_value(Value *value);

#endif
