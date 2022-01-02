/**
* Wrapper around sokol_app, sokol_gfx, sokol_glue, sokol_nuklear & nuklear
* Note this is not a single header as it needs other files in output
*  (should also work with original headers if not patched)
*
* On MSVC -> D3D11 is enabled by default
* Copyright (C) 2021-2022 Bhathiya Perera (JaDogg)
*/
//------------------------------------------------------------------------------
//  based on example -> nuklear-sapp.c
#ifndef YK__BHASKNK_SINGLE_HEADER
#define YK__BHASKNK_SINGLE_HEADER
#if _MSC_VER && !__INTEL_COMPILER
#define YK__SOKOL_D3D11
#define _CRT_SECURE_NO_WARNINGS (1)
#endif
#if defined(YK__BHASKNK_IMPLEMENTATION)
#define YK__SOKOL_GLUE_IMPL
#define YK__SOKOL_GFX_IMPL
#define YK__SOKOL_APP_IMPL
#endif
#include "yk__sokol_app.h"
#include "yk__sokol_gfx.h"
#include "yk__sokol_glue.h"
#define YK__NK_INCLUDE_FIXED_TYPES
#define YK__NK_INCLUDE_STANDARD_IO
#define YK__NK_INCLUDE_DEFAULT_ALLOCATOR
#define YK__NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define YK__NK_INCLUDE_FONT_BAKING
#define YK__NK_INCLUDE_DEFAULT_FONT
#define YK__NK_INCLUDE_STANDARD_VARARGS
#if defined(YK__BHASKNK_IMPLEMENTATION)
#define YK__NK_IMPLEMENTATION
#endif
#include "yk__nuklear.h"
#if defined(YK__BHASKNK_IMPLEMENTATION)
#define YK__SOKOL_NUKLEAR_IMPL
#endif
#include "yk__sokol_nuklear.h"
static int yk__bhasknk_ui_loop(struct yk__nk_context *ctx);
typedef struct {
  int width;
  int height;
  const char *title;
} yk__bhasknk_settings;
static yk__bhasknk_settings yk__bhasknk_init(int argc, char *argv[]);
// -- impl --
#if defined(YK__BHASKNK_IMPLEMENTATION)
#if defined(YK__BHASKNK_WIDTH) && defined(YK__BHASKNK_HEIGHT) &&               \
    defined(YK__BHASKNK_TITLE)
static yk__bhasknk_settings yk__bhasknk_init(int argc, char *argv[]) {
  (void) argc;
  (void) argv;
  return (yk__bhasknk_settings){.width = YK__BHASKNK_WIDTH,
                                .height = YK__BHASKNK_HEIGHT,
                                .title = YK__BHASKNK_TITLE};
}
#endif
void yk__bhasknk_init_sg_snk(void) {
  yk__sg_setup(&(yk__sg_desc){.context = yk__sapp_sgcontext()});
  yk__snk_setup(&(yk__snk_desc_t){.dpi_scale = yk__sapp_dpi_scale()});
}
void yk__bhasknk_frame(void) {
  struct yk__nk_context *ctx = yk__snk_new_frame();
  yk__bhasknk_ui_loop(ctx);
  const yk__sg_pass_action pass_action = {
      .colors[0] = {.action = YK__SG_ACTION_CLEAR,
                    .value = {0.25f, 0.5f, 0.7f, 1.0f}}};
  yk__sg_begin_default_pass(&pass_action, yk__sapp_width(), yk__sapp_height());
  yk__snk_render(yk__sapp_width(), yk__sapp_height());
  yk__sg_end_pass();
  yk__sg_commit();
}
void yk__bhasknk_cleanup(void) {
  yk__snk_shutdown();
  yk__sg_shutdown();
}
void yk__bhasknk_input(const yk__sapp_event *event) {
  yk__snk_handle_event(event);
}
yk__sapp_desc yk__sokol_main(int argc, char *argv[]) {
  yk__bhasknk_settings settings = yk__bhasknk_init(argc, argv);
  return (yk__sapp_desc){
      .init_cb = yk__bhasknk_init_sg_snk,
      .frame_cb = yk__bhasknk_frame,
      .cleanup_cb = yk__bhasknk_cleanup,
      .event_cb = yk__bhasknk_input,
      .enable_clipboard = true,
      .width = settings.width,
      .height = settings.height,
      .window_title = settings.title,
      .ios_keyboard_resizes_canvas = true,
      .icon.yk__sokol_default = true,
  };
}
#endif
#if defined(__GNUC__)
#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#endif
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif
/**
 * ---------------
 * Below is how your UI loop need to look like
 * ---------------
 */
#if 0
static int yk__bhasknk_ui_loop(struct yk__nk_context *ctx) { return 0; }
static yk__bhasknk_settings yk__bhasknk_init(int argc, char *argv[]) {
  (void) argc;
  (void) argv;
  return (yk__bhasknk_settings){.width = 1024, .height = 768, .title = "hello"};
}
#endif
#endif