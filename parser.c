#define _GNU_SOURCE

#include "parser.h"
#include "eval.h"
#include "sexpressizer.h"
#include "value.h"

static char *read_grammar(const char *path) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    fprintf(stderr, "Could not open file \"%s\".\n", path);
    return NULL;
  }

  fseek(file, 0L, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);

  char *buffer = (char *)malloc(fileSize + 1);
  if (buffer == NULL) {
    fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
    return NULL;
  }

  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
  if (bytesRead < fileSize) {
    fprintf(stderr, "Could not read file \"%s\".\n", path);
    return NULL;
  }

  buffer[bytesRead] = '\0';

  fclose(file);
  return buffer;
}

Parser *create_parser() {
  Parser *parser = malloc(sizeof(Parser));
  parser->language = read_grammar(GRAMMAR_FILE);

  if (parser->language == NULL) {
    printf("could not read grammar file.");
    return NULL;
  }

  parser->Number = mpc_new("number");
  parser->Symbol = mpc_new("symbol");
  parser->Sexpr = mpc_new("sexpr");
  parser->Expr = mpc_new("expr");
  parser->Lispy = mpc_new("lispy");

  return parser;
}

void parse(Parser *parser, char *input) {
  /* Define parsers with the following Language */
  mpca_lang(MPCA_LANG_DEFAULT, parser->language, parser->Number, parser->Symbol, parser->Sexpr,
            parser->Expr, parser->Lispy);

  /* Attempt to parse the user input */
  mpc_result_t result;
  if (mpc_parse("<stdin>", input, parser->Lispy, &result)) {
    /* On success print the result */
    Value *value = sexpressize(result.output);
    // Value value = eval(result.output);
    print_value(value);
    free_value(value);
    mpc_ast_delete(result.output);
  } else {
    /* Otherwise print the error */
    mpc_err_print(result.error);
    mpc_err_delete(result.error);
  }
}

void cleanup_parser(Parser *parser) {
  /* Undefine and Delete our Parsers */
  mpc_cleanup(5, parser->Number, parser->Symbol, parser->Sexpr, parser->Expr, parser->Lispy);
  free(parser);
}