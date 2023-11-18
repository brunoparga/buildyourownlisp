#include "expressionizer.h"

static Value *read_number(mpc_ast_t *ast) {
  errno = 0;
  double number = strtod(ast->contents, NULL);
  return errno == ERANGE ? make_error("number outside of valid bounds.")
                         : make_number(number);
}

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

  /* Fill the list with any valid expression contained within */
  for (int index = 0; index < ast->children_num; index++) {
    if (strcmp(ast->children[index]->tag, "regex") == 0 ||
        strcmp(ast->children[index]->tag, "comment|>") == 0 ||
        strcmp(ast->children[index]->contents, "(") == 0 ||
        strcmp(ast->children[index]->contents, ")") == 0 ||
        strcmp(ast->children[index]->contents, "{") == 0 ||
        strcmp(ast->children[index]->contents, "}") == 0) {
      continue;
    }
    value = append_value(value, expressionize(ast->children[index]));
  }
#undef HAS_TAG

  return value;
}
