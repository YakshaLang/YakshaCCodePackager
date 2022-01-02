#if defined(YK__SOKOL_IMPL) && !defined(YK__SOKOL_GLUE_IMPL)
#define YK__SOKOL_GLUE_IMPL
#endif
#ifndef YK__SOKOL_GLUE_INCLUDED
/*
    sokol_glue.h -- glue helper functions for yk__sokol headers

    Project URL: https://github.com/floooh/yk__sokol

    Do this:
        #define YK__SOKOL_IMPL or
        #define YK__SOKOL_GLUE_IMPL
    before you include this file in *one* C or C++ file to create the
    implementation.

    ...optionally provide the following macros to override defaults:

    YK__SOKOL_ASSERT(c)     - your own assert macro (default: assert(c))
    YK__SOKOL_GLUE_API_DECL - public function declaration prefix (default: extern)
    YK__SOKOL_API_DECL      - same as YK__SOKOL_GLUE_API_DECL
    YK__SOKOL_API_IMPL      - public function implementation prefix (default: -)

    If sokol_glue.h is compiled as a DLL, define the following before
    including the declaration or implementation:

    YK__SOKOL_DLL

    On Windows, YK__SOKOL_DLL will define YK__SOKOL_GLUE_API_DECL as __declspec(dllexport)
    or __declspec(dllimport) as needed.

    OVERVIEW
    ========
    The yk__sokol core headers should not depend on each other, but sometimes
    it's useful to have a set of helper functions as "glue" between
    two or more yk__sokol headers.

    This is what sokol_glue.h is for. Simply include the header after other
    yk__sokol headers (both for the implementation and declaration), and
    depending on what headers have been included before, sokol_glue.h
    will make available "glue functions".

    PROVIDED FUNCTIONS
    ==================

    - if sokol_app.h and sokol_gfx.h is included:

        yk__sg_context_desc yk__sapp_sgcontext(void):

            Returns an initialized yk__sg_context_desc function initialized
            by calling sokol_app.h functions.

    LICENSE
    =======
    zlib/libpng license

    Copyright (c) 2018 Andre Weissflog

    This software is provided 'as-is', without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

        1. The origin of this software must not be misrepresented; you must not
        claim that you wrote the original software. If you use this software in a
        product, an acknowledgment in the product documentation would be
        appreciated but is not required.

        2. Altered source versions must be plainly marked as such, and must not
        be misrepresented as being the original software.

        3. This notice may not be removed or altered from any source
        distribution.
*/
#define YK__SOKOL_GLUE_INCLUDED
#if defined(YK__SOKOL_API_DECL) && !defined(YK__SOKOL_GLUE_API_DECL)
#define YK__SOKOL_GLUE_API_DECL YK__SOKOL_API_DECL
#endif
#ifndef YK__SOKOL_GLUE_API_DECL
#if defined(_WIN32) && defined(YK__SOKOL_DLL) && defined(YK__SOKOL_GLUE_IMPL)
#define YK__SOKOL_GLUE_API_DECL __declspec(dllexport)
#elif defined(_WIN32) && defined(YK__SOKOL_DLL)
#define YK__SOKOL_GLUE_API_DECL __declspec(dllimport)
#else
#define YK__SOKOL_GLUE_API_DECL extern
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
#if defined(YK__SOKOL_GFX_INCLUDED) && defined(YK__SOKOL_APP_INCLUDED)
YK__SOKOL_GLUE_API_DECL yk__sg_context_desc yk__sapp_sgcontext(void);
#endif
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* YK__SOKOL_GLUE_INCLUDED */
/*-- IMPLEMENTATION ----------------------------------------------------------*/
#ifdef YK__SOKOL_GLUE_IMPL
#define YK__SOKOL_GLUE_IMPL_INCLUDED (1)
#include <string.h> /* memset */
#ifndef YK__SOKOL_API_IMPL
#define YK__SOKOL_API_IMPL
#endif
#if defined(YK__SOKOL_GFX_INCLUDED) && defined(YK__SOKOL_APP_INCLUDED)
YK__SOKOL_API_IMPL yk__sg_context_desc yk__sapp_sgcontext(void) {
  yk__sg_context_desc desc;
  memset(&desc, 0, sizeof(desc));
  desc.color_format = (yk__sg_pixel_format) yk__sapp_color_format();
  desc.depth_format = (yk__sg_pixel_format) yk__sapp_depth_format();
  desc.sample_count = yk__sapp_sample_count();
  desc.gl.force_gles2 = yk__sapp_gles2();
  desc.metal.device = yk__sapp_metal_get_device();
  desc.metal.renderpass_descriptor_cb =
      yk__sapp_metal_get_renderpass_descriptor;
  desc.metal.drawable_cb = yk__sapp_metal_get_drawable;
  desc.d3d11.device = yk__sapp_d3d11_get_device();
  desc.d3d11.device_context = yk__sapp_d3d11_get_device_context();
  desc.d3d11.render_target_view_cb = yk__sapp_d3d11_get_render_target_view;
  desc.d3d11.depth_stencil_view_cb = yk__sapp_d3d11_get_depth_stencil_view;
  desc.wgpu.device = yk__sapp_wgpu_get_device();
  desc.wgpu.render_view_cb = yk__sapp_wgpu_get_render_view;
  desc.wgpu.resolve_view_cb = yk__sapp_wgpu_get_resolve_view;
  desc.wgpu.depth_stencil_view_cb = yk__sapp_wgpu_get_depth_stencil_view;
  return desc;
}
#endif
#endif /* YK__SOKOL_GLUE_IMPL */
