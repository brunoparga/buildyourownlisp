#define _GNU_SOURCE

#include <err.h>
#include <fts.h>

#include "file.h"

FTS *open_dir(char *dir) {
  FTS *test_dir;
  char *paths[2];
  paths[0] = dir;
  paths[1] = NULL;

  if ((test_dir = fts_open(paths, FTS_NOCHDIR, NULL)) == NULL) {
    err(1, "Failed to open test directory.");
  }

  FTSENT *children = fts_children(test_dir, 0);
  if (children == NULL) {
    err(1, "No test directories found");
  }

  return test_dir;
}

/* Given a filename, return its contents as a string (hopefully) */
char *read_file(const char *path) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    fprintf(stderr, "Could not open file \"%s\".\n", path);
    return NULL;
  }

  int seeking_failed = fseek(file, 0L, SEEK_END);
  if (seeking_failed) {
    fprintf(stderr, "Could not traverse file \"%s\".\n", path);
    return NULL;
  }

  long size_try = ftell(file);
  if (size_try == -1L) {
    fprintf(stderr, "Could not determine size of file \"%s\".\n", path);
    return NULL;
  }
  rewind(file);
  size_t file_size = (size_t)size_try;

  char *buffer = malloc(file_size + 1);
  if (buffer == NULL) {
    fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
    return NULL;
  }

  size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
  if (bytes_read < file_size) {
    fprintf(stderr, "Could not read file \"%s\".\n", path);
    return NULL;
  }

  buffer[bytes_read] = '\0';

  fclose(file);
  return buffer;
}
