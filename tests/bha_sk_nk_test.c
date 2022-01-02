// Based on -> https://github.com/Immediate-Mode-UI/Nuklear/blob/master/example/canvas.c
#define YK__BHASKNK_WIDTH 1024
#define YK__BHASKNK_HEIGHT 768
#define YK__BHASKNK_TITLE "title"
#define YK__BHASKNK_IMPLEMENTATION
#include "yk__bhasknk.h"
struct custom_canvas {
  struct yk__nk_command_buffer *painter;
  struct yk__nk_vec2 item_spacing;
  struct yk__nk_vec2 panel_padding;
  struct yk__nk_style_item window_background;
};
static void canvas_begin(struct yk__nk_context *ctx,
                         struct custom_canvas *canvas, yk__nk_flags flags,
                         int x, int y, int width, int height,
                         struct yk__nk_color background_color) {
  /* save style properties which will be overwritten */
  canvas->panel_padding = ctx->style.window.padding;
  canvas->item_spacing = ctx->style.window.spacing;
  canvas->window_background = ctx->style.window.fixed_background;
  /* use the complete window space and set background */
  ctx->style.window.spacing = yk__nk_vec2(0, 0);
  ctx->style.window.padding = yk__nk_vec2(0, 0);
  ctx->style.window.fixed_background =
      yk__nk_style_item_color(background_color);
  /* create/update window and set position + size */
  flags = flags & ~YK__NK_WINDOW_DYNAMIC;
  yk__nk_window_set_bounds(ctx, "Window", yk__nk_rect(x, y, width, height));
  yk__nk_begin(ctx, "Window", yk__nk_rect(x, y, width, height),
               YK__NK_WINDOW_NO_SCROLLBAR | flags);
  /* allocate the complete window space for drawing */
  {
    struct yk__nk_rect total_space;
    total_space = yk__nk_window_get_content_region(ctx);
    yk__nk_layout_row_dynamic(ctx, total_space.h, 1);
    yk__nk_widget(&total_space, ctx);
    canvas->painter = yk__nk_window_get_canvas(ctx);
  }
}
static void canvas_end(struct yk__nk_context *ctx,
                       struct custom_canvas *canvas) {
  yk__nk_end(ctx);
  ctx->style.window.spacing = canvas->panel_padding;
  ctx->style.window.padding = canvas->item_spacing;
  ctx->style.window.fixed_background = canvas->window_background;
}
static int yk__bhasknk_ui_loop(struct yk__nk_context *ctx) {
  {
    struct custom_canvas canvas;
    canvas_begin(ctx, &canvas, 0, 0, 0, YK__BHASKNK_WIDTH, YK__BHASKNK_HEIGHT,
                 yk__nk_rgb(250, 250, 250));
    yk__nk_fill_rect(canvas.painter, yk__nk_rect(15, 15, 210, 210), 5,
                     yk__nk_rgb(247, 230, 154));
    yk__nk_fill_rect(canvas.painter, yk__nk_rect(20, 20, 200, 200), 5,
                     yk__nk_rgb(188, 174, 118));
    yk__nk_draw_text(canvas.painter, yk__nk_rect(30, 30, 150, 20),
                     "Text to draw", 12, ctx->style.font,
                     yk__nk_rgb(188, 174, 118), yk__nk_rgb(0, 0, 0));
    yk__nk_fill_rect(canvas.painter, yk__nk_rect(250, 20, 100, 100), 0,
                     yk__nk_rgb(0, 0, 255));
    yk__nk_fill_circle(canvas.painter, yk__nk_rect(20, 250, 100, 100),
                       yk__nk_rgb(255, 0, 0));
    yk__nk_fill_triangle(canvas.painter, 250, 250, 350, 250, 300, 350,
                         yk__nk_rgb(0, 255, 0));
    yk__nk_fill_arc(canvas.painter, 300, 180, 50, 0, 3.141592654f * 3.0f / 4.0f,
                    yk__nk_rgb(255, 255, 0));
    {
      float points[12];
      points[0] = 200;
      points[1] = 250;
      points[2] = 250;
      points[3] = 350;
      points[4] = 225;
      points[5] = 350;
      points[6] = 200;
      points[7] = 300;
      points[8] = 175;
      points[9] = 350;
      points[10] = 150;
      points[11] = 350;
      yk__nk_fill_polygon(canvas.painter, points, 6, yk__nk_rgb(0, 0, 0));
    }
    yk__nk_stroke_line(canvas.painter, 15, 10, 200, 10, 2.0f,
                       yk__nk_rgb(189, 45, 75));
    yk__nk_stroke_rect(canvas.painter, yk__nk_rect(370, 20, 100, 100), 10, 3,
                       yk__nk_rgb(0, 0, 255));
    yk__nk_stroke_curve(canvas.painter, 380, 200, 405, 270, 455, 120, 480, 200,
                        2, yk__nk_rgb(0, 150, 220));
    yk__nk_stroke_circle(canvas.painter, yk__nk_rect(20, 370, 100, 100), 5,
                         yk__nk_rgb(0, 255, 120));
    yk__nk_stroke_triangle(canvas.painter, 370, 250, 470, 250, 420, 350, 6,
                           yk__nk_rgb(255, 0, 143));
    canvas_end(ctx, &canvas);
  }
  return 0;
}
#if 0
// not needed as 3 defines are above
static yk__bhasknk_settings yk__bhasknk_init(int argc, char *argv[]) {
  (void) argc;
  (void) argv;
  return (yk__bhasknk_settings){.width = YK__BHASKNK_WIDTH,
                                .height = YK__BHASKNK_HEIGHT,
                                .title = "hello"};
}
#endif