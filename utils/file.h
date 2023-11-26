/*
 * utils/file.h
 *
 * Expose a utility function to read a file into a string.
 *
 */
#ifndef utils_file_h
#define utils_file_h

#include <stdio.h>
#include <stdlib.h>

char *read_file(const char *path);

#endif
