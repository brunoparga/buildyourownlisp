#ifndef lispy_value_h
#define lispy_value_h

/* Create Enumeration of possible Value types */
typedef enum { NUMBER, SYMBOL, SEXPR, ERROR } ValueType;

typedef char *Symbol;
typedef char *Error;

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
    Error error;
    struct Sexpr sexpr;
  };
} Value;

Value *make_number(double number);
Value *make_symbol(Symbol symbol);
Value *make_sexpr(void);
Value *make_error(Error error);
int count(Value *sexpr_value);
Value *element_at(Value *sexpr_value, int index);
void print_value(Value *value);
void free_value(Value *value);

#endif
