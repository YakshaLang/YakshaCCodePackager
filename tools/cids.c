#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#define STB_C_LEXER_IMPLEMENTATION
#include "stb_c_lexer.h"
#define BUF_SIZE 10 * 1024 * 1024
static char *read_file(const char *path, size_t *length) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    fprintf(stderr, "Could not open file \"%s\".\n", path);
    exit(74);
  }
  fseek(file, 0L, SEEK_END);
  *length = ftell(file);
  rewind(file);
  char *buffer = (char *) malloc(*length + 1);
  if (buffer == NULL) {
    fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
    exit(74);
  }
  size_t bytesRead = fread(buffer, sizeof(char), *length, file);
  if (bytesRead < *length) {
    fprintf(stderr, "Could not read file \"%s\".\n", path);
    exit(74);
  }
  buffer[bytesRead] = '\0';
  fclose(file);
  return buffer;
}
int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Invalid arguments. Usage: ctok ctok.c");
    return EXIT_FAILURE;
  }
  size_t len;
  char *code = read_file(argv[1], &len);
  char *buffer = malloc(sizeof(char) * BUF_SIZE);
  if (buffer == NULL) {
    fprintf(stderr, "Not enough memory to create buffer.");
    return EXIT_FAILURE;
  }
  stb_lexer lexer;
  stb_c_lexer_init(&lexer, code, code + len, buffer, BUF_SIZE);
  while (stb_c_lexer_get_token(&lexer)) {
    if (lexer.token != CLEX_id) {
      continue;
    }
    printf("%s\n", lexer.string);
  }
  return EXIT_SUCCESS;
}