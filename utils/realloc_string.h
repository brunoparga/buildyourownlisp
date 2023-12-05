/*
 * utils/file.h
 *
 * Encapsulate the behavior of receiving a pointer and a C value and converting
 * it to a string of the appropriate length, stored in the pointer. Return the
 * pointer.
 *
 */
#ifndef utils_realloc_string_h
#define utils_realloc_string_h

#define REALLOC_STRING(format, value, pointer)                                 \
  do {                                                                         \
    int size = snprintf(NULL, 0, format, value);                               \
    pointer = realloc(pointer, (size_t)size + 1);                              \
    sprintf(pointer, format, value);                                           \
  } while (0)

#endif