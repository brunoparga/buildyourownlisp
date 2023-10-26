#include <err.h>
#include <string.h>

#include "../utils/file.c"

#define GREEN(text) "\e[1;32m" text "\e[0m"
#define RED(text) "\e[1;31m" text "\e[0m"

static inline int is_test(char *path) {
  char *extension = strrchr(path, '.');
  return !strcmp(extension, ".lye");
}

static char *build_command(char *source, char *result) {
  char *lye = "./build/lye";
  char concat[strlen(lye) + strlen(source) + 6];
  memset(concat, 0, strlen(concat));
  strcat(concat, lye);
  strcat(concat, " -s \"");
  strcat(concat, source);
  strcat(concat, "\"");
  strcpy(result, concat);
  return result;
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

static void run_test(char *test_filename, char *result_file, int *successes,
                     int *failures) {
  // Call the Lye executable on a given test file
  char command[100];
  FILE *test_file = fopen(test_filename, "r");

  if (test_file == NULL) {
    err(1, "Error opening test file %s\n", test_filename);
  }

  char *line = NULL;
  size_t length = 0;

  while ((getline(&line, &length, test_file)) != -1) {
    if (strcmp(line, "\n") == 0) { // Empty lines in the test file
      continue;
    }
    // Read the test from the test line
    size_t semicolon_position = strcspn(line, ";");
    char test[semicolon_position];
    memcpy(test, line, semicolon_position);
    test[semicolon_position] = '\0';

    // Run the test if the line is not just a comment
    if (semicolon_position > 0) {
      freopen(result_file, "w+", stdout);

      // Get the expected result
      char *expected = get_expected(line);

      // Run the test, leaving the result in the temporary file
      char *foo = build_command(test, command);
      system(foo);
      memset(command, 0, strlen(command));

      // Read the actual result from the temporary file
      char *result = get_result(result_file);

      // Increment the success or failure count and print failure
      if (strcmp(result, expected) == 0) {
        *successes += 1;
      } else {
        *failures += 1;
        freopen("/dev/tty", "w", stdout);
        printf("(%s) " RED("Fail") ": expected %s but got %s.\n",
               test_filename + 7, expected, result);
      }
    }
  }

  freopen("/dev/tty", "w", stdout);
  // Clean up
  fclose(test_file);
  if (line) {
    free(line);
  }
}

int main(void) {
  char *result_file = "./test/test.tmp";
  FTS *test_dir = open_dir("./test");
  FTSENT *subdir;

  int successes;
  int *succ_ptr = &successes;
  *succ_ptr = 0;

  int failures;
  int *fail_ptr = &failures;
  *fail_ptr = 0;

  while ((subdir = fts_read(test_dir)) != NULL) {
    if (subdir->fts_info == FTS_F && is_test(subdir->fts_path)) {
      run_test(subdir->fts_path, result_file, succ_ptr, fail_ptr);
    }
  }

  int total = successes + failures;
  if (failures == 0) {
    printf("Ran " GREEN("%d") " tests, all passed. Hooray!\n",
           total);
  } else {
    printf("Ran %d tests, " GREEN("%d") " passed, " RED("%d") " failed.\n",
           total, successes, failures);
  }

  remove(result_file);
  return 0;
}
