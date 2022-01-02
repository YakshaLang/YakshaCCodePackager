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
#ifndef BHASKNK_SINGLE_HEADER
#define BHASKNK_SINGLE_HEADER
#if _MSC_VER && !__INTEL_COMPILER
#define SOKOL_D3D11
#define _CRT_SECURE_NO_WARNINGS (1)
#endif
#if defined(BHASKNK_IMPLEMENTATION)
#define SOKOL_GLUE_IMPL
#define SOKOL_GFX_IMPL
#define SOKOL_APP_IMPL
#endif
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_STANDARD_VARARGS
#if defined(BHASKNK_IMPLEMENTATION)
#define NK_IMPLEMENTATION
#endif
#include "nuklear.h"
#if defined(BHASKNK_IMPLEMENTATION)
#define SOKOL_NUKLEAR_IMPL
#endif
#include "sokol_nuklear.h"
static int bhasknk_ui_loop(struct nk_context *ctx);
typedef struct {
  int width;
  int height;
  const char *title;
} bhasknk_settings;
static bhasknk_settings bhasknk_init(int argc, char *argv[]);
// -- impl --
#if defined(BHASKNK_IMPLEMENTATION)
#if defined(BHASKNK_WIDTH) && defined(BHASKNK_HEIGHT) && defined(BHASKNK_TITLE)
static bhasknk_settings bhasknk_init(int argc, char *argv[]) {
  (void) argc;
  (void) argv;
  return (bhasknk_settings){.width = BHASKNK_WIDTH,
                            .height = BHASKNK_HEIGHT,
                            .title = BHASKNK_TITLE};
}
#endif
void bhasknk_init_sg_snk(void) {
  sg_setup(&(sg_desc){.context = sapp_sgcontext()});
  snk_setup(&(snk_desc_t){.dpi_scale = sapp_dpi_scale()});
}
void bhasknk_frame(void) {
  struct nk_context *ctx = snk_new_frame();
  bhasknk_ui_loop(ctx);
  const sg_pass_action pass_action = {
      .colors[0] = {.action = SG_ACTION_CLEAR,
                    .value = {0.25f, 0.5f, 0.7f, 1.0f}}};
  sg_begin_default_pass(&pass_action, sapp_width(), sapp_height());
  snk_render(sapp_width(), sapp_height());
  sg_end_pass();
  sg_commit();
}
void bhasknk_cleanup(void) {
  snk_shutdown();
  sg_shutdown();
}
void bhasknk_input(const sapp_event *event) { snk_handle_event(event); }
sapp_desc sokol_main(int argc, char *argv[]) {
  bhasknk_settings settings = bhasknk_init(argc, argv);
  return (sapp_desc){
      .init_cb = bhasknk_init_sg_snk,
      .frame_cb = bhasknk_frame,
      .cleanup_cb = bhasknk_cleanup,
      .event_cb = bhasknk_input,
      .enable_clipboard = true,
      .width = settings.width,
      .height = settings.height,
      .window_title = settings.title,
      .ios_keyboard_resizes_canvas = true,
      .icon.sokol_default = true,
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
static int bhasknk_ui_loop(struct nk_context *ctx) { return 0; }
static bhasknk_settings bhasknk_init(int argc, char *argv[]) {
  (void) argc;
  (void) argv;
  return (bhasknk_settings){.width = 1024, .height = 768, .title = "hello"};
}
#endif
#endif