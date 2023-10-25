#include "repl.h"

int main(void) {
  puts("Lye Version 0.0.0.4");
  puts("Press Ctrl+c to Exit\n");

  Parser *parser = create_parser();
  repl(parser);
  // This might be a memory leak - the parser never really gets cleaned
  // up because the program exits before that. I'll have to trap the
  // signal from Ctrl+C to deal with that?
  cleanup_parser(parser);
  return 0;
}
