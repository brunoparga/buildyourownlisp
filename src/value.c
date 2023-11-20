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

/* Create a new function Value */
Value *make_function(Builtin builtin, Symbol name) {
  Value *value = malloc(sizeof(Value));
  value->type = FUNCTION;
  value->function.name = malloc(strlen(name) + 1);
  strcpy(value->function.name, name);
  value->function.body = builtin;
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
Value *make_error(char *format, ...) {
  Value *value = malloc(sizeof(Value));
  value->type = ERROR;

  /* Create and initialize a list of arguments */
  va_list pieces;
  va_start(pieces, format);

  /* Allocate 512 bytes of space */
  value->error = malloc(512);

  /* printf the error string with a maximum of 511 characters */
  vsnprintf(value->error, 511, format, pieces);

  /* Reallocate to the number of bytes actually used */
  value->error = realloc(value->error, strlen(value->error) + 1);

  /* Clean up our arg list */
  va_end(pieces);

  return value;
}

Value *va_list_make_error(char *format, va_list pieces) {
  char *error = malloc(512);
  vsprintf(error, format, pieces);
  Value *value = make_error(error);
  free(error);
  return value;
}

void delete_value(Value *value) {
  switch (value->type) {
  /* Do nothing special for numbers */
  case NUMBER:
    break;
  /* For Functions we must free their name */
  case FUNCTION:
    free(value->function.name);
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

/* Read the type of a value */
char *get_type(Value *value) {
  switch (value->type) {
  case NUMBER:
    return "Number";
  case SYMBOL:
    return "Symbol";
  case FUNCTION:
    return "Function";
  case SEXPR:
    return "S-Expression";
  case QEXPR:
    return "Q-Expression (List)";
  case ERROR:
    return "Error";
  }
  return "Unreachable value placed here to please the deities of compilation.";
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
  case NUMBER: {
    double rounded = roundf(value->number);
    if (rounded == value->number) {
      printf("%li", (long)rounded);
    } else {
      printf("%g", value->number);
    }
    break;
  }
  case SYMBOL:
    printf("%s", value->symbol);
    break;
  case FUNCTION:
    printf("%s", value->function.name);
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

void number_to_string(Value *value, char *result) {
  // Must be called with a Number value, or everything crashes
  if (!IS_NUMBER(value)) {
    exit(EX_SOFTWARE);
  }

  double rounded = roundf(value->number);
  if (rounded == value->number) {
    int size = snprintf(NULL, 0, "%li", (long)rounded);
    result = realloc(result, size);
    sprintf(result, "%li", (long)rounded);
  } else {
    int size = snprintf(NULL, 0, "%g", value->number);
    result = realloc(result, size);
    sprintf(result, "%g", value->number);
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

inline Value *pop(Value *value) { return pop_value(value, 0); }

Value *take_value(Value *value, int index) {
  Value *popped = pop_value(value, index);
  delete_value(value);
  return popped;
}

Value *append_value(Value *list, Value *new_value) {
  list->sexpr.count++;
  list->sexpr.cell = realloc(list->sexpr.cell, sizeof(Value *) * count(list));

  list->sexpr.cell[count(list) - 1] = new_value;
  return list;
}

Value *join_values(Value *left, Value *right) {
  /* For each cell in `right` add it to `left` */
  while (count(right)) {
    left = append_value(left, pop(right));
  }

  /* Delete the empty `right` and return `left` */
  delete_value(right);
  return left;
}

Value *copy_value(Value *value) {
  Value *copy = malloc(sizeof(Value));
  copy->type = value->type;

  switch (value->type) {
  /* Copy Functions and Numbers directly */
  case NUMBER:
    copy->number = value->number;
    break;
  /* We copy the name and the pointer of the function */
  case FUNCTION:
    copy->function.name = malloc(strlen(value->function.name) + 1);
    strcpy(copy->function.name, value->function.name);
    copy->function.body = value->function.body;
    break;
  /* Copy strings using malloc and strcpy */
  case SYMBOL:
    copy->symbol = malloc(strlen(value->symbol) + 1);
    strcpy(copy->symbol, value->symbol);
    break;
  case ERROR:
    copy->error = malloc(strlen(value->error) + 1);
    strcpy(copy->error, value->error);
    break;
  /* Copy lists by copying each sub-expression */
  case SEXPR:
  case QEXPR:
    copy->sexpr.count = value->sexpr.count;
    copy->sexpr.cell = malloc(sizeof(Value *) * value->sexpr.count);
    for (int index = 0; index < copy->sexpr.count; index++) {
      copy->sexpr.cell[index] = copy_value(value->sexpr.cell[index]);
    }
    break;
  }

  return copy;
}
