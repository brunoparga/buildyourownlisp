/*
 * src/parser.h
 *
 * Define the Parser structure, which is coupled with the external dependency
 * MPC, along with functions to create, use and destroy it.
 *
 */
#ifndef lye_parser_h
#define lye_parser_h

#include "../lib/mpc.h"
#include "../utils/file.h"

#include "eval.h"
#include "value.h"

#define GRAMMAR_FILE "./grammar.txt"

/* Define the Parser struct; it should have one `mpc_parser_t` for each
rule in the grammar */
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
