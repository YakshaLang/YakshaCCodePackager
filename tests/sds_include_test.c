#define YK__SDS_IMPLEMENTATION
#include "yk__sds.h"
#include <stdio.h>

int main(void) {
  yk__sds my_str = yk__sdsnew("hello");
  puts(yk__sdscatfmt(my_str, "%s", " world"));
}