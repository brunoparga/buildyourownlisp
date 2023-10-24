#include "parser.h"

Parser *create_parser() {
  Parser *parser = malloc(sizeof(Parser));
  parser->language = read_file(GRAMMAR_FILE);

  if (parser->language == NULL) {
    printf("could not read grammar file.");
    return NULL;
  }

  parser->Number = mpc_new("number");
  parser->Symbol = mpc_new("symbol");
  parser->Sexpr = mpc_new("sexpr");
  parser->Qexpr = mpc_new("qexpr");
  parser->Expr = mpc_new("expr");
  parser->Lispy = mpc_new("lispy");

  return parser;
}

/* Parse an expression, returning its Value */
Value *parse(Parser *parser, char *input) {
  /* Define parsers for our Language */
  mpca_lang(MPCA_LANG_DEFAULT, parser->language, parser->Number, parser->Symbol,
            parser->Sexpr, parser->Qexpr, parser->Expr, parser->Lispy);

  /* Attempt to parse the user input */
  mpc_result_t result;
  Value *value;

  if (mpc_parse("<stdin>", input, parser->Lispy, &result)) {
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
  mpc_cleanup(6, parser->Number, parser->Symbol, parser->Sexpr, parser->Qexpr,
              parser->Expr, parser->Lispy);
  free(parser);
}
