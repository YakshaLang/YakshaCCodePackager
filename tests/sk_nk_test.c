//------------------------------------------------------------------------------
//  nuklear-sapp.c
//
//  Demonstrates Nuklear UI rendering in C via
//  sokol_gfx.h + sokol_nuklear.h + nuklear.h
//
//  Nuklear UI on github: https://github.com/Immediate-Mode-UI/Nuklear
//------------------------------------------------------------------------------
// this is needed for the Nuklear example code further down
#define YK__SOKOL_D3D11
#define YK__SOKOL_GLUE_IMPL
#define YK__SOKOL_GFX_IMPL
#define YK__SOKOL_APP_IMPL
#define _CRT_SECURE_NO_WARNINGS (1)
#include "yk__sokol_app.h"
#include "yk__sokol_gfx.h"
#include "yk__sokol_glue.h"
// include nuklear.h before the sokol_nuklear.h implementation
#define YK__NK_INCLUDE_FIXED_TYPES
#define YK__NK_INCLUDE_STANDARD_IO
#define YK__NK_INCLUDE_DEFAULT_ALLOCATOR
#define YK__NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define YK__NK_INCLUDE_FONT_BAKING
#define YK__NK_INCLUDE_DEFAULT_FONT
#define YK__NK_INCLUDE_STANDARD_VARARGS
#define YK__NK_IMPLEMENTATION
#include "yk__nuklear.h"
#define YK__SOKOL_NUKLEAR_IMPL
#include "yk__sokol_nuklear.h"
static int draw_demo_ui(struct yk__nk_context *ctx);
void init(void) {
  // setup sokol-gfx, sokol-time and sokol-nuklear
  yk__sg_setup(&(yk__sg_desc){.context = yk__sapp_sgcontext()});
  // use sokol-nuklear with all default-options (we're not doing
  // multi-sampled rendering or using non-default pixel formats)
  yk__snk_setup(&(yk__snk_desc_t){.dpi_scale = yk__sapp_dpi_scale()});
}
void frame(void) {
  struct yk__nk_context *ctx = yk__snk_new_frame();
  // see big function at end of file
  draw_demo_ui(ctx);
  // the sokol_gfx draw pass
  const yk__sg_pass_action pass_action = {
      .colors[0] = {.action = YK__SG_ACTION_CLEAR,
                    .value = {0.25f, 0.5f, 0.7f, 1.0f}}};
  yk__sg_begin_default_pass(&pass_action, yk__sapp_width(), yk__sapp_height());
  yk__snk_render(yk__sapp_width(), yk__sapp_height());
  yk__sg_end_pass();
  yk__sg_commit();
}
void cleanup(void) {
  yk__snk_shutdown();
  yk__sg_shutdown();
}
void input(const yk__sapp_event *event) { yk__snk_handle_event(event); }
yk__sapp_desc yk__sokol_main(int argc, char *argv[]) {
  (void) argc;
  (void) argv;
  return (yk__sapp_desc){
      .init_cb = init,
      .frame_cb = frame,
      .cleanup_cb = cleanup,
      .event_cb = input,
      .enable_clipboard = true,
      .width = 1024,
      .height = 768,
      .window_title = "nuklear (sokol-app)",
      .ios_keyboard_resizes_canvas = true,
      .icon.yk__sokol_default = true,
  };
}
/* copied from: https://github.com/Immediate-Mode-UI/Nuklear/blob/master/demo/overview.c */
#if defined(__GNUC__)
#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#endif
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif
#include <limits.h>
#include <math.h> // sin, cos, fabs
#include <stdio.h>// fprintf, sprintf
#include <time.h>
static int draw_demo_ui(struct yk__nk_context *ctx) {
  /* window flags */
  static int show_menu = yk__nk_true;
  static int titlebar = yk__nk_true;
  static int border = yk__nk_true;
  static int resize = yk__nk_true;
  static int movable = yk__nk_true;
  static int no_scrollbar = yk__nk_false;
  static int scale_left = yk__nk_false;
  static yk__nk_flags window_flags = 0;
  static int minimizable = yk__nk_true;
  /* popups */
  static enum yk__nk_style_header_align header_align = YK__NK_HEADER_RIGHT;
  static int show_app_about = yk__nk_false;
  /* window flags */
  window_flags = 0;
  ctx->style.window.header.align = header_align;
  if (border) window_flags |= YK__NK_WINDOW_BORDER;
  if (resize) window_flags |= YK__NK_WINDOW_SCALABLE;
  if (movable) window_flags |= YK__NK_WINDOW_MOVABLE;
  if (no_scrollbar) window_flags |= YK__NK_WINDOW_NO_SCROLLBAR;
  if (scale_left) window_flags |= YK__NK_WINDOW_SCALE_LEFT;
  if (minimizable) window_flags |= YK__NK_WINDOW_MINIMIZABLE;
  if (yk__nk_begin(ctx, "Overview", yk__nk_rect(10, 25, 400, 600),
                   window_flags)) {
    if (show_menu) {
      /* menubar */
      enum menu_states { MENU_DEFAULT, MENU_WINDOWS };
      static yk__nk_size mprog = 60;
      static int mslider = 10;
      static int mcheck = yk__nk_true;
      yk__nk_menubar_begin(ctx);
      /* menu #1 */
      yk__nk_layout_row_begin(ctx, YK__NK_STATIC, 25, 5);
      yk__nk_layout_row_push(ctx, 45);
      if (yk__nk_menu_begin_label(ctx, "MENU", YK__NK_TEXT_LEFT,
                                  yk__nk_vec2(120, 200))) {
        static size_t prog = 40;
        static int slider = 10;
        static int check = yk__nk_true;
        yk__nk_layout_row_dynamic(ctx, 25, 1);
        if (yk__nk_menu_item_label(ctx, "Hide", YK__NK_TEXT_LEFT))
          show_menu = yk__nk_false;
        if (yk__nk_menu_item_label(ctx, "About", YK__NK_TEXT_LEFT))
          show_app_about = yk__nk_true;
        yk__nk_progress(ctx, &prog, 100, YK__NK_MODIFIABLE);
        yk__nk_slider_int(ctx, 0, &slider, 16, 1);
        yk__nk_checkbox_label(ctx, "check", &check);
        yk__nk_menu_end(ctx);
      }
      /* menu #2 */
      yk__nk_layout_row_push(ctx, 60);
      if (yk__nk_menu_begin_label(ctx, "ADVANCED", YK__NK_TEXT_LEFT,
                                  yk__nk_vec2(200, 600))) {
        enum menu_state {
          MENU_NONE,
          MENU_FILE,
          MENU_EDIT,
          MENU_VIEW,
          MENU_CHART
        };
        static enum menu_state menu_state = MENU_NONE;
        enum yk__nk_collapse_states state;
        state = (menu_state == MENU_FILE) ? YK__NK_MAXIMIZED : YK__NK_MINIMIZED;
        if (yk__nk_tree_state_push(ctx, YK__NK_TREE_TAB, "FILE", &state)) {
          menu_state = MENU_FILE;
          yk__nk_menu_item_label(ctx, "New", YK__NK_TEXT_LEFT);
          yk__nk_menu_item_label(ctx, "Open", YK__NK_TEXT_LEFT);
          yk__nk_menu_item_label(ctx, "Save", YK__NK_TEXT_LEFT);
          yk__nk_menu_item_label(ctx, "Close", YK__NK_TEXT_LEFT);
          yk__nk_menu_item_label(ctx, "Exit", YK__NK_TEXT_LEFT);
          yk__nk_tree_pop(ctx);
        } else
          menu_state = (menu_state == MENU_FILE) ? MENU_NONE : menu_state;
        state = (menu_state == MENU_EDIT) ? YK__NK_MAXIMIZED : YK__NK_MINIMIZED;
        if (yk__nk_tree_state_push(ctx, YK__NK_TREE_TAB, "EDIT", &state)) {
          menu_state = MENU_EDIT;
          yk__nk_menu_item_label(ctx, "Copy", YK__NK_TEXT_LEFT);
          yk__nk_menu_item_label(ctx, "Delete", YK__NK_TEXT_LEFT);
          yk__nk_menu_item_label(ctx, "Cut", YK__NK_TEXT_LEFT);
          yk__nk_menu_item_label(ctx, "Paste", YK__NK_TEXT_LEFT);
          yk__nk_tree_pop(ctx);
        } else
          menu_state = (menu_state == MENU_EDIT) ? MENU_NONE : menu_state;
        state = (menu_state == MENU_VIEW) ? YK__NK_MAXIMIZED : YK__NK_MINIMIZED;
        if (yk__nk_tree_state_push(ctx, YK__NK_TREE_TAB, "VIEW", &state)) {
          menu_state = MENU_VIEW;
          yk__nk_menu_item_label(ctx, "About", YK__NK_TEXT_LEFT);
          yk__nk_menu_item_label(ctx, "Options", YK__NK_TEXT_LEFT);
          yk__nk_menu_item_label(ctx, "Customize", YK__NK_TEXT_LEFT);
          yk__nk_tree_pop(ctx);
        } else
          menu_state = (menu_state == MENU_VIEW) ? MENU_NONE : menu_state;
        state =
            (menu_state == MENU_CHART) ? YK__NK_MAXIMIZED : YK__NK_MINIMIZED;
        if (yk__nk_tree_state_push(ctx, YK__NK_TREE_TAB, "CHART", &state)) {
          size_t i = 0;
          const float values[] = {26.0f, 13.0f, 30.0f, 15.0f, 25.0f, 10.0f,
                                  20.0f, 40.0f, 12.0f, 8.0f,  22.0f, 28.0f};
          menu_state = MENU_CHART;
          yk__nk_layout_row_dynamic(ctx, 150, 1);
          yk__nk_chart_begin(ctx, YK__NK_CHART_COLUMN, YK__NK_LEN(values), 0,
                             50);
          for (i = 0; i < YK__NK_LEN(values); ++i)
            yk__nk_chart_push(ctx, values[i]);
          yk__nk_chart_end(ctx);
          yk__nk_tree_pop(ctx);
        } else
          menu_state = (menu_state == MENU_CHART) ? MENU_NONE : menu_state;
        yk__nk_menu_end(ctx);
      }
      /* menu widgets */
      yk__nk_layout_row_push(ctx, 70);
      yk__nk_progress(ctx, &mprog, 100, YK__NK_MODIFIABLE);
      yk__nk_slider_int(ctx, 0, &mslider, 16, 1);
      yk__nk_checkbox_label(ctx, "check", &mcheck);
      yk__nk_menubar_end(ctx);
    }
    if (show_app_about) {
      /* about popup */
      static struct yk__nk_rect s = {20, 100, 300, 190};
      if (yk__nk_popup_begin(ctx, YK__NK_POPUP_STATIC, "About",
                             YK__NK_WINDOW_CLOSABLE, s)) {
        yk__nk_layout_row_dynamic(ctx, 20, 1);
        yk__nk_label(ctx, "Nuklear", YK__NK_TEXT_LEFT);
        yk__nk_label(ctx, "By Micha Mettke", YK__NK_TEXT_LEFT);
        yk__nk_label(ctx,
                     "nuklear is licensed under the public domain License.",
                     YK__NK_TEXT_LEFT);
        yk__nk_popup_end(ctx);
      } else
        show_app_about = yk__nk_false;
    }
    /* window flags */
    if (yk__nk_tree_push(ctx, YK__NK_TREE_TAB, "Window", YK__NK_MINIMIZED)) {
      yk__nk_layout_row_dynamic(ctx, 30, 2);
      yk__nk_checkbox_label(ctx, "Titlebar", &titlebar);
      yk__nk_checkbox_label(ctx, "Menu", &show_menu);
      yk__nk_checkbox_label(ctx, "Border", &border);
      yk__nk_checkbox_label(ctx, "Resizable", &resize);
      yk__nk_checkbox_label(ctx, "Movable", &movable);
      yk__nk_checkbox_label(ctx, "No Scrollbar", &no_scrollbar);
      yk__nk_checkbox_label(ctx, "Minimizable", &minimizable);
      yk__nk_checkbox_label(ctx, "Scale Left", &scale_left);
      yk__nk_tree_pop(ctx);
    }
    if (yk__nk_tree_push(ctx, YK__NK_TREE_TAB, "Widgets", YK__NK_MINIMIZED)) {
      enum options { A, B, C };
      static int checkbox;
      static int option;
      if (yk__nk_tree_push(ctx, YK__NK_TREE_NODE, "Text", YK__NK_MINIMIZED)) {
        /* Text Widgets */
        yk__nk_layout_row_dynamic(ctx, 20, 1);
        yk__nk_label(ctx, "Label aligned left", YK__NK_TEXT_LEFT);
        yk__nk_label(ctx, "Label aligned centered", YK__NK_TEXT_CENTERED);
        yk__nk_label(ctx, "Label aligned right", YK__NK_TEXT_RIGHT);
        yk__nk_label_colored(ctx, "Blue text", YK__NK_TEXT_LEFT,
                             yk__nk_rgb(0, 0, 255));
        yk__nk_label_colored(ctx, "Yellow text", YK__NK_TEXT_LEFT,
                             yk__nk_rgb(255, 255, 0));
        yk__nk_text(ctx, "Text without /0", 15, YK__NK_TEXT_RIGHT);
        yk__nk_layout_row_static(ctx, 100, 200, 1);
        yk__nk_label_wrap(
            ctx, "This is a very long line to hopefully get this text to be "
                 "wrapped into multiple lines to show line wrapping");
        yk__nk_layout_row_dynamic(ctx, 100, 1);
        yk__nk_label_wrap(ctx, "This is another long text to show dynamic "
                               "window changes on multiline text");
        yk__nk_tree_pop(ctx);
      }
      if (yk__nk_tree_push(ctx, YK__NK_TREE_NODE, "Button", YK__NK_MINIMIZED)) {
        /* Buttons Widgets */
        yk__nk_layout_row_static(ctx, 30, 100, 3);
        if (yk__nk_button_label(ctx, "Button"))
          fprintf(stdout, "Button pressed!\n");
        yk__nk_button_set_behavior(ctx, YK__NK_BUTTON_REPEATER);
        if (yk__nk_button_label(ctx, "Repeater"))
          fprintf(stdout, "Repeater is being pressed!\n");
        yk__nk_button_set_behavior(ctx, YK__NK_BUTTON_DEFAULT);
        yk__nk_button_color(ctx, yk__nk_rgb(0, 0, 255));
        yk__nk_layout_row_static(ctx, 25, 25, 8);
        yk__nk_button_symbol(ctx, YK__NK_SYMBOL_CIRCLE_SOLID);
        yk__nk_button_symbol(ctx, YK__NK_SYMBOL_CIRCLE_OUTLINE);
        yk__nk_button_symbol(ctx, YK__NK_SYMBOL_RECT_SOLID);
        yk__nk_button_symbol(ctx, YK__NK_SYMBOL_RECT_OUTLINE);
        yk__nk_button_symbol(ctx, YK__NK_SYMBOL_TRIANGLE_UP);
        yk__nk_button_symbol(ctx, YK__NK_SYMBOL_TRIANGLE_DOWN);
        yk__nk_button_symbol(ctx, YK__NK_SYMBOL_TRIANGLE_LEFT);
        yk__nk_button_symbol(ctx, YK__NK_SYMBOL_TRIANGLE_RIGHT);
        yk__nk_layout_row_static(ctx, 30, 100, 2);
        yk__nk_button_symbol_label(ctx, YK__NK_SYMBOL_TRIANGLE_LEFT, "prev",
                                   YK__NK_TEXT_RIGHT);
        yk__nk_button_symbol_label(ctx, YK__NK_SYMBOL_TRIANGLE_RIGHT, "next",
                                   YK__NK_TEXT_LEFT);
        yk__nk_tree_pop(ctx);
      }
      if (yk__nk_tree_push(ctx, YK__NK_TREE_NODE, "Basic", YK__NK_MINIMIZED)) {
        /* Basic widgets */
        static int int_slider = 5;
        static float float_slider = 2.5f;
        static size_t prog_value = 40;
        static float property_float = 2;
        static int property_int = 10;
        static int property_neg = 10;
        static float range_float_min = 0;
        static float range_float_max = 100;
        static float range_float_value = 50;
        static int range_int_min = 0;
        static int range_int_value = 2048;
        static int range_int_max = 4096;
        static const float ratio[] = {120, 150};
        yk__nk_layout_row_static(ctx, 30, 100, 1);
        yk__nk_checkbox_label(ctx, "Checkbox", &checkbox);
        yk__nk_layout_row_static(ctx, 30, 80, 3);
        option = yk__nk_option_label(ctx, "optionA", option == A) ? A : option;
        option = yk__nk_option_label(ctx, "optionB", option == B) ? B : option;
        option = yk__nk_option_label(ctx, "optionC", option == C) ? C : option;
        yk__nk_layout_row(ctx, YK__NK_STATIC, 30, 2, ratio);
        yk__nk_labelf(ctx, YK__NK_TEXT_LEFT, "Slider int");
        yk__nk_slider_int(ctx, 0, &int_slider, 10, 1);
        yk__nk_label(ctx, "Slider float", YK__NK_TEXT_LEFT);
        yk__nk_slider_float(ctx, 0, &float_slider, 5.0, 0.5f);
        yk__nk_labelf(ctx, YK__NK_TEXT_LEFT, "Progressbar: %zu", prog_value);
        yk__nk_progress(ctx, &prog_value, 100, YK__NK_MODIFIABLE);
        yk__nk_layout_row(ctx, YK__NK_STATIC, 25, 2, ratio);
        yk__nk_label(ctx, "Property float:", YK__NK_TEXT_LEFT);
        yk__nk_property_float(ctx, "Float:", 0, &property_float, 64.0f, 0.1f,
                              0.2f);
        yk__nk_label(ctx, "Property int:", YK__NK_TEXT_LEFT);
        yk__nk_property_int(ctx, "Int:", 0, &property_int, 100, 1, 1);
        yk__nk_label(ctx, "Property neg:", YK__NK_TEXT_LEFT);
        yk__nk_property_int(ctx, "Neg:", -10, &property_neg, 10, 1, 1);
        yk__nk_layout_row_dynamic(ctx, 25, 1);
        yk__nk_label(ctx, "Range:", YK__NK_TEXT_LEFT);
        yk__nk_layout_row_dynamic(ctx, 25, 3);
        yk__nk_property_float(ctx, "#min:", 0, &range_float_min,
                              range_float_max, 1.0f, 0.2f);
        yk__nk_property_float(ctx, "#float:", range_float_min,
                              &range_float_value, range_float_max, 1.0f, 0.2f);
        yk__nk_property_float(ctx, "#max:", range_float_min, &range_float_max,
                              100, 1.0f, 0.2f);
        yk__nk_property_int(ctx, "#min:", INT_MIN, &range_int_min,
                            range_int_max, 1, 10);
        yk__nk_property_int(ctx, "#neg:", range_int_min, &range_int_value,
                            range_int_max, 1, 10);
        yk__nk_property_int(ctx, "#max:", range_int_min, &range_int_max,
                            INT_MAX, 1, 10);
        yk__nk_tree_pop(ctx);
      }
      if (yk__nk_tree_push(ctx, YK__NK_TREE_NODE, "Inactive",
                           YK__NK_MINIMIZED)) {
        static int inactive = 1;
        yk__nk_layout_row_dynamic(ctx, 30, 1);
        yk__nk_checkbox_label(ctx, "Inactive", &inactive);
        yk__nk_layout_row_static(ctx, 30, 80, 1);
        if (inactive) {
          struct yk__nk_style_button button;
          button = ctx->style.button;
          ctx->style.button.normal =
              yk__nk_style_item_color(yk__nk_rgb(40, 40, 40));
          ctx->style.button.hover =
              yk__nk_style_item_color(yk__nk_rgb(40, 40, 40));
          ctx->style.button.active =
              yk__nk_style_item_color(yk__nk_rgb(40, 40, 40));
          ctx->style.button.border_color = yk__nk_rgb(60, 60, 60);
          ctx->style.button.text_background = yk__nk_rgb(60, 60, 60);
          ctx->style.button.text_normal = yk__nk_rgb(60, 60, 60);
          ctx->style.button.text_hover = yk__nk_rgb(60, 60, 60);
          ctx->style.button.text_active = yk__nk_rgb(60, 60, 60);
          yk__nk_button_label(ctx, "button");
          ctx->style.button = button;
        } else if (yk__nk_button_label(ctx, "button"))
          fprintf(stdout, "button pressed\n");
        yk__nk_tree_pop(ctx);
      }
      if (yk__nk_tree_push(ctx, YK__NK_TREE_NODE, "Selectable",
                           YK__NK_MINIMIZED)) {
        if (yk__nk_tree_push(ctx, YK__NK_TREE_NODE, "List", YK__NK_MINIMIZED)) {
          static int selected[4] = {yk__nk_false, yk__nk_false, yk__nk_true,
                                    yk__nk_false};
          yk__nk_layout_row_static(ctx, 18, 100, 1);
          yk__nk_selectable_label(ctx, "Selectable", YK__NK_TEXT_LEFT,
                                  &selected[0]);
          yk__nk_selectable_label(ctx, "Selectable", YK__NK_TEXT_LEFT,
                                  &selected[1]);
          yk__nk_label(ctx, "Not Selectable", YK__NK_TEXT_LEFT);
          yk__nk_selectable_label(ctx, "Selectable", YK__NK_TEXT_LEFT,
                                  &selected[2]);
          yk__nk_selectable_label(ctx, "Selectable", YK__NK_TEXT_LEFT,
                                  &selected[3]);
          yk__nk_tree_pop(ctx);
        }
        if (yk__nk_tree_push(ctx, YK__NK_TREE_NODE, "Grid", YK__NK_MINIMIZED)) {
          int i;
          static int selected[16] = {1, 0, 0, 0, 0, 1, 0, 0,
                                     0, 0, 1, 0, 0, 0, 0, 1};
          yk__nk_layout_row_static(ctx, 50, 50, 4);
          for (i = 0; i < 16; ++i) {
            if (yk__nk_selectable_label(ctx, "Z", YK__NK_TEXT_CENTERED,
                                        &selected[i])) {
              int x = (i % 4), y = i / 4;
              if (x > 0) selected[i - 1] ^= 1;
              if (x < 3) selected[i + 1] ^= 1;
              if (y > 0) selected[i - 4] ^= 1;
              if (y < 3) selected[i + 4] ^= 1;
            }
          }
          yk__nk_tree_pop(ctx);
        }
        yk__nk_tree_pop(ctx);
      }
      if (yk__nk_tree_push(ctx, YK__NK_TREE_NODE, "Combo", YK__NK_MINIMIZED)) {
        /* Combobox Widgets
                 * In this library comboboxes are not limited to being a popup
                 * list of selectable text. Instead it is a abstract concept of
                 * having something that is *selected* or displayed, a popup window
                 * which opens if something needs to be modified and the content
                 * of the popup which causes the *selected* or displayed value to
                 * change or if wanted close the combobox.
                 *
                 * While strange at first handling comboboxes in a abstract way
                 * solves the problem of overloaded window content. For example
                 * changing a color value requires 4 value modifier (slider, property,...)
                 * for RGBA then you need a label and ways to display the current color.
                 * If you want to go fancy you even add rgb and hsv ratio boxes.
                 * While fine for one color if you have a lot of them it because
                 * tedious to look at and quite wasteful in space. You could add
                 * a popup which modifies the color but this does not solve the
                 * fact that it still requires a lot of cluttered space to do.
                 *
                 * In these kind of instance abstract comboboxes are quite handy. All
                 * value modifiers are hidden inside the combobox popup and only
                 * the color is shown if not open. This combines the clarity of the
                 * popup with the ease of use of just using the space for modifiers.
                 *
                 * Other instances are for example time and especially date picker,
                 * which only show the currently activated time/data and hide the
                 * selection logic inside the combobox popup.
                 */
        static float chart_selection = 8.0f;
        static int current_weapon = 0;
        static int check_values[5];
        static float position[3];
        static struct yk__nk_color combo_color = {130, 50, 50, 255};
        static struct yk__nk_colorf combo_color2 = {0.509f, 0.705f, 0.2f, 1.0f};
        static size_t prog_a = 20, prog_b = 40, prog_c = 10, prog_d = 90;
        static const char *weapons[] = {"Fist", "Pistol", "Shotgun", "Plasma",
                                        "BFG"};
        char buffer[64];
        size_t sum = 0;
        /* default combobox */
        yk__nk_layout_row_static(ctx, 25, 200, 1);
        current_weapon =
            yk__nk_combo(ctx, weapons, YK__NK_LEN(weapons), current_weapon, 25,
                         yk__nk_vec2(200, 200));
        /* slider color combobox */
        if (yk__nk_combo_begin_color(ctx, combo_color, yk__nk_vec2(200, 200))) {
          float ratios[] = {0.15f, 0.85f};
          yk__nk_layout_row(ctx, YK__NK_DYNAMIC, 30, 2, ratios);
          yk__nk_label(ctx, "R:", YK__NK_TEXT_LEFT);
          combo_color.r =
              (yk__nk_byte) yk__nk_slide_int(ctx, 0, combo_color.r, 255, 5);
          yk__nk_label(ctx, "G:", YK__NK_TEXT_LEFT);
          combo_color.g =
              (yk__nk_byte) yk__nk_slide_int(ctx, 0, combo_color.g, 255, 5);
          yk__nk_label(ctx, "B:", YK__NK_TEXT_LEFT);
          combo_color.b =
              (yk__nk_byte) yk__nk_slide_int(ctx, 0, combo_color.b, 255, 5);
          yk__nk_label(ctx, "A:", YK__NK_TEXT_LEFT);
          combo_color.a =
              (yk__nk_byte) yk__nk_slide_int(ctx, 0, combo_color.a, 255, 5);
          yk__nk_combo_end(ctx);
        }
        /* complex color combobox */
        if (yk__nk_combo_begin_color(ctx, yk__nk_rgb_cf(combo_color2),
                                     yk__nk_vec2(200, 400))) {
          enum color_mode { COL_RGB, COL_HSV };
          static int col_mode = COL_RGB;
#ifndef DEMO_DO_NOT_USE_COLOR_PICKER
          yk__nk_layout_row_dynamic(ctx, 120, 1);
          combo_color2 = yk__nk_color_picker(ctx, combo_color2, YK__NK_RGBA);
#endif
          yk__nk_layout_row_dynamic(ctx, 25, 2);
          col_mode = yk__nk_option_label(ctx, "RGB", col_mode == COL_RGB)
                         ? COL_RGB
                         : col_mode;
          col_mode = yk__nk_option_label(ctx, "HSV", col_mode == COL_HSV)
                         ? COL_HSV
                         : col_mode;
          yk__nk_layout_row_dynamic(ctx, 25, 1);
          if (col_mode == COL_RGB) {
            combo_color2.r = yk__nk_propertyf(ctx, "#R:", 0, combo_color2.r,
                                              1.0f, 0.01f, 0.005f);
            combo_color2.g = yk__nk_propertyf(ctx, "#G:", 0, combo_color2.g,
                                              1.0f, 0.01f, 0.005f);
            combo_color2.b = yk__nk_propertyf(ctx, "#B:", 0, combo_color2.b,
                                              1.0f, 0.01f, 0.005f);
            combo_color2.a = yk__nk_propertyf(ctx, "#A:", 0, combo_color2.a,
                                              1.0f, 0.01f, 0.005f);
          } else {
            float hsva[4];
            yk__nk_colorf_hsva_fv(hsva, combo_color2);
            hsva[0] =
                yk__nk_propertyf(ctx, "#H:", 0, hsva[0], 1.0f, 0.01f, 0.05f);
            hsva[1] =
                yk__nk_propertyf(ctx, "#S:", 0, hsva[1], 1.0f, 0.01f, 0.05f);
            hsva[2] =
                yk__nk_propertyf(ctx, "#V:", 0, hsva[2], 1.0f, 0.01f, 0.05f);
            hsva[3] =
                yk__nk_propertyf(ctx, "#A:", 0, hsva[3], 1.0f, 0.01f, 0.05f);
            combo_color2 = yk__nk_hsva_colorfv(hsva);
          }
          yk__nk_combo_end(ctx);
        }
        /* progressbar combobox */
        sum = prog_a + prog_b + prog_c + prog_d;
        sprintf(buffer, "%d", (int) sum);
        if (yk__nk_combo_begin_label(ctx, buffer, yk__nk_vec2(200, 200))) {
          yk__nk_layout_row_dynamic(ctx, 30, 1);
          yk__nk_progress(ctx, &prog_a, 100, YK__NK_MODIFIABLE);
          yk__nk_progress(ctx, &prog_b, 100, YK__NK_MODIFIABLE);
          yk__nk_progress(ctx, &prog_c, 100, YK__NK_MODIFIABLE);
          yk__nk_progress(ctx, &prog_d, 100, YK__NK_MODIFIABLE);
          yk__nk_combo_end(ctx);
        }
        /* checkbox combobox */
        sum = (size_t) (check_values[0] + check_values[1] + check_values[2] +
                        check_values[3] + check_values[4]);
        sprintf(buffer, "%d", (int) sum);
        if (yk__nk_combo_begin_label(ctx, buffer, yk__nk_vec2(200, 200))) {
          yk__nk_layout_row_dynamic(ctx, 30, 1);
          yk__nk_checkbox_label(ctx, weapons[0], &check_values[0]);
          yk__nk_checkbox_label(ctx, weapons[1], &check_values[1]);
          yk__nk_checkbox_label(ctx, weapons[2], &check_values[2]);
          yk__nk_checkbox_label(ctx, weapons[3], &check_values[3]);
          yk__nk_combo_end(ctx);
        }
        /* complex text combobox */
        sprintf(buffer, "%.2f, %.2f, %.2f", position[0], position[1],
                position[2]);
        if (yk__nk_combo_begin_label(ctx, buffer, yk__nk_vec2(200, 200))) {
          yk__nk_layout_row_dynamic(ctx, 25, 1);
          yk__nk_property_float(ctx, "#X:", -1024.0f, &position[0], 1024.0f, 1,
                                0.5f);
          yk__nk_property_float(ctx, "#Y:", -1024.0f, &position[1], 1024.0f, 1,
                                0.5f);
          yk__nk_property_float(ctx, "#Z:", -1024.0f, &position[2], 1024.0f, 1,
                                0.5f);
          yk__nk_combo_end(ctx);
        }
        /* chart combobox */
        sprintf(buffer, "%.1f", chart_selection);
        if (yk__nk_combo_begin_label(ctx, buffer, yk__nk_vec2(200, 250))) {
          size_t i = 0;
          static const float values[] = {26.0f, 13.0f, 30.0f, 15.0f, 25.0f,
                                         10.0f, 20.0f, 40.0f, 12.0f, 8.0f,
                                         22.0f, 28.0f, 5.0f};
          yk__nk_layout_row_dynamic(ctx, 150, 1);
          yk__nk_chart_begin(ctx, YK__NK_CHART_COLUMN, YK__NK_LEN(values), 0,
                             50);
          for (i = 0; i < YK__NK_LEN(values); ++i) {
            yk__nk_flags res = yk__nk_chart_push(ctx, values[i]);
            if (res & YK__NK_CHART_CLICKED) {
              chart_selection = values[i];
              yk__nk_combo_close(ctx);
            }
          }
          yk__nk_chart_end(ctx);
          yk__nk_combo_end(ctx);
        }
        {
          static int time_selected = 0;
          static int date_selected = 0;
          static struct tm sel_time;
          static struct tm sel_date;
          if (!time_selected || !date_selected) {
            /* keep time and date updated if nothing is selected */
            time_t cur_time = time(0);
            struct tm *n = localtime(&cur_time);
            if (!time_selected) memcpy(&sel_time, n, sizeof(struct tm));
            if (!date_selected) memcpy(&sel_date, n, sizeof(struct tm));
          }
          /* time combobox */
          sprintf(buffer, "%02d:%02d:%02d", sel_time.tm_hour, sel_time.tm_min,
                  sel_time.tm_sec);
          if (yk__nk_combo_begin_label(ctx, buffer, yk__nk_vec2(200, 250))) {
            time_selected = 1;
            yk__nk_layout_row_dynamic(ctx, 25, 1);
            sel_time.tm_sec =
                yk__nk_propertyi(ctx, "#S:", 0, sel_time.tm_sec, 60, 1, 1);
            sel_time.tm_min =
                yk__nk_propertyi(ctx, "#M:", 0, sel_time.tm_min, 60, 1, 1);
            sel_time.tm_hour =
                yk__nk_propertyi(ctx, "#H:", 0, sel_time.tm_hour, 23, 1, 1);
            yk__nk_combo_end(ctx);
          }
          /* date combobox */
          sprintf(buffer, "%02d-%02d-%02d", sel_date.tm_mday,
                  sel_date.tm_mon + 1, sel_date.tm_year + 1900);
          if (yk__nk_combo_begin_label(ctx, buffer, yk__nk_vec2(350, 400))) {
            int i = 0;
            const char *month[] = {"January", "February", "March",
                                   "April",   "May",      "June",
                                   "July",    "August",   "September",
                                   "October", "November", "December"};
            const char *week_days[] = {"SUN", "MON", "TUE", "WED",
                                       "THU", "FRI", "SAT"};
            const int month_days[] = {31, 28, 31, 30, 31, 30,
                                      31, 31, 30, 31, 30, 31};
            int year = sel_date.tm_year + 1900;
            int leap_year = (!(year % 4) && ((year % 100))) || !(year % 400);
            int days = (sel_date.tm_mon == 1)
                           ? month_days[sel_date.tm_mon] + leap_year
                           : month_days[sel_date.tm_mon];
            /* header with month and year */
            date_selected = 1;
            yk__nk_layout_row_begin(ctx, YK__NK_DYNAMIC, 20, 3);
            yk__nk_layout_row_push(ctx, 0.05f);
            if (yk__nk_button_symbol(ctx, YK__NK_SYMBOL_TRIANGLE_LEFT)) {
              if (sel_date.tm_mon == 0) {
                sel_date.tm_mon = 11;
                sel_date.tm_year = YK__NK_MAX(0, sel_date.tm_year - 1);
              } else
                sel_date.tm_mon--;
            }
            yk__nk_layout_row_push(ctx, 0.9f);
            sprintf(buffer, "%s %d", month[sel_date.tm_mon], year);
            yk__nk_label(ctx, buffer, YK__NK_TEXT_CENTERED);
            yk__nk_layout_row_push(ctx, 0.05f);
            if (yk__nk_button_symbol(ctx, YK__NK_SYMBOL_TRIANGLE_RIGHT)) {
              if (sel_date.tm_mon == 11) {
                sel_date.tm_mon = 0;
                sel_date.tm_year++;
              } else
                sel_date.tm_mon++;
            }
            yk__nk_layout_row_end(ctx);
            /* good old week day formula (double because precision) */
            {
              int year_n = (sel_date.tm_mon < 2) ? year - 1 : year;
              int y = year_n % 100;
              int c = year_n / 100;
              int y4 = (int) ((float) y / 4);
              int c4 = (int) ((float) c / 4);
              int m =
                  (int) (2.6 * (double) (((sel_date.tm_mon + 10) % 12) + 1) -
                         0.2);
              int week_day = (((1 + m + y + y4 + c4 - 2 * c) % 7) + 7) % 7;
              /* weekdays  */
              yk__nk_layout_row_dynamic(ctx, 35, 7);
              for (i = 0; i < (int) YK__NK_LEN(week_days); ++i)
                yk__nk_label(ctx, week_days[i], YK__NK_TEXT_CENTERED);
              /* days  */
              if (week_day > 0) yk__nk_spacing(ctx, week_day);
              for (i = 1; i <= days; ++i) {
                sprintf(buffer, "%d", i);
                if (yk__nk_button_label(ctx, buffer)) {
                  sel_date.tm_mday = i;
                  yk__nk_combo_close(ctx);
                }
              }
            }
            yk__nk_combo_end(ctx);
          }
        }
        yk__nk_tree_pop(ctx);
      }
      if (yk__nk_tree_push(ctx, YK__NK_TREE_NODE, "Input", YK__NK_MINIMIZED)) {
        static const float ratio[] = {120, 150};
        static char field_buffer[64];
        static char text[9][64];
        static int text_len[9];
        static char box_buffer[512];
        static int field_len;
        static int box_len;
        yk__nk_flags active;
        yk__nk_layout_row(ctx, YK__NK_STATIC, 25, 2, ratio);
        yk__nk_label(ctx, "Default:", YK__NK_TEXT_LEFT);
        yk__nk_edit_string(ctx, YK__NK_EDIT_SIMPLE, text[0], &text_len[0], 64,
                           yk__nk_filter_default);
        yk__nk_label(ctx, "Int:", YK__NK_TEXT_LEFT);
        yk__nk_edit_string(ctx, YK__NK_EDIT_SIMPLE, text[1], &text_len[1], 64,
                           yk__nk_filter_decimal);
        yk__nk_label(ctx, "Float:", YK__NK_TEXT_LEFT);
        yk__nk_edit_string(ctx, YK__NK_EDIT_SIMPLE, text[2], &text_len[2], 64,
                           yk__nk_filter_float);
        yk__nk_label(ctx, "Hex:", YK__NK_TEXT_LEFT);
        yk__nk_edit_string(ctx, YK__NK_EDIT_SIMPLE, text[4], &text_len[4], 64,
                           yk__nk_filter_hex);
        yk__nk_label(ctx, "Octal:", YK__NK_TEXT_LEFT);
        yk__nk_edit_string(ctx, YK__NK_EDIT_SIMPLE, text[5], &text_len[5], 64,
                           yk__nk_filter_oct);
        yk__nk_label(ctx, "Binary:", YK__NK_TEXT_LEFT);
        yk__nk_edit_string(ctx, YK__NK_EDIT_SIMPLE, text[6], &text_len[6], 64,
                           yk__nk_filter_binary);
        yk__nk_label(ctx, "Password:", YK__NK_TEXT_LEFT);
        {
          int i = 0;
          int old_len = text_len[8];
          char buffer[64];
          for (i = 0; i < text_len[8]; ++i) buffer[i] = '*';
          yk__nk_edit_string(ctx, YK__NK_EDIT_FIELD, buffer, &text_len[8], 64,
                             yk__nk_filter_default);
          if (old_len < text_len[8])
            memcpy(&text[8][old_len], &buffer[old_len],
                   (yk__nk_size) (text_len[8] - old_len));
        }
        yk__nk_label(ctx, "Field:", YK__NK_TEXT_LEFT);
        yk__nk_edit_string(ctx, YK__NK_EDIT_FIELD, field_buffer, &field_len, 64,
                           yk__nk_filter_default);
        yk__nk_label(ctx, "Box:", YK__NK_TEXT_LEFT);
        yk__nk_layout_row_static(ctx, 180, 278, 1);
        yk__nk_edit_string(ctx, YK__NK_EDIT_BOX, box_buffer, &box_len, 512,
                           yk__nk_filter_default);
        yk__nk_layout_row(ctx, YK__NK_STATIC, 25, 2, ratio);
        active =
            yk__nk_edit_string(ctx, YK__NK_EDIT_FIELD | YK__NK_EDIT_SIG_ENTER,
                               text[7], &text_len[7], 64, yk__nk_filter_ascii);
        if (yk__nk_button_label(ctx, "Submit") ||
            (active & YK__NK_EDIT_COMMITED)) {
          text[7][text_len[7]] = '\n';
          text_len[7]++;
          memcpy(&box_buffer[box_len], &text[7], (yk__nk_size) text_len[7]);
          box_len += text_len[7];
          text_len[7] = 0;
        }
        yk__nk_tree_pop(ctx);
      }
      yk__nk_tree_pop(ctx);
    }
    if (yk__nk_tree_push(ctx, YK__NK_TREE_TAB, "Chart", YK__NK_MINIMIZED)) {
      /* Chart Widgets
             * This library has two different rather simple charts. The line and the
             * column chart. Both provide a simple way of visualizing values and
             * have a retained mode and immediate mode API version. For the retain
             * mode version `nk_plot` and `nk_plot_function` you either provide
             * an array or a callback to call to handle drawing the graph.
             * For the immediate mode version you start by calling `yk__nk_chart_begin`
             * and need to provide min and max values for scaling on the Y-axis.
             * and then call `yk__nk_chart_push` to push values into the chart.
             * Finally `yk__nk_chart_end` needs to be called to end the process. */
      float id = 0;
      static int col_index = -1;
      static int line_index = -1;
      float step = (2 * 3.141592654f) / 32;
      int i;
      int index = -1;
      struct yk__nk_rect bounds;
      /* line chart */
      id = 0;
      index = -1;
      yk__nk_layout_row_dynamic(ctx, 100, 1);
      bounds = yk__nk_widget_bounds(ctx);
      if (yk__nk_chart_begin(ctx, YK__NK_CHART_LINES, 32, -1.0f, 1.0f)) {
        for (i = 0; i < 32; ++i) {
          yk__nk_flags res = yk__nk_chart_push(ctx, (float) cos(id));
          if (res & YK__NK_CHART_HOVERING) index = (int) i;
          if (res & YK__NK_CHART_CLICKED) line_index = (int) i;
          id += step;
        }
        yk__nk_chart_end(ctx);
      }
      if (index != -1)
        yk__nk_tooltipf(ctx, "Value: %.2f", (float) cos((float) index * step));
      if (line_index != -1) {
        yk__nk_layout_row_dynamic(ctx, 20, 1);
        yk__nk_labelf(ctx, YK__NK_TEXT_LEFT, "Selected value: %.2f",
                      (float) cos((float) index * step));
      }
      /* column chart */
      yk__nk_layout_row_dynamic(ctx, 100, 1);
      bounds = yk__nk_widget_bounds(ctx);
      if (yk__nk_chart_begin(ctx, YK__NK_CHART_COLUMN, 32, 0.0f, 1.0f)) {
        for (i = 0; i < 32; ++i) {
          yk__nk_flags res = yk__nk_chart_push(ctx, (float) fabs(sin(id)));
          if (res & YK__NK_CHART_HOVERING) index = (int) i;
          if (res & YK__NK_CHART_CLICKED) col_index = (int) i;
          id += step;
        }
        yk__nk_chart_end(ctx);
      }
      if (index != -1)
        yk__nk_tooltipf(ctx, "Value: %.2f",
                        (float) fabs(sin(step * (float) index)));
      if (col_index != -1) {
        yk__nk_layout_row_dynamic(ctx, 20, 1);
        yk__nk_labelf(ctx, YK__NK_TEXT_LEFT, "Selected value: %.2f",
                      (float) fabs(sin(step * (float) col_index)));
      }
      /* mixed chart */
      yk__nk_layout_row_dynamic(ctx, 100, 1);
      bounds = yk__nk_widget_bounds(ctx);
      if (yk__nk_chart_begin(ctx, YK__NK_CHART_COLUMN, 32, 0.0f, 1.0f)) {
        yk__nk_chart_add_slot(ctx, YK__NK_CHART_LINES, 32, -1.0f, 1.0f);
        yk__nk_chart_add_slot(ctx, YK__NK_CHART_LINES, 32, -1.0f, 1.0f);
        for (id = 0, i = 0; i < 32; ++i) {
          yk__nk_chart_push_slot(ctx, (float) fabs(sin(id)), 0);
          yk__nk_chart_push_slot(ctx, (float) cos(id), 1);
          yk__nk_chart_push_slot(ctx, (float) sin(id), 2);
          id += step;
        }
      }
      yk__nk_chart_end(ctx);
      /* mixed colored chart */
      yk__nk_layout_row_dynamic(ctx, 100, 1);
      bounds = yk__nk_widget_bounds(ctx);
      if (yk__nk_chart_begin_colored(ctx, YK__NK_CHART_LINES,
                                     yk__nk_rgb(255, 0, 0),
                                     yk__nk_rgb(150, 0, 0), 32, 0.0f, 1.0f)) {
        yk__nk_chart_add_slot_colored(ctx, YK__NK_CHART_LINES,
                                      yk__nk_rgb(0, 0, 255),
                                      yk__nk_rgb(0, 0, 150), 32, -1.0f, 1.0f);
        yk__nk_chart_add_slot_colored(ctx, YK__NK_CHART_LINES,
                                      yk__nk_rgb(0, 255, 0),
                                      yk__nk_rgb(0, 150, 0), 32, -1.0f, 1.0f);
        for (id = 0, i = 0; i < 32; ++i) {
          yk__nk_chart_push_slot(ctx, (float) fabs(sin(id)), 0);
          yk__nk_chart_push_slot(ctx, (float) cos(id), 1);
          yk__nk_chart_push_slot(ctx, (float) sin(id), 2);
          id += step;
        }
      }
      yk__nk_chart_end(ctx);
      yk__nk_tree_pop(ctx);
    }
    if (yk__nk_tree_push(ctx, YK__NK_TREE_TAB, "Popup", YK__NK_MINIMIZED)) {
      static struct yk__nk_color color = {255, 0, 0, 255};
      static int select[4];
      static int popup_active;
      const struct yk__nk_input *in = &ctx->input;
      struct yk__nk_rect bounds;
      /* menu contextual */
      yk__nk_layout_row_static(ctx, 30, 160, 1);
      bounds = yk__nk_widget_bounds(ctx);
      yk__nk_label(ctx, "Right click me for menu", YK__NK_TEXT_LEFT);
      if (yk__nk_contextual_begin(ctx, 0, yk__nk_vec2(100, 300), bounds)) {
        static size_t prog = 40;
        static int slider = 10;
        yk__nk_layout_row_dynamic(ctx, 25, 1);
        yk__nk_checkbox_label(ctx, "Menu", &show_menu);
        yk__nk_progress(ctx, &prog, 100, YK__NK_MODIFIABLE);
        yk__nk_slider_int(ctx, 0, &slider, 16, 1);
        if (yk__nk_contextual_item_label(ctx, "About", YK__NK_TEXT_CENTERED))
          show_app_about = yk__nk_true;
        yk__nk_selectable_label(ctx, select[0] ? "Unselect" : "Select",
                                YK__NK_TEXT_LEFT, &select[0]);
        yk__nk_selectable_label(ctx, select[1] ? "Unselect" : "Select",
                                YK__NK_TEXT_LEFT, &select[1]);
        yk__nk_selectable_label(ctx, select[2] ? "Unselect" : "Select",
                                YK__NK_TEXT_LEFT, &select[2]);
        yk__nk_selectable_label(ctx, select[3] ? "Unselect" : "Select",
                                YK__NK_TEXT_LEFT, &select[3]);
        yk__nk_contextual_end(ctx);
      }
      /* color contextual */
      yk__nk_layout_row_begin(ctx, YK__NK_STATIC, 30, 2);
      yk__nk_layout_row_push(ctx, 120);
      yk__nk_label(ctx, "Right Click here:", YK__NK_TEXT_LEFT);
      yk__nk_layout_row_push(ctx, 50);
      bounds = yk__nk_widget_bounds(ctx);
      yk__nk_button_color(ctx, color);
      yk__nk_layout_row_end(ctx);
      if (yk__nk_contextual_begin(ctx, 0, yk__nk_vec2(350, 60), bounds)) {
        yk__nk_layout_row_dynamic(ctx, 30, 4);
        color.r =
            (yk__nk_byte) yk__nk_propertyi(ctx, "#r", 0, color.r, 255, 1, 1);
        color.g =
            (yk__nk_byte) yk__nk_propertyi(ctx, "#g", 0, color.g, 255, 1, 1);
        color.b =
            (yk__nk_byte) yk__nk_propertyi(ctx, "#b", 0, color.b, 255, 1, 1);
        color.a =
            (yk__nk_byte) yk__nk_propertyi(ctx, "#a", 0, color.a, 255, 1, 1);
        yk__nk_contextual_end(ctx);
      }
      /* popup */
      yk__nk_layout_row_begin(ctx, YK__NK_STATIC, 30, 2);
      yk__nk_layout_row_push(ctx, 120);
      yk__nk_label(ctx, "Popup:", YK__NK_TEXT_LEFT);
      yk__nk_layout_row_push(ctx, 50);
      if (yk__nk_button_label(ctx, "Popup")) popup_active = 1;
      yk__nk_layout_row_end(ctx);
      if (popup_active) {
        static struct yk__nk_rect s = {20, 100, 220, 90};
        if (yk__nk_popup_begin(ctx, YK__NK_POPUP_STATIC, "Error", 0, s)) {
          yk__nk_layout_row_dynamic(ctx, 25, 1);
          yk__nk_label(ctx, "A terrible error as occured", YK__NK_TEXT_LEFT);
          yk__nk_layout_row_dynamic(ctx, 25, 2);
          if (yk__nk_button_label(ctx, "OK")) {
            popup_active = 0;
            yk__nk_popup_close(ctx);
          }
          if (yk__nk_button_label(ctx, "Cancel")) {
            popup_active = 0;
            yk__nk_popup_close(ctx);
          }
          yk__nk_popup_end(ctx);
        } else
          popup_active = yk__nk_false;
      }
      /* tooltip */
      yk__nk_layout_row_static(ctx, 30, 150, 1);
      bounds = yk__nk_widget_bounds(ctx);
      yk__nk_label(ctx, "Hover me for tooltip", YK__NK_TEXT_LEFT);
      if (yk__nk_input_is_mouse_hovering_rect(in, bounds))
        yk__nk_tooltip(ctx, "This is a tooltip");
      yk__nk_tree_pop(ctx);
    }
    if (yk__nk_tree_push(ctx, YK__NK_TREE_TAB, "Layout", YK__NK_MINIMIZED)) {
      if (yk__nk_tree_push(ctx, YK__NK_TREE_NODE, "Widget", YK__NK_MINIMIZED)) {
        float ratio_two[] = {0.2f, 0.6f, 0.2f};
        float width_two[] = {100, 200, 50};
        yk__nk_layout_row_dynamic(ctx, 30, 1);
        yk__nk_label(
            ctx,
            "Dynamic fixed column layout with generated position and size:",
            YK__NK_TEXT_LEFT);
        yk__nk_layout_row_dynamic(ctx, 30, 3);
        yk__nk_button_label(ctx, "button");
        yk__nk_button_label(ctx, "button");
        yk__nk_button_label(ctx, "button");
        yk__nk_layout_row_dynamic(ctx, 30, 1);
        yk__nk_label(
            ctx, "static fixed column layout with generated position and size:",
            YK__NK_TEXT_LEFT);
        yk__nk_layout_row_static(ctx, 30, 100, 3);
        yk__nk_button_label(ctx, "button");
        yk__nk_button_label(ctx, "button");
        yk__nk_button_label(ctx, "button");
        yk__nk_layout_row_dynamic(ctx, 30, 1);
        yk__nk_label(ctx,
                     "Dynamic array-based custom column layout with generated "
                     "position and custom size:",
                     YK__NK_TEXT_LEFT);
        yk__nk_layout_row(ctx, YK__NK_DYNAMIC, 30, 3, ratio_two);
        yk__nk_button_label(ctx, "button");
        yk__nk_button_label(ctx, "button");
        yk__nk_button_label(ctx, "button");
        yk__nk_layout_row_dynamic(ctx, 30, 1);
        yk__nk_label(ctx,
                     "Static array-based custom column layout with generated "
                     "position and custom size:",
                     YK__NK_TEXT_LEFT);
        yk__nk_layout_row(ctx, YK__NK_STATIC, 30, 3, width_two);
        yk__nk_button_label(ctx, "button");
        yk__nk_button_label(ctx, "button");
        yk__nk_button_label(ctx, "button");
        yk__nk_layout_row_dynamic(ctx, 30, 1);
        yk__nk_label(ctx,
                     "Dynamic immediate mode custom column layout with "
                     "generated position and custom size:",
                     YK__NK_TEXT_LEFT);
        yk__nk_layout_row_begin(ctx, YK__NK_DYNAMIC, 30, 3);
        yk__nk_layout_row_push(ctx, 0.2f);
        yk__nk_button_label(ctx, "button");
        yk__nk_layout_row_push(ctx, 0.6f);
        yk__nk_button_label(ctx, "button");
        yk__nk_layout_row_push(ctx, 0.2f);
        yk__nk_button_label(ctx, "button");
        yk__nk_layout_row_end(ctx);
        yk__nk_layout_row_dynamic(ctx, 30, 1);
        yk__nk_label(ctx,
                     "Static immediate mode custom column layout with "
                     "generated position and custom size:",
                     YK__NK_TEXT_LEFT);
        yk__nk_layout_row_begin(ctx, YK__NK_STATIC, 30, 3);
        yk__nk_layout_row_push(ctx, 100);
        yk__nk_button_label(ctx, "button");
        yk__nk_layout_row_push(ctx, 200);
        yk__nk_button_label(ctx, "button");
        yk__nk_layout_row_push(ctx, 50);
        yk__nk_button_label(ctx, "button");
        yk__nk_layout_row_end(ctx);
        yk__nk_layout_row_dynamic(ctx, 30, 1);
        yk__nk_label(ctx,
                     "Static free space with custom position and custom size:",
                     YK__NK_TEXT_LEFT);
        yk__nk_layout_space_begin(ctx, YK__NK_STATIC, 60, 4);
        yk__nk_layout_space_push(ctx, yk__nk_rect(100, 0, 100, 30));
        yk__nk_button_label(ctx, "button");
        yk__nk_layout_space_push(ctx, yk__nk_rect(0, 15, 100, 30));
        yk__nk_button_label(ctx, "button");
        yk__nk_layout_space_push(ctx, yk__nk_rect(200, 15, 100, 30));
        yk__nk_button_label(ctx, "button");
        yk__nk_layout_space_push(ctx, yk__nk_rect(100, 30, 100, 30));
        yk__nk_button_label(ctx, "button");
        yk__nk_layout_space_end(ctx);
        yk__nk_layout_row_dynamic(ctx, 30, 1);
        yk__nk_label(ctx, "Row template:", YK__NK_TEXT_LEFT);
        yk__nk_layout_row_template_begin(ctx, 30);
        yk__nk_layout_row_template_push_dynamic(ctx);
        yk__nk_layout_row_template_push_variable(ctx, 80);
        yk__nk_layout_row_template_push_static(ctx, 80);
        yk__nk_layout_row_template_end(ctx);
        yk__nk_button_label(ctx, "button");
        yk__nk_button_label(ctx, "button");
        yk__nk_button_label(ctx, "button");
        yk__nk_tree_pop(ctx);
      }
      if (yk__nk_tree_push(ctx, YK__NK_TREE_NODE, "Group", YK__NK_MINIMIZED)) {
        static int group_titlebar = yk__nk_false;
        static int group_border = yk__nk_true;
        static int group_no_scrollbar = yk__nk_false;
        static int group_width = 320;
        static int group_height = 200;
        yk__nk_flags group_flags = 0;
        if (group_border) group_flags |= YK__NK_WINDOW_BORDER;
        if (group_no_scrollbar) group_flags |= YK__NK_WINDOW_NO_SCROLLBAR;
        if (group_titlebar) group_flags |= YK__NK_WINDOW_TITLE;
        yk__nk_layout_row_dynamic(ctx, 30, 3);
        yk__nk_checkbox_label(ctx, "Titlebar", &group_titlebar);
        yk__nk_checkbox_label(ctx, "Border", &group_border);
        yk__nk_checkbox_label(ctx, "No Scrollbar", &group_no_scrollbar);
        yk__nk_layout_row_begin(ctx, YK__NK_STATIC, 22, 3);
        yk__nk_layout_row_push(ctx, 50);
        yk__nk_label(ctx, "size:", YK__NK_TEXT_LEFT);
        yk__nk_layout_row_push(ctx, 130);
        yk__nk_property_int(ctx, "#Width:", 100, &group_width, 500, 10, 1);
        yk__nk_layout_row_push(ctx, 130);
        yk__nk_property_int(ctx, "#Height:", 100, &group_height, 500, 10, 1);
        yk__nk_layout_row_end(ctx);
        yk__nk_layout_row_static(ctx, (float) group_height, group_width, 2);
        if (yk__nk_group_begin(ctx, "Group", group_flags)) {
          int i = 0;
          static int selected[16];
          yk__nk_layout_row_static(ctx, 18, 100, 1);
          for (i = 0; i < 16; ++i)
            yk__nk_selectable_label(ctx,
                                    (selected[i]) ? "Selected" : "Unselected",
                                    YK__NK_TEXT_CENTERED, &selected[i]);
          yk__nk_group_end(ctx);
        }
        yk__nk_tree_pop(ctx);
      }
      if (yk__nk_tree_push(ctx, YK__NK_TREE_NODE, "Tree", YK__NK_MINIMIZED)) {
        static int root_selected = 0;
        int sel = root_selected;
        if (yk__nk_tree_element_push(ctx, YK__NK_TREE_NODE, "Root",
                                     YK__NK_MINIMIZED, &sel)) {
          static int selected[8];
          int i = 0, node_select = selected[0];
          if (sel != root_selected) {
            root_selected = sel;
            for (i = 0; i < 8; ++i) selected[i] = sel;
          }
          if (yk__nk_tree_element_push(ctx, YK__NK_TREE_NODE, "Node",
                                       YK__NK_MINIMIZED, &node_select)) {
            int j = 0;
            static int sel_nodes[4];
            if (node_select != selected[0]) {
              selected[0] = node_select;
              for (i = 0; i < 4; ++i) sel_nodes[i] = node_select;
            }
            yk__nk_layout_row_static(ctx, 18, 100, 1);
            for (j = 0; j < 4; ++j)
              yk__nk_selectable_symbol_label(ctx, YK__NK_SYMBOL_CIRCLE_SOLID,
                                             (sel_nodes[j]) ? "Selected"
                                                            : "Unselected",
                                             YK__NK_TEXT_RIGHT, &sel_nodes[j]);
            yk__nk_tree_element_pop(ctx);
          }
          yk__nk_layout_row_static(ctx, 18, 100, 1);
          for (i = 1; i < 8; ++i)
            yk__nk_selectable_symbol_label(ctx, YK__NK_SYMBOL_CIRCLE_SOLID,
                                           (selected[i]) ? "Selected"
                                                         : "Unselected",
                                           YK__NK_TEXT_RIGHT, &selected[i]);
          yk__nk_tree_element_pop(ctx);
        }
        yk__nk_tree_pop(ctx);
      }
      if (yk__nk_tree_push(ctx, YK__NK_TREE_NODE, "Notebook",
                           YK__NK_MINIMIZED)) {
        static int current_tab = 0;
        struct yk__nk_rect bounds;
        float step = (2 * 3.141592654f) / 32;
        enum chart_type { CHART_LINE, CHART_HISTO, CHART_MIXED };
        const char *names[] = {"Lines", "Columns", "Mixed"};
        float id = 0;
        int i;
        /* Header */
        yk__nk_style_push_vec2(ctx, &ctx->style.window.spacing,
                               yk__nk_vec2(0, 0));
        yk__nk_style_push_float(ctx, &ctx->style.button.rounding, 0);
        yk__nk_layout_row_begin(ctx, YK__NK_STATIC, 20, 3);
        for (i = 0; i < 3; ++i) {
          /* make sure button perfectly fits text */
          const struct yk__nk_user_font *f = ctx->style.font;
          float text_width = f->width(f->userdata, f->height, names[i],
                                      yk__nk_strlen(names[i]));
          float widget_width = text_width + 3 * ctx->style.button.padding.x;
          yk__nk_layout_row_push(ctx, widget_width);
          if (current_tab == i) {
            /* active tab gets highlighted */
            struct yk__nk_style_item button_color = ctx->style.button.normal;
            ctx->style.button.normal = ctx->style.button.active;
            current_tab = yk__nk_button_label(ctx, names[i]) ? i : current_tab;
            ctx->style.button.normal = button_color;
          } else
            current_tab = yk__nk_button_label(ctx, names[i]) ? i : current_tab;
        }
        yk__nk_style_pop_float(ctx);
        /* Body */
        yk__nk_layout_row_dynamic(ctx, 140, 1);
        if (yk__nk_group_begin(ctx, "Notebook", YK__NK_WINDOW_BORDER)) {
          yk__nk_style_pop_vec2(ctx);
          switch (current_tab) {
            default:
              break;
            case CHART_LINE:
              yk__nk_layout_row_dynamic(ctx, 100, 1);
              bounds = yk__nk_widget_bounds(ctx);
              if (yk__nk_chart_begin_colored(
                      ctx, YK__NK_CHART_LINES, yk__nk_rgb(255, 0, 0),
                      yk__nk_rgb(150, 0, 0), 32, 0.0f, 1.0f)) {
                yk__nk_chart_add_slot_colored(
                    ctx, YK__NK_CHART_LINES, yk__nk_rgb(0, 0, 255),
                    yk__nk_rgb(0, 0, 150), 32, -1.0f, 1.0f);
                for (i = 0, id = 0; i < 32; ++i) {
                  yk__nk_chart_push_slot(ctx, (float) fabs(sin(id)), 0);
                  yk__nk_chart_push_slot(ctx, (float) cos(id), 1);
                  id += step;
                }
              }
              yk__nk_chart_end(ctx);
              break;
            case CHART_HISTO:
              yk__nk_layout_row_dynamic(ctx, 100, 1);
              bounds = yk__nk_widget_bounds(ctx);
              if (yk__nk_chart_begin_colored(
                      ctx, YK__NK_CHART_COLUMN, yk__nk_rgb(255, 0, 0),
                      yk__nk_rgb(150, 0, 0), 32, 0.0f, 1.0f)) {
                for (i = 0, id = 0; i < 32; ++i) {
                  yk__nk_chart_push_slot(ctx, (float) fabs(sin(id)), 0);
                  id += step;
                }
              }
              yk__nk_chart_end(ctx);
              break;
            case CHART_MIXED:
              yk__nk_layout_row_dynamic(ctx, 100, 1);
              bounds = yk__nk_widget_bounds(ctx);
              if (yk__nk_chart_begin_colored(
                      ctx, YK__NK_CHART_LINES, yk__nk_rgb(255, 0, 0),
                      yk__nk_rgb(150, 0, 0), 32, 0.0f, 1.0f)) {
                yk__nk_chart_add_slot_colored(
                    ctx, YK__NK_CHART_LINES, yk__nk_rgb(0, 0, 255),
                    yk__nk_rgb(0, 0, 150), 32, -1.0f, 1.0f);
                yk__nk_chart_add_slot_colored(
                    ctx, YK__NK_CHART_COLUMN, yk__nk_rgb(0, 255, 0),
                    yk__nk_rgb(0, 150, 0), 32, 0.0f, 1.0f);
                for (i = 0, id = 0; i < 32; ++i) {
                  yk__nk_chart_push_slot(ctx, (float) fabs(sin(id)), 0);
                  yk__nk_chart_push_slot(ctx, (float) fabs(cos(id)), 1);
                  yk__nk_chart_push_slot(ctx, (float) fabs(sin(id)), 2);
                  id += step;
                }
              }
              yk__nk_chart_end(ctx);
              break;
          }
          yk__nk_group_end(ctx);
        } else
          yk__nk_style_pop_vec2(ctx);
        yk__nk_tree_pop(ctx);
      }
      if (yk__nk_tree_push(ctx, YK__NK_TREE_NODE, "Simple", YK__NK_MINIMIZED)) {
        yk__nk_layout_row_dynamic(ctx, 300, 2);
        if (yk__nk_group_begin(ctx, "Group_Without_Border", 0)) {
          int i = 0;
          char buffer[64];
          yk__nk_layout_row_static(ctx, 18, 150, 1);
          for (i = 0; i < 64; ++i) {
            sprintf(buffer, "0x%02x", i);
            yk__nk_labelf(ctx, YK__NK_TEXT_LEFT, "%s: scrollable region",
                          buffer);
          }
          yk__nk_group_end(ctx);
        }
        if (yk__nk_group_begin(ctx, "Group_With_Border",
                               YK__NK_WINDOW_BORDER)) {
          int i = 0;
          char buffer[64];
          yk__nk_layout_row_dynamic(ctx, 25, 2);
          for (i = 0; i < 64; ++i) {
            sprintf(buffer, "%08d",
                    ((((i % 7) * 10) ^ 32)) + (64 + (i % 2) * 2));
            yk__nk_button_label(ctx, buffer);
          }
          yk__nk_group_end(ctx);
        }
        yk__nk_tree_pop(ctx);
      }
      if (yk__nk_tree_push(ctx, YK__NK_TREE_NODE, "Complex",
                           YK__NK_MINIMIZED)) {
        int i;
        yk__nk_layout_space_begin(ctx, YK__NK_STATIC, 500, 64);
        yk__nk_layout_space_push(ctx, yk__nk_rect(0, 0, 150, 500));
        if (yk__nk_group_begin(ctx, "Group_left", YK__NK_WINDOW_BORDER)) {
          static int selected[32];
          yk__nk_layout_row_static(ctx, 18, 100, 1);
          for (i = 0; i < 32; ++i)
            yk__nk_selectable_label(ctx,
                                    (selected[i]) ? "Selected" : "Unselected",
                                    YK__NK_TEXT_CENTERED, &selected[i]);
          yk__nk_group_end(ctx);
        }
        yk__nk_layout_space_push(ctx, yk__nk_rect(160, 0, 150, 240));
        if (yk__nk_group_begin(ctx, "Group_top", YK__NK_WINDOW_BORDER)) {
          yk__nk_layout_row_dynamic(ctx, 25, 1);
          yk__nk_button_label(ctx, "#FFAA");
          yk__nk_button_label(ctx, "#FFBB");
          yk__nk_button_label(ctx, "#FFCC");
          yk__nk_button_label(ctx, "#FFDD");
          yk__nk_button_label(ctx, "#FFEE");
          yk__nk_button_label(ctx, "#FFFF");
          yk__nk_group_end(ctx);
        }
        yk__nk_layout_space_push(ctx, yk__nk_rect(160, 250, 150, 250));
        if (yk__nk_group_begin(ctx, "Group_buttom", YK__NK_WINDOW_BORDER)) {
          yk__nk_layout_row_dynamic(ctx, 25, 1);
          yk__nk_button_label(ctx, "#FFAA");
          yk__nk_button_label(ctx, "#FFBB");
          yk__nk_button_label(ctx, "#FFCC");
          yk__nk_button_label(ctx, "#FFDD");
          yk__nk_button_label(ctx, "#FFEE");
          yk__nk_button_label(ctx, "#FFFF");
          yk__nk_group_end(ctx);
        }
        yk__nk_layout_space_push(ctx, yk__nk_rect(320, 0, 150, 150));
        if (yk__nk_group_begin(ctx, "Group_right_top", YK__NK_WINDOW_BORDER)) {
          static int selected[4];
          yk__nk_layout_row_static(ctx, 18, 100, 1);
          for (i = 0; i < 4; ++i)
            yk__nk_selectable_label(ctx,
                                    (selected[i]) ? "Selected" : "Unselected",
                                    YK__NK_TEXT_CENTERED, &selected[i]);
          yk__nk_group_end(ctx);
        }
        yk__nk_layout_space_push(ctx, yk__nk_rect(320, 160, 150, 150));
        if (yk__nk_group_begin(ctx, "Group_right_center",
                               YK__NK_WINDOW_BORDER)) {
          static int selected[4];
          yk__nk_layout_row_static(ctx, 18, 100, 1);
          for (i = 0; i < 4; ++i)
            yk__nk_selectable_label(ctx,
                                    (selected[i]) ? "Selected" : "Unselected",
                                    YK__NK_TEXT_CENTERED, &selected[i]);
          yk__nk_group_end(ctx);
        }
        yk__nk_layout_space_push(ctx, yk__nk_rect(320, 320, 150, 150));
        if (yk__nk_group_begin(ctx, "Group_right_bottom",
                               YK__NK_WINDOW_BORDER)) {
          static int selected[4];
          yk__nk_layout_row_static(ctx, 18, 100, 1);
          for (i = 0; i < 4; ++i)
            yk__nk_selectable_label(ctx,
                                    (selected[i]) ? "Selected" : "Unselected",
                                    YK__NK_TEXT_CENTERED, &selected[i]);
          yk__nk_group_end(ctx);
        }
        yk__nk_layout_space_end(ctx);
        yk__nk_tree_pop(ctx);
      }
      if (yk__nk_tree_push(ctx, YK__NK_TREE_NODE, "Splitter",
                           YK__NK_MINIMIZED)) {
        const struct yk__nk_input *in = &ctx->input;
        yk__nk_layout_row_static(ctx, 20, 320, 1);
        yk__nk_label(ctx, "Use slider and spinner to change tile size",
                     YK__NK_TEXT_LEFT);
        yk__nk_label(ctx, "Drag the space between tiles to change tile ratio",
                     YK__NK_TEXT_LEFT);
        if (yk__nk_tree_push(ctx, YK__NK_TREE_NODE, "Vertical",
                             YK__NK_MINIMIZED)) {
          static float a = 100, b = 100, c = 100;
          struct yk__nk_rect bounds;
          float row_layout[5];
          row_layout[0] = a;
          row_layout[1] = 8;
          row_layout[2] = b;
          row_layout[3] = 8;
          row_layout[4] = c;
          /* header */
          yk__nk_layout_row_static(ctx, 30, 100, 2);
          yk__nk_label(ctx, "left:", YK__NK_TEXT_LEFT);
          yk__nk_slider_float(ctx, 10.0f, &a, 200.0f, 10.0f);
          yk__nk_label(ctx, "middle:", YK__NK_TEXT_LEFT);
          yk__nk_slider_float(ctx, 10.0f, &b, 200.0f, 10.0f);
          yk__nk_label(ctx, "right:", YK__NK_TEXT_LEFT);
          yk__nk_slider_float(ctx, 10.0f, &c, 200.0f, 10.0f);
          /* tiles */
          yk__nk_layout_row(ctx, YK__NK_STATIC, 200, 5, row_layout);
          /* left space */
          if (yk__nk_group_begin(ctx, "left",
                                 YK__NK_WINDOW_NO_SCROLLBAR |
                                     YK__NK_WINDOW_BORDER |
                                     YK__NK_WINDOW_NO_SCROLLBAR)) {
            yk__nk_layout_row_dynamic(ctx, 25, 1);
            yk__nk_button_label(ctx, "#FFAA");
            yk__nk_button_label(ctx, "#FFBB");
            yk__nk_button_label(ctx, "#FFCC");
            yk__nk_button_label(ctx, "#FFDD");
            yk__nk_button_label(ctx, "#FFEE");
            yk__nk_button_label(ctx, "#FFFF");
            yk__nk_group_end(ctx);
          }
          /* scaler */
          bounds = yk__nk_widget_bounds(ctx);
          yk__nk_spacing(ctx, 1);
          if ((yk__nk_input_is_mouse_hovering_rect(in, bounds) ||
               yk__nk_input_is_mouse_prev_hovering_rect(in, bounds)) &&
              yk__nk_input_is_mouse_down(in, YK__NK_BUTTON_LEFT)) {
            a = row_layout[0] + in->mouse.delta.x;
            b = row_layout[2] - in->mouse.delta.x;
          }
          /* middle space */
          if (yk__nk_group_begin(ctx, "center",
                                 YK__NK_WINDOW_BORDER |
                                     YK__NK_WINDOW_NO_SCROLLBAR)) {
            yk__nk_layout_row_dynamic(ctx, 25, 1);
            yk__nk_button_label(ctx, "#FFAA");
            yk__nk_button_label(ctx, "#FFBB");
            yk__nk_button_label(ctx, "#FFCC");
            yk__nk_button_label(ctx, "#FFDD");
            yk__nk_button_label(ctx, "#FFEE");
            yk__nk_button_label(ctx, "#FFFF");
            yk__nk_group_end(ctx);
          }
          /* scaler */
          bounds = yk__nk_widget_bounds(ctx);
          yk__nk_spacing(ctx, 1);
          if ((yk__nk_input_is_mouse_hovering_rect(in, bounds) ||
               yk__nk_input_is_mouse_prev_hovering_rect(in, bounds)) &&
              yk__nk_input_is_mouse_down(in, YK__NK_BUTTON_LEFT)) {
            b = (row_layout[2] + in->mouse.delta.x);
            c = (row_layout[4] - in->mouse.delta.x);
          }
          /* right space */
          if (yk__nk_group_begin(ctx, "right",
                                 YK__NK_WINDOW_BORDER |
                                     YK__NK_WINDOW_NO_SCROLLBAR)) {
            yk__nk_layout_row_dynamic(ctx, 25, 1);
            yk__nk_button_label(ctx, "#FFAA");
            yk__nk_button_label(ctx, "#FFBB");
            yk__nk_button_label(ctx, "#FFCC");
            yk__nk_button_label(ctx, "#FFDD");
            yk__nk_button_label(ctx, "#FFEE");
            yk__nk_button_label(ctx, "#FFFF");
            yk__nk_group_end(ctx);
          }
          yk__nk_tree_pop(ctx);
        }
        if (yk__nk_tree_push(ctx, YK__NK_TREE_NODE, "Horizontal",
                             YK__NK_MINIMIZED)) {
          static float a = 100, b = 100, c = 100;
          struct yk__nk_rect bounds;
          /* header */
          yk__nk_layout_row_static(ctx, 30, 100, 2);
          yk__nk_label(ctx, "top:", YK__NK_TEXT_LEFT);
          yk__nk_slider_float(ctx, 10.0f, &a, 200.0f, 10.0f);
          yk__nk_label(ctx, "middle:", YK__NK_TEXT_LEFT);
          yk__nk_slider_float(ctx, 10.0f, &b, 200.0f, 10.0f);
          yk__nk_label(ctx, "bottom:", YK__NK_TEXT_LEFT);
          yk__nk_slider_float(ctx, 10.0f, &c, 200.0f, 10.0f);
          /* top space */
          yk__nk_layout_row_dynamic(ctx, a, 1);
          if (yk__nk_group_begin(ctx, "top",
                                 YK__NK_WINDOW_NO_SCROLLBAR |
                                     YK__NK_WINDOW_BORDER)) {
            yk__nk_layout_row_dynamic(ctx, 25, 3);
            yk__nk_button_label(ctx, "#FFAA");
            yk__nk_button_label(ctx, "#FFBB");
            yk__nk_button_label(ctx, "#FFCC");
            yk__nk_button_label(ctx, "#FFDD");
            yk__nk_button_label(ctx, "#FFEE");
            yk__nk_button_label(ctx, "#FFFF");
            yk__nk_group_end(ctx);
          }
          /* scaler */
          yk__nk_layout_row_dynamic(ctx, 8, 1);
          bounds = yk__nk_widget_bounds(ctx);
          yk__nk_spacing(ctx, 1);
          if ((yk__nk_input_is_mouse_hovering_rect(in, bounds) ||
               yk__nk_input_is_mouse_prev_hovering_rect(in, bounds)) &&
              yk__nk_input_is_mouse_down(in, YK__NK_BUTTON_LEFT)) {
            a = a + in->mouse.delta.y;
            b = b - in->mouse.delta.y;
          }
          /* middle space */
          yk__nk_layout_row_dynamic(ctx, b, 1);
          if (yk__nk_group_begin(ctx, "middle",
                                 YK__NK_WINDOW_NO_SCROLLBAR |
                                     YK__NK_WINDOW_BORDER)) {
            yk__nk_layout_row_dynamic(ctx, 25, 3);
            yk__nk_button_label(ctx, "#FFAA");
            yk__nk_button_label(ctx, "#FFBB");
            yk__nk_button_label(ctx, "#FFCC");
            yk__nk_button_label(ctx, "#FFDD");
            yk__nk_button_label(ctx, "#FFEE");
            yk__nk_button_label(ctx, "#FFFF");
            yk__nk_group_end(ctx);
          }
          {
            /* scaler */
            yk__nk_layout_row_dynamic(ctx, 8, 1);
            bounds = yk__nk_widget_bounds(ctx);
            if ((yk__nk_input_is_mouse_hovering_rect(in, bounds) ||
                 yk__nk_input_is_mouse_prev_hovering_rect(in, bounds)) &&
                yk__nk_input_is_mouse_down(in, YK__NK_BUTTON_LEFT)) {
              b = b + in->mouse.delta.y;
              c = c - in->mouse.delta.y;
            }
          }
          /* bottom space */
          yk__nk_layout_row_dynamic(ctx, c, 1);
          if (yk__nk_group_begin(ctx, "bottom",
                                 YK__NK_WINDOW_NO_SCROLLBAR |
                                     YK__NK_WINDOW_BORDER)) {
            yk__nk_layout_row_dynamic(ctx, 25, 3);
            yk__nk_button_label(ctx, "#FFAA");
            yk__nk_button_label(ctx, "#FFBB");
            yk__nk_button_label(ctx, "#FFCC");
            yk__nk_button_label(ctx, "#FFDD");
            yk__nk_button_label(ctx, "#FFEE");
            yk__nk_button_label(ctx, "#FFFF");
            yk__nk_group_end(ctx);
          }
          yk__nk_tree_pop(ctx);
        }
        yk__nk_tree_pop(ctx);
      }
      yk__nk_tree_pop(ctx);
    }
  }
  yk__nk_end(ctx);
  return !yk__nk_window_is_closed(ctx, "Overview");
}
