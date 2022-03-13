#include "yk__lib.h"
struct a {
  uint32_t x;
  uint32_t y;
};
yk__sds readfile(yk__sds filename) {
  size_t ln;
  int err;
  char *out = yk__bhalib_read_file(filename, &ln, &err);
  yk__sdsfree(filename);// clean up filename as it will be copied
  if (err != 0) { return yk__sdsnewlen(out, ln); }
  return yk__sdsempty();
}
int main(void) { return 0; }