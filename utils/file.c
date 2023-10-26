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
