/*
 * test/test.c
 *
 * Language test runner; runs the Lye files contained in subdirectories.
 *
 */
#define _GNU_SOURCE

#include <err.h>
#include <fts.h>
#include <stdio.h>
#include <string.h>

#include "../utils/file.c"

#define GREEN(text) "\e[1;32m" text "\e[0m"
#define RED(text) "\e[1;31m" text "\e[0m"

int successes = 0;
int failures = 0;

static inline int is_test(char *path) {
  char *extension = strrchr(path, '.');
  return !strcmp(extension, ".lye");
}

static void build_command(char *source, char *result) {
  char *lye = "./lye";
  int command_length = strlen(lye) + strlen(source) + 7;
  char concat[command_length];
  memset(concat, 0, command_length);
  strcat(concat, lye);
  strcat(concat, " -s \"");
  strcat(concat, source);
  strcat(concat, "\"");
  strcpy(result, concat);
}

static char *get_expected(char *line) {
  // Find the comment
  char *comment = strchr(line, ';');
  // Advance to the first space after the semicolon
  comment = strchr(comment, ' ');
  comment++;
  // Advance to the space after the word "expect"
  comment = strchr(comment, ' ');
  // Trim the newline
  comment[strcspn(comment, "\n")] = '\0';
  // Return the actual expected value
  return ++comment;
}

static inline char *get_result(char *tmp_file) {
  char *result = read_file(tmp_file);
  result[strcspn(result, "\n")] = '\0';
  return result;
}

static void run_test(char *line, int line_number, char *test_filename,
                     char *result_filename) {
  // Read the test from the test line
  size_t semicolon_position = strcspn(line, ";");

  // Run the test if the line is not just a comment
  if (semicolon_position > 0) {
    // Get the expected result
    char *expected = get_expected(line);

    // Get the actual test
    line[semicolon_position] = '\0';

    // Write to a file that we can read
    freopen(result_filename, "w+", stdout);

    // Run the test, leaving the result in the temporary file
    char command[100];
    build_command(line, command);
    system(command);

    // Reset the Lye command for the next test run
    command[0] = '\0';

    // Read the actual result from the temporary file
    char *result = get_result(result_filename);

    // Increment the success or failure count and print failure
    if (strcmp(result, expected) == 0) {
      successes += 1;
    } else {
      failures += 1;
      freopen("/dev/tty", "w", stdout);
      printf("(%s:%d) " RED("Fail") ": expected %s but got %s.\n",
             test_filename + 7, line_number, expected, result);
    }

    free(result);
  }
}

static void run_test_file(char *test_filename, char *result_filename) {
  // Call the Lye executable on a given test file
  FILE *test_file = fopen(test_filename, "r");

  if (test_file == NULL) {
    err(1, "Error opening test file %s\n", test_filename);
  }

  char *line = NULL;
  size_t length = 0;
  int line_number = 0;

  while ((getline(&line, &length, test_file)) != -1) {
    line_number++;
    if (strcmp(line, "\n") == 0) { // Empty lines in the test file
      continue;
    }

    run_test(line, line_number, test_filename, result_filename);
  }

  // Clean up
  fclose(test_file);
  if (line) {
    free(line);
  }
}

int main(int argc, char **argv) {
  char *result_filename = "./test/test.tmp";

  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      run_test_file(argv[i], result_filename);
    }
  } else {
    FTS *test_dir = open_dir("./test");
    FTSENT *subdir;

    while ((subdir = fts_read(test_dir)) != NULL) {
      if (subdir->fts_info == FTS_F && is_test(subdir->fts_path)) {
        run_test_file(subdir->fts_path, result_filename);
      }
    }

    fts_close(test_dir);
  }

  int total = successes + failures;
  freopen("/dev/tty", "w", stdout);
  if (failures == 0) {
    printf("Ran " GREEN("%d") " tests, all passed. Hooray!\n", total);
  } else {
    printf("Ran %d tests, " GREEN("%d") " passed, " RED("%d") " failed.\n",
           total, successes, failures);
  }

  remove(result_filename);
  return 0;
}
