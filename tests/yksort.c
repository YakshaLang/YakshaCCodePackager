#include <stdio.h>
#define YK__STB_DS_IMPLEMENTATION
#define YK__STBDS_UNIT_TESTS
#define YK__SDS_IMPLEMENTATION
#define YK__BHALIB_IMPLEMENTATION
#define YK__SORT_IMPLEMENTATION
#include "yk__lib.h"

int compare_int(const void* a, const void* b) {
  return *((const int*)a) - *((const int*)b);
}
int main() {
  int x[] = {4, 3, 2, 6, 1, 5, 10, 20, 60, 30, 40, 50};
  int elements = sizeof(x)/ sizeof(int);
  printf("Success = %s\n", yk__quicksort((void*)x, sizeof(int), elements, compare_int) == 0 ? "True" : "False");
  for (int i = 0; i < elements; i++) {
    printf("%d\n", x[i]);
  }
}