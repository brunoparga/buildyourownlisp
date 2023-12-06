#include "value.h"

// Included here and not in header file to avoid circular dependency
#include "env.h"

// ============================
// Constructors and destructors
// ============================

/*
 * src/value.c:make_number
 * buildyourownlisp.com correspondence: lval_num
 *
 * Create a new number Value from the provided double.
 *
 */
Value *make_number(double number) {
  Value *value = malloc(sizeof(Value));
  value->type = NUMBER;
  value->data.number = number;
  return value;
}

/*
 * src/value.c:make_symbol
 * buildyourownlisp.com correspondence: lval_sym
 *
 * Create a new symbol Value from the provided string.
 *
 */
Value *make_symbol(Symbol symbol) {
  Value *value = malloc(sizeof(Value));
  value->type = SYMBOL;
  value->data.symbol = malloc(strlen(symbol) + 1);
  strcpy(value->data.symbol, symbol);
  return value;
}

/*
 * src/value.c:make_builtin
 * buildyourownlisp.com correspondence: lval_fun
 *
 * Create a new function Value from the provided name and builtin.
 *
 */
Value *make_builtin(Symbol name, Builtin builtin) {
  Value *value = malloc(sizeof(Value));
  value->type = FUNCTION;

  Function *function = malloc(sizeof(Function));
  function->name = malloc(strlen(name) + 1);
  strcpy(function->name, name);
  function->builtin = builtin;

  value->data.function = function;
  return value;
}

/*
 * src/value.c:make_lambda
 * buildyourownlisp.com correspondence: lval_lambda
 *
 * Create a new user-defined function Value. It doesn't have a name; it is
 * called by the symbol used to `def` it.
 *
 */
Value *make_lambda(Value *params, Value *body) {
  Value *value = malloc(sizeof(Value));
  value->type = FUNCTION;

  Function *function = malloc(sizeof(Function));
  function->name = NULL;
  function->builtin = NULL;
  function->env = make_env();
  function->params = params;
  function->body = body;

  value->data.function = function;
  return value;
}

/*
 * src/value.c:make_sexpr
 * buildyourownlisp.com correspondence: lval_sexpr
 *
 * Create a new, empty S-expression Value.
 *
 */
Value *make_sexpr() {
  Value *value = malloc(sizeof(Value));
  value->type = SEXPR;
  value->data.sexpr.count = 0;
  value->data.sexpr.cell = NULL;
  return value;
}

/*
 * src/value.c:make_qexpr
 * buildyourownlisp.com correspondence: lval_expr
 *
 * Create a new, empty Q-expression Value.
 *
 */
Value *make_qexpr() {
  Value *value = malloc(sizeof(Value));
  value->type = QEXPR;
  value->data.sexpr.count = 0;
  value->data.sexpr.cell = NULL;
  return value;
}

/*
 * src/value.c:make_error
 * buildyourownlisp.com correspondence: lval_err
 *
 * Create a new error Value. This function is variadic, taking a printf-like
 * format and any number of arguments to fill in the format.
 *
 */
Value *make_error(char *format, ...) {
  Value *value = malloc(sizeof(Value));
  value->type = ERROR;

  /* Create and initialize a list of arguments */
  va_list pieces;
  va_start(pieces, format);

  /* Allocate 512 bytes of space */
  value->data.error = malloc(512);

  /* printf the error string with a maximum of 511 characters */
  vsnprintf(value->data.error, 511, format, pieces);

  /* Reallocate to the number of bytes actually used */
  value->data.error = realloc(value->data.error, strlen(value->data.error) + 1);

  /* Clean up our arg list */
  va_end(pieces);

  return value;
}

/*
 * src/value.c:va_list_make_error
 * buildyourownlisp.com correspondence: none
 *
 * Create a new error Value. This is the non-variadic version of the function;
 * it may be called by variadic functions that group the extra arguments
 * (beside `format`) into a `va_list` structure.
 *
 */
Value *va_list_make_error(char *format, va_list pieces) {
  char *error = malloc(512);
  vsprintf(error, format, pieces);
  Value *value = make_error(error);
  free(error);
  return value;
}

/*
 * src/value.c:delete_value
 * buildyourownlisp.com correspondence: lval_del
 *
 * Release the memory taken up by a Value, including its nested Values.
 *
 */
void delete_value(Value *value) {
  switch (value->type) {
  /* Do nothing special for numbers */
  case NUMBER:
    break;
  /* What to free is different for builtins and user-defined functions */
  case FUNCTION:
    if (value->data.function->builtin) {
      free(value->data.function->name);
    } else {
      free(value->data.function->env);
      free(value->data.function->params);
      free(value->data.function->body);
    }
    free(value->data.function);
    break;
  /* For Symbol (and ErrorMsg, below), free the string data */
  case SYMBOL:
    free(value->data.symbol);
    break;
  /* For Sexpr delete all the elements inside */
  case SEXPR:
  case QEXPR:
    for (size_t index = 0; index < count(value); index++) {
      delete_value(element_at(value, index));
    }
    /* Also free the memory allocated to contain the pointers */
    free(value->data.sexpr.cell);
    break;
  case ERROR:
    free(value->data.error);
    break;
  }

  /* Free the memory allocated for the Value itself */
  free(value);
}

// ========================
// Information about Values
// ========================

/*
 * src/value.c:count
 * buildyourownlisp.com correspondence: none
 *
 * Return the count of values in the cell of an S-expression. Unlike the
 * `length` builtin, which returns a Value, this utility function returns an
 * integer for use within the interpreter.
 *
 */
inline size_t count(Value *sexpr_value) {
  switch (sexpr_value->type) {
  case QEXPR:
  case SEXPR:
    return sexpr_value->data.sexpr.count;
  default:
    printf(
        "Error: Value is not an S- or Q-expression and doesn't have a count.");
    return 0;
  }
}

/*
 * src/value.c:get_type
 * buildyourownlisp.com correspondence: ltype_name
 *
 * Return the type of a Value as a string, to be used e.g. in error messages.
 *
 */
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

/*
 * src/value.c:stringify_number
 * buildyourownlisp.com correspondence: none
 *
 * Return the string equivalent of a number Value. Used in error reporting in
 * arithmetic operations.
 *
 */
static char *stringify_number(Value *value, char *result) {
  // Must be called with a Number value, or everything crashes
  if (!IS_NUMBER(value)) {
    exit(EX_SOFTWARE);
  }

  double rounded = round(value->data.number);
  if (rounded == value->data.number) {
    REALLOC_STRING("%li", (long)rounded, result);
  } else {
    REALLOC_STRING("%g", value->data.number, result);
  }

  return result;
}

/*
 * src/value.c:stringify_list
 * buildyourownlisp.com correspondence: lval_print_expr
 *
 * Return the string representation of a list, including its elements.
 *
 */
static char *stringify_list(Value *value, char *open, char *close) {
  char *result = malloc(strlen(open) + 1);
  strcpy(result, open);

  size_t length = count(value);
  for (size_t index = 0; index < length; index++) {
    char *element_string = stringify(element_at(value, index));
    result = realloc(result, strlen(result) + strlen(element_string) + 1);
    strcat(result, element_string);
    free(element_string);
    /* Don't print trailing space if last element */
    if (index != length - 1) {
      result = realloc(result, strlen(result) + 2);
      strcat(result, " ");
    }
  }
  result = realloc(result, strlen(result) + 2);
  strcat(result, close);

  return result;
}

/*
 * src/value.c:stringify
 * buildyourownlisp.com correspondence: none
 *
 * Return a string representation of a Value, for use in print functions.
 *
 */
char *stringify(Value *value) {
  char *result = NULL;
  switch (value->type) {
  case NUMBER:
    result = stringify_number(value, result);
    break;
  case SYMBOL:
    result = realloc(result, strlen(value->data.symbol) + 1);
    strcpy(result, value->data.symbol);
    break;
  case FUNCTION:
    if (value->data.function->builtin) {
      result = realloc(result, strlen(value->data.function->name) + 1);
      strcpy(result, value->data.function->name);
    } else {
      char *params_string = stringify(value->data.function->params);
      char *body_string = stringify(value->data.function->body);
      result = realloc(result, strlen("(\\ )") + strlen(params_string) + 1 +
                                   strlen(body_string) + 1);
      sprintf(result, "(\\ %s %s)", params_string, body_string);
      free(params_string);
      free(body_string);
    }
    break;
  case SEXPR:
    result = stringify_list(value, "(", ")");
    break;
  case QEXPR:
    result = stringify_list(value, "{", "}");
    break;
  case ERROR:
    result = realloc(result, strlen(value->data.error) + 8);
    result[0] = '\0';
    strcpy(result, "Error: ");
    strcat(result, value->data.error);
    break;
  }

  return result;
}

/*
 * src/value.c:print_value_no_newline
 * buildyourownlisp.com correspondence: lval_print
 *
 * Print a Value. Since this can be nested within a list, no newline is
 * printed at the end.
 *
 */
static void print_value_no_newline(Value *value) {
  char *result = stringify(value);
  printf("%s", result);
  free(result);
}

/*
 * src/value.c:println_value
 * buildyourownlisp.com correspondence: lval_println
 *
 * Print a complete Value, with a newline in the end.
 *
 */
void println_value(Value *value) {
  print_value_no_newline(value);
  putchar('\n');
}

// ==================
// Value manipulation
// ==================

/*
 * src/value.c:element_at
 * buildyourownlisp.com correspondence: none
 *
 * Return the Value at the given index from an S- or Q-expression. This would
 * be the equivalent of square bracket array access in C-like languages. The
 * expression remains unchanged (unlike `pop_value`, which removes the popped
 * element but maintains the rest of the list, and `take_value`, which
 * discards the list.)
 *
 */
Value *element_at(Value *sexpr_value, size_t index) {
  switch (sexpr_value->type) {
  case QEXPR:
  case SEXPR:
    return sexpr_value->data.sexpr.cell[index];
  default:
    return make_error(
        "cannot get element from a Value that is not an S- or Q-expression.");
  }
}

/*
 * src/value.c:pop_value
 * buildyourownlisp.com correspondence: lval_pop
 *
 * Remove and return the Value from the given list at the given index.
 * Compare with the more destructive `take_value`, which deletes the list, and
 * the more conservative `element_at`, which maintains the element in place.
 *
 */
Value *pop_value(Value *value, size_t index) {
  /* Find the value to pop */
  Value *popped = element_at(value, index);

  /* Shift memory after the popped item */
  memmove(&value->data.sexpr.cell[index], &value->data.sexpr.cell[index + 1],
          sizeof(Value *) * (count(value) - index - 1));

  /* Decrement the size of the list */
  value->data.sexpr.count--;

  /* Reallocate the memory used */
  value->data.sexpr.cell =
      realloc(value->data.sexpr.cell, sizeof(Value *) * count(value));
  return popped;
}

// Since the most common index to pop is 0, this DRYs up the code
inline Value *pop(Value *value) { return pop_value(value, 0); }

/*
 * src/value.c:take_value
 * buildyourownlisp.com correspondence: lval_take
 *
 * Replace an entire list by its element at the given index, destroying the
 * list. Compare with the less destructive `pop_value` and `element_at`.
 *
 */
Value *take_value(Value *value, size_t index) {
  Value *popped = pop_value(value, index);
  delete_value(value);
  return popped;
}

/*
 * src/value.c:append_value
 * buildyourownlisp.com correspondence: lval_add
 *
 * Append the given Value at the end of the given (S-/Q-Expr) list. This is
 * used internally by the interpreter, e.g. while parsing lists, as opposed to
 * `builtin_cons`, which is exposed to the user.
 *
 */
Value *append_value(Value *list, Value *new_value) {
  list->data.sexpr.count++;
  list->data.sexpr.cell =
      realloc(list->data.sexpr.cell, sizeof(Value *) * count(list));

  list->data.sexpr.cell[count(list) - 1] = new_value;
  return list;
}

/*
 * src/value.c:join_values
 * buildyourownlisp.com correspondence: lval_join
 *
 * Concatenate two lists. Used internally by the interpreter.
 *
 */
Value *join_values(Value *left, Value *right) {
  /* For each cell in `right` add it to `left` */
  while (count(right)) {
    left = append_value(left, pop(right));
  }

  /* Delete the empty `right` and return `left` */
  delete_value(right);
  return left;
}

/*
 * src/value.c:copy_value
 * buildyourownlisp.com correspondence: lval_copy
 *
 * Return a copy of the Value. This is useful in situations where we transform
 * an input Value and must delete it before returning; if the transformed
 * version were not a copy, important information from it would be lost due to
 * it sharing data with the original.
 *
 */
Value *copy_value(Value *value) {
  Value *copy = malloc(sizeof(Value));
  copy->type = value->type;

  switch (value->type) {
  /* Copy Functions and Numbers directly */
  case NUMBER:
    copy->data.number = value->data.number;
    break;
  /* We copy the name and the pointer of the function */
  case FUNCTION: {
    Function *fun_copy = malloc(sizeof(Function));

    if (value->data.function->builtin) {
      fun_copy->name = malloc(strlen(value->data.function->name) + 1);
      strcpy(fun_copy->name, value->data.function->name);
      fun_copy->builtin = value->data.function->builtin;
      fun_copy->env = NULL;
      fun_copy->params = NULL;
      fun_copy->body = NULL;
    } else {
      fun_copy->name = NULL;
      fun_copy->builtin = NULL;
      fun_copy->env = copy_env(value->data.function->env);
      fun_copy->params = copy_value(value->data.function->params);
      fun_copy->body = copy_value(value->data.function->body);
    }

    copy->data.function = fun_copy;
    break;
  }
  /* Copy strings using malloc and strcpy */
  case SYMBOL:
    copy->data.symbol = malloc(strlen(value->data.symbol) + 1);
    strcpy(copy->data.symbol, value->data.symbol);
    break;
  case ERROR:
    copy->data.error = malloc(strlen(value->data.error) + 1);
    strcpy(copy->data.error, value->data.error);
    break;
  /* Copy lists by copying each sub-expression */
  case SEXPR:
  case QEXPR:
    copy->data.sexpr.count = value->data.sexpr.count;
    copy->data.sexpr.cell = malloc(sizeof(Value *) * value->data.sexpr.count);
    for (size_t index = 0; index < copy->data.sexpr.count; index++) {
      copy->data.sexpr.cell[index] = copy_value(value->data.sexpr.cell[index]);
    }
    break;
  }

  return copy;
}
