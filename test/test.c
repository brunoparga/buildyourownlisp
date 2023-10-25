#include <stdio.h>
#include <string.h>

#include "../utils/read_file.c"


static char* build_command(char* filename, char* result) {
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
  comment = strchr(comment,  ' ');
  comment++;
  // Advance to the space after the word "expect"
  comment = strchr(comment, ' ');
  // Trim the newline
  comment[strcspn(comment, "\n")] = '\0';
  // Return the actual expected value
  return ++comment;
}

static void print_fail(char *filename, char* expected, char* result) {
  char error_msg[200];
  error_msg[0] = '\0';
  strcat(error_msg, "(");
  strcat(error_msg, filename + 5);
  strcat(error_msg, ") Fail: expected ");
  strcat(error_msg, expected);
  strcat(error_msg, " but got ");
  strcat(error_msg, result);
  strcat(error_msg, ".\n");
  printf("%s", error_msg);
}

int main(void) {
  char *tmp_file = "test.tmp";
  freopen(tmp_file, "w+", stdout);

  // This is gonna loop over all test files
  char *filename = "test/test.lye";
  char command[100];
  system(build_command(filename, command));

  char *test = read_file(filename);
  char *expected = get_expected(test);
  char *result = read_file(tmp_file);
  // Trim the newline
  result[strcspn(result, "\n")] = '\0';

  freopen("/dev/tty", "w", stdout);

  if (strcmp(result, expected) == 0) {
    printf("The test passed!\n");
  } else {
    print_fail(filename, expected, result);
  }

  remove(tmp_file);
  return 0;
}
