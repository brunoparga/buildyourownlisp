#ifndef lispy_parser_h
#define lispy_parser_h

#include "lib/mpc.h"

#define GRAMMAR_FILE "/home/bruno/code/learn/buildyourownlisp/grammar.txt"

typedef struct Parser {
  char* language;
  mpc_parser_t *Number;
  mpc_parser_t *Operator;
  mpc_parser_t *Expr;
  mpc_parser_t *Lispy;
} Parser;

Parser* create_parser();
void parse(Parser* parser, char* input);
void cleanup_parser(Parser* parser);

#endif
