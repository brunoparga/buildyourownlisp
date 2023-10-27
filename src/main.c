#include "../utils/file.h"
#include "repl.h"

static void run_file(Parser *parser, char *filename) {
  char *source = read_file(filename);
  run_string(parser, source);
  free(source);
}

int main(int argc, char **argv) {
  Parser *parser = create_parser();
  if (argc == 1) {
    puts("Lye Version 0.0.0.4");
    puts("Press Ctrl+c to Exit\n");
    repl(parser);
  } else if (argc == 2) {
    run_file(parser, argv[1]);
  } else if (argc == 3 && strcmp(argv[1], "-s") == 0) {
    run_string(parser, argv[2]);
  }
  // This might be a memory leak - the parser never really gets cleaned
  // up because the program exits before that. I'll have to trap the
  // signal from Ctrl+C to deal with that?
  cleanup_parser(parser);
  return 0;
}
