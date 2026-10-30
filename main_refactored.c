#include <errno.h>
#include <fcntl.h>
#include <fenv.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>

enum Errors {
  INVALID_KEY = -1000,
  INVALID_LENGTH_KEY = -1001,
  TOO_FEW_ARGUMENTS = -1002,
  WORD_NOT_FOUND_IN_BUFFER = -1003,
  INVALID_FILE_LENGTH = -1004,
  WORD_NOT_FOUND = -1005
};

int err_handler(long error_code);
int key_validator(char key[], size_t size_of_key);
int file_open(void);
long file_reader(int fd, char buffer[], size_t space_left);
size_t *jump_table_create(size_t arr[], size_t key_length,
                          char key[key_length]);

int horsepool(char *buffer, long buf_elements, size_t buf_size,
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

  int space_left = 4096;

  size_t *jump_table = jump_table_create(arr, size_of_key, key[1]);

  long buf_size = file_reader(fd, buffer, space_left);

  if (buf_size < 0) {
    err_handler(buf_size);
    goto free_memory;
  } else if (buf_size == 0) {
    err_handler(WORD_NOT_FOUND);
    goto free_memory;

  } else if (buf_size < size_of_key) {
    err_handler(INVALID_FILE_LENGTH);
    goto free_memory;
  }

  exit_code =
      horsepool(buffer, buf_size, 4096, jump_table, key[1], size_of_key);

  if (exit_code == WORD_NOT_FOUND_IN_BUFFER) {

    err_handler(WORD_NOT_FOUND_IN_BUFFER);
    memcpy(buffer, buffer + space_left - size_of_key + 1, size_of_key - 1);
  }
  if (exit_code >= 0) {
    write(1, "The string was succesfully found\n", 33);

    free(buffer);
    buffer = NULL;
    close(fd);

    return EXIT_SUCCESS;
  }
  while (1) {
    long buf_size = file_reader(fd, buffer, space_left);

    if (buf_size < 0) {
      err_handler(buf_size);
      goto free_memory;
    } else if (buf_size == 0) {
      err_handler(WORD_NOT_FOUND);
      goto free_memory;

    } else if (buf_size < size_of_key) {
      err_handler(INVALID_FILE_LENGTH);
      goto free_memory;
    }

    exit_code =
        horsepool(buffer, buf_size, 4096, jump_table, key[1], size_of_key);

    if (exit_code == WORD_NOT_FOUND_IN_BUFFER) {

      err_handler(WORD_NOT_FOUND_IN_BUFFER);
      memcpy(buffer, buffer + space_left - size_of_key + 1, size_of_key - 1);
    }
    if (exit_code >= 0) {
      write(1, "The string was succesfully found\n", 33);

      free(buffer);
      buffer = NULL;
      close(fd);

      return EXIT_SUCCESS;
    }
  }

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

long file_reader(int fd, char buffer[], size_t space_left) {

  long exit_code = read(fd, buffer, space_left);

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

int horsepool(char *buffer, long buf_elements, size_t buf_size,
              size_t *jump_table, char *key, size_t key_length) {

  size_t i = 0;
  size_t count = 0;
  size_t pattern_end = i + key_length - 1;
  size_t key_index = key_length - 1;

  while (i <= (size_t)buf_elements - key_length) {

    pattern_end = i + key_length - 1;

    while (buffer[pattern_end] == key[key_index]) {
      // chances of underflow here
      pattern_end--;
      key_index--;
      count++;
      if (count == key_length) {
        return (int)i;
      }
    }

    if (buffer[pattern_end] - 'a' < 0 || buffer[pattern_end] - 'a' > 25) {
      i = i + key_length;
      count = 0;
      key_index = key_length - 1;
      continue;
    }
    i = i + jump_table[buffer[i + key_length - 1] - 'a'];
    count = 0;
    key_index = key_length - 1;
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
    return -2000;
  case INVALID_FILE_LENGTH:
    error_reason = "File is shorter than the key";
    break;

  case WORD_NOT_FOUND:
    error_reason = "Word was not found";
    break;
  default:
    error_reason = strerror(-(int)error_code);
  }

  size_t error_reason_string_length = strlen(error_reason);

  write(2, error_reason, error_reason_string_length);
  write(2, "\n", 1);
  return 0;
}