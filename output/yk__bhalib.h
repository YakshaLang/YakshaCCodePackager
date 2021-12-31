/**
 * Bunch of utilities
 * Copyright (C) 2021-2022 Bhathiya Perera (JaDogg)
 */
#ifndef YK__BHALIB_SINGLE_HEADER
#define YK__BHALIB_SINGLE_HEADER
#define YK__BHALIB_BUF_SIZE 10 * 1024 * 1024;
#include <stdio.h>
#include <stdlib.h>
// Default to malloc if not defined
#ifndef yk__bhalib_malloc
#define yk__bhalib_malloc malloc
#endif
// Default to free if not defined
#ifndef yk__bhalib_free
#define yk__bhalib_free free
#endif
/**
 * Could not open the file
 */
#define YK__BHALIB_ERROR_NO_OPEN 1
/**
 * Could not allocate memory to read the file
 */
#define YK__BHALIB_ERROR_NO_MEM 2
/**
 * Could not read the full file
 */
#define YK__BHALIB_ERROR_NO_READ 3
/**
 * Read a file
 * @param path filepath
 * @param length pointer to receive read length
 * @param error pointer to receive error message
 *  (if there's no error this is set to zero)
 * @return buffer. (You need to call free on this)
 */
char *yk__bhalib_read_file(const char *path, size_t *length, int *error);
#ifdef YK__BHALIB_IMPLEMENTATION
char *yk__bhalib_read_file(const char *path, size_t *length, int *error) {
  // both length and error must be present
  if (length == NULL || error == NULL) { return NULL; }
  *error = 0;
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    *error = YK__BHALIB_ERROR_NO_OPEN;
    return NULL;
  }
  fseek(file, 0L, SEEK_END);
  *length = ftell(file);
  rewind(file);
  char *buffer = (char *) yk__bhalib_malloc(*length + 1);
  if (buffer == NULL) {
    fclose(file);
    *error = YK__BHALIB_ERROR_NO_MEM;
    return NULL;
  }
  size_t bytes_read = fread(buffer, sizeof(char), *length, file);
  if (bytes_read < *length) {
    fclose(file);
    yk__bhalib_free(buffer);
    *error = YK__BHALIB_ERROR_NO_READ;
    return NULL;
  }
  buffer[bytes_read] = '\0';
  fclose(file);
  return buffer;
}
#endif
#endif