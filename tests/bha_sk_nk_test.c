//#define YK__BHASKNK_WIDTH 400
//#define YK__BHASKNK_HEIGHT 400
//#define YK__BHASKNK_TITLE "square"
#define YK__BHASKNK_IMPLEMENTATION
#include "yk__bhasknk.h"
static int yk__bhasknk_ui_loop(struct yk__nk_context *ctx) {
  (void) ctx;
  return 0;
}
static yk__bhasknk_settings yk__bhasknk_init(int argc, char *argv[]) {
  (void) argc;
  (void) argv;
  return (yk__bhasknk_settings){.width = 1024, .height = 768, .title = "hello"};
}