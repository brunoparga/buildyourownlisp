#include "parser.h"

/*
 * src/parser.c:create_parser
 * buildyourownlisp.com correspondence: main
 *
 * Create an MPC parser. It has one field per grammar rule.
 *
 */
Parser *create_parser() {
  Parser *parser = malloc(sizeof(Parser));
  parser->Number = mpc_new("number");
  parser->Symbol = mpc_new("symbol");
  parser->Sexpr = mpc_new("sexpr");
  parser->Qexpr = mpc_new("qexpr");
  parser->Expr = mpc_new("expr");
  parser->Comment = mpc_new("comment");
  parser->Lye = mpc_new("lye");

  return parser;
}

/*
 * src/parser.c:read_number
 * buildyourownlisp.com correspondence: lval_read_num
 *
 * Convert a number string in the source code to a Lye Value. We check that
 * the number can actually be represented as a C double, which is the internal
 * representation of Lye numbers.
 *
 */
static Value *read_number(mpc_ast_t *ast) {
  errno = 0;
  double number = strtod(ast->contents, NULL);
  return errno == ERANGE ? make_error("number outside of valid bounds.")
                         : make_number(number);
}

// Check if the parser should skip the current AST (e.g. parentheses)
static inline bool skip(mpc_ast_t *ast, size_t index) {
  return strcmp(ast->children[index]->tag, "regex") == 0 ||
         strcmp(ast->children[index]->tag, "comment|>") == 0 ||
         strcmp(ast->children[index]->contents, "(") == 0 ||
         strcmp(ast->children[index]->contents, ")") == 0 ||
         strcmp(ast->children[index]->contents, "{") == 0 ||
         strcmp(ast->children[index]->contents, "}") == 0;
}

/*
 * src/parser.c:expressionize
 * buildyourownlisp.com correspondence: lval_read
 *
 * Recursively convert an Abstract Syntax Tree into a Lye Value. An
 * S-Expression will always be at the root.
 *
 */
static Value *expressionize(mpc_ast_t *ast) {
#define HAS_TAG(some_tag) (strstr(ast->tag, some_tag))
  /* If Symbol or Number return Value of that type */
  if (HAS_TAG("number")) {
    return read_number(ast);
  }
  if (HAS_TAG("symbol")) {
    return make_symbol(ast->contents);
  }

  /* If root (">"), sexpr or qexpr then create empty list */
  Value *value = NULL;
  if (strcmp(ast->tag, ">") == 0 || HAS_TAG("sexpr")) {
    value = make_sexpr();
  } else if (HAS_TAG("qexpr")) {
    value = make_qexpr();
  }

  /* Fill the list with any valid expressions contained within */
  for (size_t index = 0; index < (size_t)ast->children_num; index++) {
    if (skip(ast, index)) {
      continue;
    }
    value = append_value(value, expressionize(ast->children[index]));
  }
#undef HAS_TAG

  return value;
}

/*
 * src/parser.c:parse
 * buildyourownlisp.com correspondence: main
 *
 * Parse an expression according to the rules of Lye, returning its Value
 *
 */
Value *parse(Env *env, Parser *parser, char *input) {
  /* Define parsers for our Language */
  mpca_lang_contents(MPCA_LANG_DEFAULT, GRAMMAR_FILE, parser->Number,
                     parser->Symbol, parser->Sexpr, parser->Qexpr, parser->Expr,
                     parser->Comment, parser->Lye);

  /* Attempt to parse the user input */
  mpc_result_t result;
  Value *value;

  if (mpc_parse("<stdin>", input, parser->Lye, &result)) {
    /* On success return the result */
    value = evaluate(env, expressionize(result.output));
    mpc_ast_delete(result.output);
  } else {
    /* Otherwise return the error */
    ErrorMsg error = mpc_err_string(result.error);
    value = make_error(error);
    mpc_err_delete(result.error);
  }

  return value;
}

/*
 * src/parser.c:cleanup_parser
 * buildyourownlisp.com correspondence: main
 *
 * Free the memory used by the Parser
 *
 */
void cleanup_parser(Parser *parser) {
  /* Undefine and Delete our Parsers */
  mpc_cleanup(7, parser->Number, parser->Symbol, parser->Sexpr, parser->Qexpr,
              parser->Expr, parser->Comment, parser->Lye);
  free(parser);
}
