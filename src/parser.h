#ifndef lye_parser_h
#define lye_parser_h

#include "../lib/mpc.h"

#include "eval.h"
#include "expressionizer.h"
#include "value.h"

#include "../utils/file.h"

#define GRAMMAR_FILE "./grammar.txt"

typedef struct Parser {
  mpc_parser_t *Number;
  mpc_parser_t *Symbol;
  mpc_parser_t *Sexpr;
  mpc_parser_t *Qexpr;
  mpc_parser_t *Expr;
  mpc_parser_t *Comment;
  mpc_parser_t *Lye;
} Parser;

Parser *create_parser();
Value *parse(Env *env, Parser *parser, char *input);
void cleanup_parser(Parser *parser);

#endif
