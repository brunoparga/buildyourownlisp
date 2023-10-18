#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "value.h"

/* Create a new number Value */
Value *make_number(double number) {
  Value *value = malloc(sizeof(Value));
  value->type = NUMBER;
  value->number = number;
  return value;
}

/* Create a new symbol Value */
Value *make_symbol(Symbol symbol) {
  Value *value = malloc(sizeof(Value));
  value->type = SYMBOL;
  value->symbol = malloc(strlen(symbol) + 1);
  strcpy(value->symbol, symbol);
  return value;
}

/* Create a new S-expression Value */
Value *make_sexpr() {
  Value *value = malloc(sizeof(Value));
  value->type = SEXPR;
  value->sexpr.count = 0;
  value->sexpr.cell = NULL;
  return value;
}

/* Create a new error Value */
Value *make_error(Error error) {
  Value *value = malloc(sizeof(Value));
  value->type = ERROR;
  value->error = malloc(strlen(error) + 1);
  strcpy(value->error, error);
  return value;
}

/* Print a Value */
static void print_value_no_newline(Value *value);

void print_expression(Value *value, char open, char close) {
  putchar(open);
  for (int i = 0; i < value->sexpr.count; i++) {
    /* Print Value contained within */
    print_value_no_newline(value->sexpr.cell[i]);

    /* Don't print trailing space if last element */
    if (i != value->sexpr.count - 1) {
      putchar(' ');
    }
  }

  putchar(close);
}

static void print_value_no_newline(Value *value) {
  switch (value->type) {
  case NUMBER:
    if (roundf(value->number) == value->number) {
      printf("%li", (long)value->number);
    } else {
      printf("%f", value->number);
    }
    break;
  case SYMBOL:
    printf("%s", value->symbol);
    break;
  case SEXPR:
    print_expression(value, '(', ')');
    break;
  case ERROR:
    printf("Error: %s", value->error);
    break;
  }
}

void print_value(Value *value) {
  print_value_no_newline(value);
  putchar('\n');
}

void free_value(Value *value) {
  switch (value->type) {
  /* Do nothing special for number type */
  case NUMBER:
    break;
  /* For Symbol (and Error, below), free the string data */
  case SYMBOL:
    free(value->symbol);
    break;
  /* For Sexpr delete all the elements inside */
  case SEXPR:
    for (int i = 0; i < value->sexpr.count; i++) {
      free_value(value->sexpr.cell[i]);
    }
    /* Also free the memory allocated to contain the pointers */
    free(value->sexpr.cell);
    break;
  case ERROR:
    free(value->error);
    break;
  }

  /* Free the memory allocated for the Value itself */
  free(value);
}
