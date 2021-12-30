#define YK__SDS_IMPLEMENTATION
#include "yk__sds.h"
#include <stdio.h>

int main(void) {
  yk__sds result = yk__sdscatfmt(yk__sdsempty(), "hello%s", " world");
  puts(result);
  yk__sdsfree(result);
  return EXIT_SUCCESS;
}