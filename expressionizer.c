#include "expressionizer.h"

static Value *append_value(Value *list, Value *new_value) {
  list->sexpr.count++;
  list->sexpr.cell = realloc(list->sexpr.cell, sizeof(Value *) * count(list));

  list->sexpr.cell[count(list) - 1] = new_value;
  return list;
}

static Value *read_number(mpc_ast_t *t) {
  errno = 0;
  double number = strtod(t->contents, NULL);
  return errno == ERANGE ? make_error("number outside of valid bounds.")
                         : make_number(number);
}

// static int str_match(char* string, int count, ...) {
//   va_list(candidates);
//   int result = 0;
//   va_start(candidates, count);
//   for (int index = 0; index < count; index++) {
//     if (strcmp(string, va_arg(candidates, char*)) == 0) {
//       result = 1;
//     }
//   }
//   va_end(candidates);
//   return result;
// }

Value *expressionize(mpc_ast_t *ast) {
#define HAS_TAG(some_tag) (strstr(ast->tag, some_tag))
  /* If Symbol or Number return conversion to that type */
  if (HAS_TAG("number")) {
    return read_number(ast);
  }
  if (HAS_TAG("symbol")) {
    return make_symbol(ast->contents);
  }

  /* If root (">") or sexpr then create empty list */
  Value *value = NULL;
  if (strcmp(ast->tag, ">") == 0 || HAS_TAG("sexpr")) {
    value = make_sexpr();
  }

  if (HAS_TAG("qexpr")) {
    value = make_qexpr();
  }
#undef HAS_TAG

  /* Fill the list with any valid expression contained within */
  for (int index = 0; index < ast->children_num; index++) {
    if (strcmp(ast->children[index]->tag, "regex") == 0 ||
        strcmp(ast->children[index]->contents, "(") == 0 ||
        strcmp(ast->children[index]->contents, ")") == 0 ||
        strcmp(ast->children[index]->contents, "{") == 0 ||
        strcmp(ast->children[index]->contents, "}") == 0) {
      continue;
    }
    value = append_value(value, expressionize(ast->children[index]));
  }

  return value;
}
