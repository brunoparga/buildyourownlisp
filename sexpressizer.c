#include <stdlib.h>
#include "sexpressizer.h"
#include "value.h"

static Value *append_value(Value *list, Value *new_value) {
  list->sexpr.count++;
  list->sexpr.cell = realloc(list->sexpr.cell, sizeof(Value*) * list->sexpr.count);
  list->sexpr.cell[list->sexpr.count - 1] = new_value;
  return list;
}

static Value *read_number(mpc_ast_t *t) {
  errno = 0;
  double number = strtod(t->contents, NULL);
  return errno == ERANGE ? make_error("Error: number outside of valid bounds.")
                         : make_number(number);
}

Value *sexpressize(mpc_ast_t *t) {
  /* If Symbol or Number return conversion to that type */
  if (strstr(t->tag, "number")) {
    return read_number(t);
  }
  if (strstr(t->tag, "symbol")) {
    return make_symbol(t->contents);
  }

  /* If root (">") or sexpr then create empty list */
  Value *value = NULL;
  if (strcmp(t->tag, ">") == 0 || strstr(t->tag, "sexpr")) {
    value = make_sexpr();
  }

  /* Fill the list with any valid expression contained within */
  for (int i = 0; i < t->children_num; i++) {
    if (strcmp(t->children[i]->contents, "(") == 0 ||
        strcmp(t->children[i]->contents, ")") == 0 ||
        strcmp(t->children[i]->tag, "regex") == 0) {
      continue;
    }
    value = append_value(value, sexpressize(t->children[i]));
  }

  return value;
}
