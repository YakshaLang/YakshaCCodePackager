/**
 * Bunch of utilities
 * Copyright (C) 2021-2022 Bhathiya Perera (JaDogg)
 */
#ifndef BHALIB_SINGLE_HEADER
#define BHALIB_SINGLE_HEADER
#define BHALIB_BUF_SIZE 10 * 1024 * 1024;
#include <stdio.h>
#include <stdlib.h>
// Default to malloc if not defined
#ifndef bhalib_malloc
#define bhalib_malloc malloc
#endif
// Default to free if not defined
#ifndef bhalib_free
#define bhalib_free free
#endif
/**
 * Could not open the file
 */
#define BHALIB_ERROR_NO_OPEN 1
/**
 * Could not allocate memory to read the file
 */
#define BHALIB_ERROR_NO_MEM 2
/**
 * Could not read the full file
 */
#define BHALIB_ERROR_NO_READ 3
/**
 * Read a file
 * @param path filepath
 * @param length pointer to receive read length
 * @param error pointer to receive error message
 *  (if there's no error this is set to zero)
 * @return buffer. (You need to call free on this)
 */
char *bhalib_read_file(const char *path, size_t *length, int *error);
#ifdef BHALIB_IMPLEMENTATION
char *bhalib_read_file(const char *path, size_t *length, int *error) {
  // both length and error must be present
  if (length == NULL || error == NULL) {
    return NULL;
  }
  *error = 0;
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    *error = BHALIB_ERROR_NO_OPEN;
    return NULL;
  }
  fseek(file, 0L, SEEK_END);
  *length = ftell(file);
  rewind(file);
  char *buffer = (char *) bhalib_malloc(*length + 1);
  if (buffer == NULL) {
    fclose(file);
    *error = BHALIB_ERROR_NO_MEM;
    return NULL;
  }
  size_t bytes_read = fread(buffer, sizeof(char), *length, file);
  if (bytes_read < *length) {
    fclose(file);
    bhalib_free(buffer);
    *error = BHALIB_ERROR_NO_READ;
    return NULL;
  }
  buffer[bytes_read] = '\0';
  fclose(file);
  return buffer;
}
#endif
#endif