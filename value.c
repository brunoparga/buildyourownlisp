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

/* Create a new Q-expression Value */
Value *make_qexpr() {
  Value *value = malloc(sizeof(Value));
  value->type = QEXPR;
  value->sexpr.count = 0;
  value->sexpr.cell = NULL;
  return value;
}

/* Create a new error Value */
Value *make_error(ErrorMsg error) {
  Value *value = malloc(sizeof(Value));
  value->type = ERROR;
  value->error = malloc(strlen(error) + 1);
  strcpy(value->error, error);
  return value;
}

void delete_value(Value *value) {
  switch (value->type) {
  /* Do nothing special for number type */
  case NUMBER:
    break;
  /* For Symbol (and ErrorMsg, below), free the string data */
  case SYMBOL:
    free(value->symbol);
    break;
  /* For Sexpr delete all the elements inside */
  case SEXPR:
  case QEXPR:
    for (int index = 0; index < count(value); index++) {
      delete_value(element_at(value, index));
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

/* Return the count of values in the cell of an S-expression */
int count(Value *sexpr_value) {
  switch (sexpr_value->type) {
  case QEXPR:
  case SEXPR:
    return sexpr_value->sexpr.count;
  default:
    printf(
        "Error: Value is not an S- or Q-expression and doesn't have a count.");
    return -1;
  }
}

/* Return the element at the given index, contained in an S- or Q-expression */
Value *element_at(Value *sexpr_value, int index) {
  switch (sexpr_value->type) {
  case QEXPR:
  case SEXPR:
    return sexpr_value->sexpr.cell[index];
  default:
    return make_error(
        "cannot get element from a Value that is not an S- or Q-expression.");
  }
}

/* Print a Value */
static void print_value_no_newline(Value *value);

static void print_expression(Value *value, char open, char close) {
  putchar(open);
  for (int index = 0; index < count(value); index++) {
    /* Print Value contained within */
    print_value_no_newline(element_at(value, index));

    /* Don't print trailing space if last element */
    if (index != count(value) - 1) {
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
  case QEXPR:
    print_expression(value, '{', '}');
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

Value *pop_value(Value *value, int index) {
  /* Find the value to pop */
  Value *popped = element_at(value, index);

  /* Shift memory after the popped item over the top*/
  memmove(&value->sexpr.cell[index], &value->sexpr.cell[index + 1],
          sizeof(Value *) * (count(value) - index - 1));

  /* Decrement the size of the list */
  value->sexpr.count--;

  /* Reallocate the memory used */
  value->sexpr.cell =
      realloc(value->sexpr.cell, sizeof(Value *) * count(value));
  return popped;
}

Value *take_value(Value *value, int index) {
  Value *popped = pop_value(value, index);
  delete_value(value);
  return popped;
}
