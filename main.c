#include "repl.h"

int main(void) {
  puts("Lispy Version 0.0.0.4");
  puts("Press Ctrl+c to Exit\n");

  Parser *parser = create_parser();
  repl(parser);
  cleanup_parser(parser);
  return 0;
}
