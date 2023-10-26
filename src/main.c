#include "../utils/file.h"
#include "repl.h"

static void run_file(Parser *parser, char *filename) {
  char *source = read_file(filename);
  Value *value = parse(parser, source);
  print_value(value);
  delete_value(value);
}

int main(int argc, char **argv) {
  Parser *parser = create_parser();
  if (argc == 1) {
    puts("Lye Version 0.0.0.4");
    puts("Press Ctrl+c to Exit\n");
    repl(parser);
  } else {
    run_file(parser, argv[1]);
  }
  // This might be a memory leak - the parser never really gets cleaned
  // up because the program exits before that. I'll have to trap the
  // signal from Ctrl+C to deal with that?
  cleanup_parser(parser);
  return 0;
}
