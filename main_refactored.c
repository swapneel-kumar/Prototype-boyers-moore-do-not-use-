#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum Errors {
  INVALID_KEY = -1000,
  INVALID_LENGTH_KEY = -1001,
  TOO_FEW_ARGUMENTS = -1002,
  WORD_NOT_FOUND_IN_BUFFER = -1003,
};

int err_handler(long error_code);
int key_validator(char key[], size_t size_of_key);
int file_open(void);
long file_reader(int fd, char buffer[]);
size_t *jump_table_create(size_t arr[], size_t key_length,
                          char key[key_length]);

size_t horsepool(char *buffer, long buf_elements, size_t buf_size,
                 size_t *jump_table, char *key, size_t key_length);

int main(int argc, char *key[]) {

  size_t arr[26];

  if (argc < 2) {
    err_handler(TOO_FEW_ARGUMENTS);
    return EXIT_FAILURE;
  }

  size_t size_of_key = strlen(key[1]);

  long exit_code = 0;

  exit_code = key_validator(key[1], size_of_key);

  if (exit_code == INVALID_KEY) {
    err_handler(INVALID_KEY);
    return EXIT_FAILURE;
  }
  if (exit_code == INVALID_LENGTH_KEY) {
    err_handler(INVALID_LENGTH_KEY);
    return EXIT_FAILURE;
  }

  int fd = file_open();

  if (fd < 0) {
    err_handler(fd);
    return EXIT_FAILURE;
  }

  char *buffer = calloc(4096, 1);

  if (buffer == NULL) {
    err_handler(-errno);
    close(fd);
    return EXIT_FAILURE;
  }

  long buf_size = file_reader(fd, buffer);
  if (buf_size < 0) {
    err_handler(buf_size);
    goto free_memory;
  }

  size_t *jump_table = jump_table_create(arr, size_of_key, key[1]);

  exit_code =
      horsepool(buffer, buf_size, 4096, jump_table, key[1], size_of_key);

  if (exit_code == WORD_NOT_FOUND_IN_BUFFER) {

    err_handler(WORD_NOT_FOUND_IN_BUFFER);
    goto free_memory;
  } else {
    write(1, "The string was succesfully found\n", 33);
  }

  free(buffer);
  close(fd);

  return EXIT_SUCCESS;

free_memory:

  free(buffer);
  buffer = NULL;
  close(fd);
  return EXIT_FAILURE;
}

int key_validator(char key[], size_t size_of_key) {

  if (size_of_key == 0)
    return INVALID_LENGTH_KEY;

  for (size_t i = 0; i < size_of_key; i++) {
    if (key[i] > 'z' || key[i] < 'a')
      return INVALID_KEY;
  }

  return 0;
}

int file_open(void) {

  int fd = open("hello.txt", O_RDONLY);

  if (fd < 0) {
    int err = -errno;

    return err;
  }

  return fd;
}

long file_reader(int fd, char buffer[]) {

  long exit_code = read(fd, buffer, 4096);

  if (exit_code < 0) {
    int err = -errno;
    return err;
  }

  return exit_code;
}

size_t *jump_table_create(size_t arr[], size_t key_length,
                          char key[key_length]) {

  for (int i = 0; i < 26; i++) {
    arr[i] = key_length;
  }

  for (size_t i = 0; i < key_length - 1; i++) {
    arr[key[i] - 'a'] = key_length - i - 1;
  }

  return arr;
}

size_t horsepool(char *buffer, long buf_elements, size_t buf_size,
                 size_t *jump_table, char *key, size_t key_length) {

  size_t i = 0;
  size_t count = 0;
  size_t pattern_end = i + key_length - 1;
  size_t key_index = key_length - 1;

  while (i <= (size_t)buf_elements - pattern_end - 1) {

    while (buffer[pattern_end] == key[key_index]) {

      pattern_end--;
      key_index--;
      count++;
      if (count == key_length) {
        return i;
      }
    }
    i = i + jump_table[buffer[pattern_end] - 'a'];
  }

  return WORD_NOT_FOUND_IN_BUFFER;
}

int err_handler(long error_code) {

  char *error_reason = "";

  switch (error_code) {
  case INVALID_KEY:
    error_reason =
        "The key must consist of lower case alphabets from ASCII only";

    break;

  case INVALID_LENGTH_KEY:
    error_reason = "The key is empty";
    break;

  case TOO_FEW_ARGUMENTS:
    error_reason = "Too few arguments";
    break;

  case WORD_NOT_FOUND_IN_BUFFER:
    error_reason = "The string was not found";
    break;
  default:
    error_reason = strerror(-(int)error_code);
  }

  size_t error_reason_string_length = strlen(error_reason);

  write(2, error_reason, error_reason_string_length);
  write(2, "\n", 1);
  return 0;
}