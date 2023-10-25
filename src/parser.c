#include "parser.h"

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

/* Parse an expression, returning its Value */
Value *parse(Parser *parser, char *input) {
  /* Define parsers for our Language */
  mpca_lang_contents(MPCA_LANG_DEFAULT, GRAMMAR_FILE, parser->Number,
                     parser->Symbol, parser->Sexpr, parser->Qexpr, parser->Expr,
                     parser->Comment, parser->Lye);

  /* Attempt to parse the user input */
  mpc_result_t result;
  Value *value;

  if (mpc_parse("<stdin>", input, parser->Lye, &result)) {
    /* On success return the result */
    value = evaluate(expressionize(result.output));
    mpc_ast_delete(result.output);
  } else {
    /* Otherwise return the error */
    ErrorMsg error = mpc_err_string(result.error);
    value = make_error(error);
    mpc_err_delete(result.error);
  }

  return value;
}

void cleanup_parser(Parser *parser) {
  /* Undefine and Delete our Parsers */
  mpc_cleanup(7, parser->Number, parser->Symbol, parser->Sexpr, parser->Qexpr,
              parser->Expr, parser->Comment, parser->Lye);
  free(parser);
}
