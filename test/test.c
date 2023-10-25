#include <err.h>
#include <fts.h>
#include <stdio.h>
#include <string.h>

#include "../utils/read_file.c"

static FTS *open_tests_dir() {
  FTS *test_dir;
  char *paths[2];
  paths[0] = "./test";
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

static inline int is_test(char *path) {
  char *extension = strrchr(path, '.');
  return !strcmp(extension, ".lye");
}

static char *build_command(char *filename, char *result) {
  char *lye = "./build/lye";
  char concat[strlen(lye) + strlen(filename) + 2];
  concat[0] = '\0';
  strcat(concat, lye);
  strcat(concat, " ");
  strcat(concat, filename);
  strcpy(result, concat);
  return result;
}

static char *get_expected(char *test) {
  // Find the comment
  char *comment = strchr(test, ';');
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

static void print_fail(char *filename, char *expected, char *result) {
  char error_msg[200];
  error_msg[0] = '\0';
  strcat(error_msg, "(");
  strcat(error_msg, filename + 7);
  strcat(error_msg, ") Fail: expected ");
  strcat(error_msg, expected);
  strcat(error_msg, " but got ");
  strcat(error_msg, result);
  strcat(error_msg, ".\n");
  printf("%s", error_msg);
}

static void run_test(char *test_file, char *tmp_file) {
  // Call the Lye executable on a given test file
  char command[100];
  freopen(tmp_file, "w+", stdout);
  system(build_command(test_file, command));

  // Read the expected result from the test file
  char *test = read_file(test_file);
  char *expected = get_expected(test);

  // Read the actual result from the temporary file
  char *result = read_file(tmp_file);
  result[strcspn(result, "\n")] = '\0';

  freopen("/dev/tty", "w", stdout);

  if (strcmp(result, expected) == 0) {
    printf("The test passed!\n");
  } else {
    print_fail(test_file, expected, result);
  }
}

int main(void) {
  char *tmp_file = "./test/test.tmp";
  FTS *test_dir = open_tests_dir();
  FTSENT *subdir;

  while ((subdir = fts_read(test_dir)) != NULL) {
    if (subdir->fts_info == FTS_F && is_test(subdir->fts_path)) {
      printf("%s\n", subdir->fts_path);
      run_test(subdir->fts_path, tmp_file);
    }
  }

  remove(tmp_file);
  return 0;
}
