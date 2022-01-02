#if defined(YK__SOKOL_IMPL) && !defined(YK__SOKOL_GFX_IMPL)
#define YK__SOKOL_GFX_IMPL
#endif
#ifndef YK__SOKOL_GFX_INCLUDED
/*
    sokol_gfx.h -- simple 3D API wrapper

    Project URL: https://github.com/floooh/yk__sokol

    Example code: https://github.com/floooh/yk__sokol-samples

    Do this:
        #define YK__SOKOL_IMPL or
        #define YK__SOKOL_GFX_IMPL
    before you include this file in *one* C or C++ file to create the
    implementation.

    In the same place define one of the following to select the rendering
    backend:
        #define YK__SOKOL_GLCORE33
        #define YK__SOKOL_GLES2
        #define YK__SOKOL_GLES3
        #define YK__SOKOL_D3D11
        #define YK__SOKOL_METAL
        #define YK__SOKOL_WGPU
        #define YK__SOKOL_DUMMY_BACKEND

    I.e. for the GL 3.3 Core Profile it should look like this:

    #include ...
    #include ...
    #define YK__SOKOL_IMPL
    #define YK__SOKOL_GLCORE33
    #include "sokol_gfx.h"

    The dummy backend replaces the platform-specific backend code with empty
    stub functions. This is useful for writing tests that need to run on the
    command line.

    Optionally provide the following defines with your own implementations:

    YK__SOKOL_ASSERT(c)             - your own assert macro (default: assert(c))
    YK__SOKOL_MALLOC(s)             - your own malloc function (default: malloc(s))
    YK__SOKOL_FREE(p)               - your own free function (default: free(p))
    YK__SOKOL_LOG(yk__msg)              - your own logging function (default: puts(yk__msg))
    YK__SOKOL_UNREACHABLE()         - a guard macro for unreachable code (default: assert(false))
    YK__SOKOL_GFX_API_DECL          - public function declaration prefix (default: extern)
    YK__SOKOL_API_DECL              - same as YK__SOKOL_GFX_API_DECL
    YK__SOKOL_API_IMPL              - public function implementation prefix (default: -)
    YK__SOKOL_TRACE_HOOKS           - enable trace hook callbacks (search below for TRACE HOOKS)
    YK__SOKOL_EXTERNAL_GL_LOADER    - indicates that you're using your own GL loader, in this case
                                  sokol_gfx.h will not include any platform GL headers and disable
                                  the integrated Win32 GL loader

    If sokol_gfx.h is compiled as a DLL, define the following before
    including the declaration or implementation:

    YK__SOKOL_DLL

    On Windows, YK__SOKOL_DLL will define YK__SOKOL_GFX_API_DECL as __declspec(dllexport)
    or __declspec(dllimport) as needed.

    If you want to compile without deprecated structs and functions,
    define:

    SOKOL_NO_DEPRECATED

    Optionally define the following to force debug checks and validations
    even in release mode:

    YK__SOKOL_DEBUG         - by default this is defined if _DEBUG is defined

    sokol_gfx DOES NOT:
    ===================
    - create a window or the 3D-API context/device, you must do this
      before sokol_gfx is initialized, and pass any required information
      (like 3D device pointers) to the sokol_gfx initialization call

    - present the rendered frame, how this is done exactly usually depends
      on how the window and 3D-API context/device was created

    - provide a unified shader language, instead 3D-API-specific shader
      source-code or shader-bytecode must be provided (for the "official"
      offline shader cross-compiler, see here:
      https://github.com/floooh/yk__sokol-tools/blob/master/docs/yk__sokol-shdc.md)

    STEP BY STEP
    ============
    --- to initialize sokol_gfx, after creating a window and a 3D-API
        context/device, call:

            yk__sg_setup(const yk__sg_desc*)

    --- create resource objects (at least buffers, shaders and pipelines,
        and optionally images and passes):

            yk__sg_buffer yk__sg_make_buffer(const yk__sg_buffer_desc*)
            yk__sg_image yk__sg_make_image(const yk__sg_image_desc*)
            yk__sg_shader yk__sg_make_shader(const yk__sg_shader_desc*)
            yk__sg_pipeline yk__sg_make_pipeline(const yk__sg_pipeline_desc*)
            yk__sg_pass yk__sg_make_pass(const yk__sg_pass_desc*)

    --- start rendering to the default frame buffer with:

            yk__sg_begin_default_pass(const yk__sg_pass_action* action, int width, int height)

        ...or alternatively with:

            yk__sg_begin_default_passf(const yk__sg_pass_action* action, float width, float height)

        ...which takes the framebuffer width and height as float values.

    --- or start rendering to an offscreen framebuffer with:

            yk__sg_begin_pass(yk__sg_pass pass, const yk__sg_pass_action* action)

    --- set the pipeline state for the next draw call with:

            yk__sg_apply_pipeline(yk__sg_pipeline pip)

    --- fill an yk__sg_bindings struct with the resource bindings for the next
        draw call (1..N vertex buffers, 0 or 1 index buffer, 0..N image objects
        to use as textures each on the vertex-shader- and fragment-shader-stage
        and then call

            yk__sg_apply_bindings(const yk__sg_bindings* bindings)

        to update the resource bindings

    --- optionally update shader uniform data with:

            yk__sg_apply_uniforms(yk__sg_shader_stage stage, int ub_index, const void* data, int num_bytes)

    --- kick off a draw call with:

            yk__sg_draw(int base_element, int num_elements, int num_instances)

        The yk__sg_draw() function unifies all the different ways to render primitives
        in a single call (indexed vs non-indexed rendering, and instanced vs non-instanced
        rendering). In case of indexed rendering, base_element and num_element specify
        indices in the currently bound index buffer. In case of non-indexed rendering
        base_element and num_elements specify vertices in the currently bound
        vertex-buffer(s). To perform instanced rendering, the rendering pipeline
        must be setup for instancing (see yk__sg_pipeline_desc below), a separate vertex buffer
        containing per-instance data must be bound, and the num_instances parameter
        must be > 1.

    --- finish the current rendering pass with:

            yk__sg_end_pass()

    --- when done with the current frame, call

            yk__sg_commit()

    --- at the end of your program, shutdown sokol_gfx with:

            yk__sg_shutdown()

    --- if you need to destroy resources before yk__sg_shutdown(), call:

            yk__sg_destroy_buffer(yk__sg_buffer buf)
            yk__sg_destroy_image(yk__sg_image img)
            yk__sg_destroy_shader(yk__sg_shader shd)
            yk__sg_destroy_pipeline(yk__sg_pipeline pip)
            yk__sg_destroy_pass(yk__sg_pass pass)

    --- to set a new viewport rectangle, call

            yk__sg_apply_viewport(int x, int y, int width, int height, bool origin_top_left)

        ...or if you want to specifiy the viewport rectangle with float values:

            yk__sg_apply_viewportf(float x, float y, float width, float height, bool origin_top_left)

    --- to set a new scissor rect, call:

            yk__sg_apply_scissor_rect(int x, int y, int width, int height, bool origin_top_left)

        ...or with float values:

            yk__sg_apply_scissor_rectf(float x, float y, float width, float height, bool origin_top_left)

        Both yk__sg_apply_viewport() and yk__sg_apply_scissor_rect() must be called
        inside a rendering pass

        Note that yk__sg_begin_default_pass() and yk__sg_begin_pass() will reset both the
        viewport and scissor rectangles to cover the entire framebuffer.

    --- to update (overwrite) the content of buffer and image resources, call:

            yk__sg_update_buffer(yk__sg_buffer buf, const yk__sg_range* data)
            yk__sg_update_image(yk__sg_image img, const yk__sg_image_data* data)

        Buffers and images to be updated must have been created with
        YK__SG_USAGE_DYNAMIC or YK__SG_USAGE_STREAM

        Only one update per frame is allowed for buffer and image resources when
        using the sg_update_*() functions. The rationale is to have a simple
        countermeasure to avoid the CPU scribbling over data the GPU is currently
        using, or the CPU having to wait for the GPU

        Buffer and image updates can be partial, as long as a rendering
        operation only references the valid (updated) data in the
        buffer or image.

    --- to append a chunk of data to a buffer resource, call:

            int yk__sg_append_buffer(yk__sg_buffer buf, const yk__sg_range* data)

        The difference to yk__sg_update_buffer() is that yk__sg_append_buffer()
        can be called multiple times per frame to append new data to the
        buffer piece by piece, optionally interleaved with draw calls referencing
        the previously written data.

        yk__sg_append_buffer() returns a byte offset to the start of the
        written data, this offset can be assigned to
        yk__sg_bindings.vertex_buffer_offsets[n] or
        yk__sg_bindings.index_buffer_offset

        Code example:

        for (...) {
            const void* data = ...;
            const int num_bytes = ...;
            int offset = yk__sg_append_buffer(buf, &(yk__sg_range) { .ptr=data, .size=num_bytes });
            bindings.vertex_buffer_offsets[0] = offset;
            yk__sg_apply_pipeline(pip);
            yk__sg_apply_bindings(&bindings);
            yk__sg_apply_uniforms(...);
            yk__sg_draw(...);
        }

        A buffer to be used with yk__sg_append_buffer() must have been created
        with YK__SG_USAGE_DYNAMIC or YK__SG_USAGE_STREAM.

        If the application appends more data to the buffer then fits into
        the buffer, the buffer will go into the "overflow" state for the
        rest of the frame.

        Any draw calls attempting to render an overflown buffer will be
        silently dropped (in debug mode this will also result in a
        validation error).

        You can also check manually if a buffer is in overflow-state by calling

            bool yk__sg_query_buffer_overflow(yk__sg_buffer buf)

        NOTE: Due to restrictions in underlying 3D-APIs, appended chunks of
        data will be 4-byte aligned in the destination buffer. This means
        that there will be gaps in index buffers containing 16-bit indices
        when the number of indices in a call to yk__sg_append_buffer() is
        odd. This isn't a problem when each call to yk__sg_append_buffer()
        is associated with one draw call, but will be problematic when
        a single indexed draw call spans several appended chunks of indices.

    --- to check at runtime for optional features, limits and pixelformat support,
        call:

            yk__sg_features yk__sg_query_features()
            yk__sg_limits yk__sg_query_limits()
            yk__sg_pixelformat_info yk__sg_query_pixelformat(yk__sg_pixel_format fmt)

    --- if you need to call into the underlying 3D-API directly, you must call:

            yk__sg_reset_state_cache()

        ...before calling sokol_gfx functions again

    --- you can inspect the original yk__sg_desc structure handed to yk__sg_setup()
        by calling yk__sg_query_desc(). This will return an yk__sg_desc struct with
        the default values patched in instead of any zero-initialized values

    --- you can inspect various internal resource attributes via:

            yk__sg_buffer_info yk__sg_query_buffer_info(yk__sg_buffer buf)
            yk__sg_image_info yk__sg_query_image_info(yk__sg_image img)
            yk__sg_shader_info yk__sg_query_shader_info(yk__sg_shader shd)
            yk__sg_pipeline_info yk__sg_query_pipeline_info(yk__sg_pipeline pip)
            yk__sg_pass_info yk__sg_query_pass_info(yk__sg_pass pass)

        ...please note that the returned info-structs are tied quite closely
        to sokol_gfx.h internals, and may change more often than other
        public API functions and structs.

    --- you can ask at runtime what backend sokol_gfx.h has been compiled
        for, or whether the GLES3 backend had to fall back to GLES2 with:

            yk__sg_backend yk__sg_query_backend(void)

    --- you can query the default resource creation parameters through the functions

            yk__sg_buffer_desc yk__sg_query_buffer_defaults(const yk__sg_buffer_desc* desc)
            yk__sg_image_desc yk__sg_query_image_defaults(const yk__sg_image_desc* desc)
            yk__sg_shader_desc yk__sg_query_shader_defaults(const yk__sg_shader_desc* desc)
            yk__sg_pipeline_desc yk__sg_query_pipeline_defaults(const yk__sg_pipeline_desc* desc)
            yk__sg_pass_desc yk__sg_query_pass_defaults(const yk__sg_pass_desc* desc)

        These functions take a pointer to a desc structure which may contain
        zero-initialized items for default values. These zero-init values
        will be replaced with their concrete values in the returned desc
        struct.

    ON INITIALIZATION:
    ==================
    When calling yk__sg_setup(), a pointer to an yk__sg_desc struct must be provided
    which contains initialization options. These options provide two types
    of information to yk__sokol-gfx:

        (1) upper bounds and limits needed to allocate various internal
            data structures:
                - the max number of resources of each type that can
                  be alive at the same time, this is used for allocating
                  internal pools
                - the max overall size of uniform data that can be
                  updated per frame, including a worst-case alignment
                  per uniform update (this worst-case alignment is 256 bytes)
                - the max size of all dynamic resource updates (yk__sg_update_buffer,
                  yk__sg_append_buffer and yk__sg_update_image) per frame
                - the max number of entries in the texture sampler cache
                  (how many unique texture sampler can exist at the same time)
            Not all of those limit values are used by all backends, but it is
            good practice to provide them none-the-less.

        (2) 3D-API "context information" (sometimes also called "bindings"):
            sokol_gfx.h doesn't create or initialize 3D API objects which are
            closely related to the presentation layer (this includes the "rendering
            device", the swapchain, and any objects which depend on the
            swapchain). These API objects (or callback functions to obtain
            them, if those objects might change between frames), must
            be provided in a nested yk__sg_context_desc struct inside the
            yk__sg_desc struct. If sokol_gfx.h is used together with
            sokol_app.h, have a look at the sokol_glue.h header which provides
            a convenience function to get a yk__sg_context_desc struct filled out
            with context information provided by sokol_app.h

    See the documention block of the yk__sg_desc struct below for more information.

    BACKEND-SPECIFIC TOPICS:
    ========================
    --- the GL backends need to know about the internal structure of uniform
        blocks, and the texture sampler-name and -type:

            typedef struct {
                float mvp[16];      // model-view-projection matrix
                float offset0[2];   // some 2D vectors
                float offset1[2];
                float offset2[2];
            } params_t;

            // uniform block structure and texture image definition in yk__sg_shader_desc:
            yk__sg_shader_desc desc = {
                // uniform block description (size and internal structure)
                .vs.uniform_blocks[0] = {
                    .size = sizeof(params_t),
                    .uniforms = {
                        [0] = { .name="mvp", .type=YK__SG_UNIFORMTYPE_MAT4 },
                        [1] = { .name="offset0", .type=SG_UNIFORMTYPE_VEC2 },
                        ...
                    }
                },
                // one texture on the fragment-shader-stage, GLES2/WebGL needs name and image type
                .fs.images[0] = { .name="tex", .type=YK__SG_IMAGETYPE_ARRAY }
                ...
            };

    --- the Metal and D3D11 backends only need to know the size of uniform blocks,
        not their internal member structure, and they only need to know
        the type of a texture sampler, not its name:

            yk__sg_shader_desc desc = {
                .vs.uniform_blocks[0].size = sizeof(params_t),
                .fs.images[0].type = YK__SG_IMAGETYPE_ARRAY,
                ...
            };

    --- when creating a shader object, GLES2/WebGL need to know the vertex
        attribute names as used in the vertex shader:

            yk__sg_shader_desc desc = {
                .attrs = {
                    [0] = { .name="position" },
                    [1] = { .name="color1" }
                }
            };

        The vertex attribute names provided when creating a shader will be
        used later in sg_create_pipeline() for matching the vertex layout
        to vertex shader inputs.

    --- on D3D11 you need to provide a semantic name and semantic index in the
        shader description struct instead (see the D3D11 documentation on
        D3D11_INPUT_ELEMENT_DESC for details):

            yk__sg_shader_desc desc = {
                .attrs = {
                    [0] = { .sem_name="POSITION", .sem_index=0 }
                    [1] = { .sem_name="COLOR", .sem_index=1 }
                }
            };

        The provided semantic information will be used later in sg_create_pipeline()
        to match the vertex layout to vertex shader inputs.

    --- on D3D11, and when passing HLSL source code (instead of byte code) to shader
        creation, you can optionally define the shader model targets on the vertex
        stage:

            sg_shader_Desc desc = {
                .vs = {
                    ...
                    .d3d11_target = "vs_5_0"
                },
                .fs = {
                    ...
                    .d3d11_target = "ps_5_0"
                }
            };

        The default targets are "ps_4_0" and "fs_4_0". Note that those target names
        are only used when compiling shaders from source. They are ignored when
        creating a shader from bytecode.

    --- on Metal, GL 3.3 or GLES3/WebGL2, you don't need to provide an attribute
        name or semantic name, since vertex attributes can be bound by their slot index
        (this is mandatory in Metal, and optional in GL):

            yk__sg_pipeline_desc desc = {
                .layout = {
                    .attrs = {
                        [0] = { .format=YK__SG_VERTEXFORMAT_FLOAT3 },
                        [1] = { .format=YK__SG_VERTEXFORMAT_FLOAT4 }
                    }
                }
            };

    WORKING WITH CONTEXTS
    =====================
    yk__sokol-gfx allows to switch between different rendering contexts and
    associate resource objects with contexts. This is useful to
    create GL applications that render into multiple windows.

    A rendering context keeps track of all resources created while
    the context is active. When the context is destroyed, all resources
    "belonging to the context" are destroyed as well.

    A default context will be created and activated implicitly in
    yk__sg_setup(), and destroyed in yk__sg_shutdown(). So for a typical application
    which *doesn't* use multiple contexts, nothing changes, and calling
    the context functions isn't necessary.

    Three functions have been added to work with contexts:

    --- yk__sg_context yk__sg_setup_context():
        This must be called once after a GL context has been created and
        made active.

    --- void yk__sg_activate_context(yk__sg_context ctx)
        This must be called after making a different GL context active.
        Apart from 3D-API-specific actions, the call to yk__sg_activate_context()
        will internally call yk__sg_reset_state_cache().

    --- void yk__sg_discard_context(yk__sg_context ctx)
        This must be called right before a GL context is destroyed and
        will destroy all resources associated with the context (that
        have been created while the context was active) The GL context must be
        active at the time yk__sg_discard_context(yk__sg_context ctx) is called.

    Also note that resources (buffers, images, shaders and pipelines) must
    only be used or destroyed while the same GL context is active that
    was also active while the resource was created (an exception is
    resource sharing on GL, such resources can be used while
    another context is active, but must still be destroyed under
    the same context that was active during creation).

    For more information, check out the multiwindow-glfw sample:

    https://github.com/floooh/yk__sokol-samples/blob/master/glfw/multiwindow-glfw.c

    TRACE HOOKS:
    ============
    sokol_gfx.h optionally allows to install "trace hook" callbacks for
    each public API functions. When a public API function is called, and
    a trace hook callback has been installed for this function, the
    callback will be invoked with the parameters and result of the function.
    This is useful for things like debugging- and profiling-tools, or
    keeping track of resource creation and destruction.

    To use the trace hook feature:

    --- Define YK__SOKOL_TRACE_HOOKS before including the implementation.

    --- Setup an yk__sg_trace_hooks structure with your callback function
        pointers (keep all function pointers you're not interested
        in zero-initialized), optionally set the user_data member
        in the yk__sg_trace_hooks struct.

    --- Install the trace hooks by calling yk__sg_install_trace_hooks(),
        the return value of this function is another yk__sg_trace_hooks
        struct which contains the previously set of trace hooks.
        You should keep this struct around, and call those previous
        functions pointers from your own trace callbacks for proper
        chaining.

    As an example of how trace hooks are used, have a look at the
    imgui/sokol_gfx_imgui.h header which implements a realtime
    debugging UI for sokol_gfx.h on top of Dear ImGui.

    A NOTE ON PORTABLE PACKED VERTEX FORMATS:
    =========================================
    There are two things to consider when using packed
    vertex formats like UBYTE4, SHORT2, etc which need to work
    across all backends:

    - D3D11 can only convert *normalized* vertex formats to
      floating point during vertex fetch, normalized formats
      have a trailing 'N', and are "normalized" to a range
      -1.0..+1.0 (for the signed formats) or 0.0..1.0 (for the
      unsigned formats):

        - YK__SG_VERTEXFORMAT_BYTE4N
        - YK__SG_VERTEXFORMAT_UBYTE4N
        - YK__SG_VERTEXFORMAT_SHORT2N
        - YK__SG_VERTEXFORMAT_USHORT2N
        - YK__SG_VERTEXFORMAT_SHORT4N
        - YK__SG_VERTEXFORMAT_USHORT4N

      D3D11 will not convert *non-normalized* vertex formats to floating point
      vertex shader inputs, those can only be uses with the *ivecn* vertex shader
      input types when D3D11 is used as backend (GL and Metal can use both formats)

        - YK__SG_VERTEXFORMAT_BYTE4,
        - YK__SG_VERTEXFORMAT_UBYTE4
        - YK__SG_VERTEXFORMAT_SHORT2
        - YK__SG_VERTEXFORMAT_SHORT4

    - WebGL/GLES2 cannot use integer vertex shader inputs (int or ivecn)

    - YK__SG_VERTEXFORMAT_UINT10_N2 is not supported on WebGL/GLES2

    So for a vertex input layout which works on all platforms, only use the following
    vertex formats, and if needed "expand" the normalized vertex shader
    inputs in the vertex shader by multiplying with 127.0, 255.0, 32767.0 or
    65535.0:

        - YK__SG_VERTEXFORMAT_FLOAT,
        - YK__SG_VERTEXFORMAT_FLOAT2,
        - YK__SG_VERTEXFORMAT_FLOAT3,
        - YK__SG_VERTEXFORMAT_FLOAT4,
        - YK__SG_VERTEXFORMAT_BYTE4N,
        - YK__SG_VERTEXFORMAT_UBYTE4N,
        - YK__SG_VERTEXFORMAT_SHORT2N,
        - YK__SG_VERTEXFORMAT_USHORT2N
        - YK__SG_VERTEXFORMAT_SHORT4N,
        - YK__SG_VERTEXFORMAT_USHORT4N

    TODO:
    ====
    - talk about asynchronous resource creation

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
#define YK__SOKOL_GFX_INCLUDED (1)
#include <stddef.h>// size_t
#include <stdint.h>
#include <stdbool.h>
#if defined(YK__SOKOL_API_DECL) && !defined(YK__SOKOL_GFX_API_DECL)
#define YK__SOKOL_GFX_API_DECL YK__SOKOL_API_DECL
#endif
#ifndef YK__SOKOL_GFX_API_DECL
#if defined(_WIN32) && defined(YK__SOKOL_DLL) && defined(YK__SOKOL_GFX_IMPL)
#define YK__SOKOL_GFX_API_DECL __declspec(dllexport)
#elif defined(_WIN32) && defined(YK__SOKOL_DLL)
#define YK__SOKOL_GFX_API_DECL __declspec(dllimport)
#else
#define YK__SOKOL_GFX_API_DECL extern
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
/*
    Resource id typedefs:

    yk__sg_buffer:      vertex- and index-buffers
    yk__sg_image:       textures and render targets
    yk__sg_shader:      vertex- and fragment-shaders, uniform blocks
    yk__sg_pipeline:    associated shader and vertex-layouts, and render states
    yk__sg_pass:        a bundle of render targets and actions on them
    yk__sg_context:     a 'context handle' for switching between 3D-API contexts

    Instead of pointers, resource creation functions return a 32-bit
    number which uniquely identifies the resource object.

    The 32-bit resource id is split into a 16-bit pool index in the lower bits,
    and a 16-bit 'unique counter' in the upper bits. The index allows fast
    pool lookups, and combined with the unique-mask it allows to detect
    'dangling accesses' (trying to use an object which no longer exists, and
    its pool slot has been reused for a new object)

    The resource ids are wrapped into a struct so that the compiler
    can complain when the wrong resource type is used.
*/
typedef struct yk__sg_buffer {
  uint32_t id;
} yk__sg_buffer;
typedef struct yk__sg_image {
  uint32_t id;
} yk__sg_image;
typedef struct yk__sg_shader {
  uint32_t id;
} yk__sg_shader;
typedef struct yk__sg_pipeline {
  uint32_t id;
} yk__sg_pipeline;
typedef struct yk__sg_pass {
  uint32_t id;
} yk__sg_pass;
typedef struct yk__sg_context {
  uint32_t id;
} yk__sg_context;
/*
    yk__sg_range is a pointer-size-pair struct used to pass memory blobs into
    yk__sokol-gfx. When initialized from a value type (array or struct), you can
    use the YK__SG_RANGE() macro to build an yk__sg_range struct. For functions which
    take either a yk__sg_range pointer, or a (C++) yk__sg_range reference, use the
    YK__SG_RANGE_REF macro as a solution which compiles both in C and C++.
*/
typedef struct yk__sg_range {
  const void *ptr;
  size_t size;
} yk__sg_range;
// disabling this for every includer isn't great, but the warnings are also quite pointless
#if defined(_MSC_VER)
#pragma warning(                                                               \
    disable : 4221) /* /W4 only: nonstandard extension used: 'x': cannot be initialized using address of automatic variable 'y' */
#pragma warning(                                                               \
    disable : 4204) /* VS2015: nonstandard extension used: non-constant aggregate initializer */
#endif
#if defined(__cplusplus)
#define YK__SG_RANGE(x)                                                        \
  yk__sg_range { &x, sizeof(x) }
#define YK__SG_RANGE_REF(x)                                                    \
  yk__sg_range { &x, sizeof(x) }
#else
#define YK__SG_RANGE(x)                                                        \
  (yk__sg_range) { &x, sizeof(x) }
#define YK__SG_RANGE_REF(x)                                                    \
  &(yk__sg_range) { &x, sizeof(x) }
#endif
//  various compile-time constants
enum {
  YK__SG_INVALID_ID = 0,
  YK__SG_NUM_SHADER_STAGES = 2,
  YK__SG_NUM_INFLIGHT_FRAMES = 2,
  YK__SG_MAX_COLOR_ATTACHMENTS = 4,
  YK__SG_MAX_SHADERSTAGE_BUFFERS = 8,
  YK__SG_MAX_SHADERSTAGE_IMAGES = 12,
  YK__SG_MAX_SHADERSTAGE_UBS = 4,
  YK__SG_MAX_UB_MEMBERS = 16,
  YK__SG_MAX_VERTEX_ATTRIBUTES =
      16, /* NOTE: actual max vertex attrs can be less on GLES2, see yk__sg_limits! */
  YK__SG_MAX_MIPMAPS = 16,
  YK__SG_MAX_TEXTUREARRAY_LAYERS = 128
};
/*
    yk__sg_color

    An RGBA color value.
*/
typedef struct yk__sg_color {
  float r, g, b, a;
} yk__sg_color;
/*
    yk__sg_backend

    The active 3D-API backend, use the function yk__sg_query_backend()
    to get the currently active backend.

    NOTE that YK__SG_BACKEND_GLES2 will be returned if yk__sokol-gfx was
    compiled with YK__SOKOL_GLES3, but the runtime platform doesn't support
    GLES3/WebGL2 and yk__sokol-gfx had to fallback to GLES2/WebGL.
*/
typedef enum yk__sg_backend {
  YK__SG_BACKEND_GLCORE33,
  YK__SG_BACKEND_GLES2,
  YK__SG_BACKEND_GLES3,
  YK__SG_BACKEND_D3D11,
  YK__SG_BACKEND_METAL_IOS,
  YK__SG_BACKEND_METAL_MACOS,
  YK__SG_BACKEND_METAL_SIMULATOR,
  YK__SG_BACKEND_WGPU,
  YK__SG_BACKEND_DUMMY,
} yk__sg_backend;
/*
    yk__sg_pixel_format

    sokol_gfx.h basically uses the same pixel formats as WebGPU, since these
    are supported on most newer GPUs. GLES2 and WebGL only supports a much
    smaller subset of actually available pixel formats. Call
    yk__sg_query_pixelformat() to check at runtime if a pixel format supports the
    desired features.

    A pixelformat name consist of three parts:

        - components (R, RG, RGB or RGBA)
        - bit width per component (8, 16 or 32)
        - component data type:
            - unsigned normalized (no postfix)
            - signed normalized (SN postfix)
            - unsigned integer (UI postfix)
            - signed integer (SI postfix)
            - float (F postfix)

    Not all pixel formats can be used for everything, call yk__sg_query_pixelformat()
    to inspect the capabilities of a given pixelformat. The function returns
    an yk__sg_pixelformat_info struct with the following bool members:

        - sample: the pixelformat can be sampled as texture at least with
                  nearest filtering
        - filter: the pixelformat can be samples as texture with linear
                  filtering
        - render: the pixelformat can be used for render targets
        - blend:  blending is supported when using the pixelformat for
                  render targets
        - msaa:   multisample-antialiasing is supported when using the
                  pixelformat for render targets
        - depth:  the pixelformat can be used for depth-stencil attachments

    When targeting GLES2/WebGL, the only safe formats to use
    as texture are YK__SG_PIXELFORMAT_R8 and YK__SG_PIXELFORMAT_RGBA8. For rendering
    in GLES2/WebGL, only YK__SG_PIXELFORMAT_RGBA8 is safe. All other formats
    must be checked via sg_query_pixelformats().

    The default pixel format for texture images is YK__SG_PIXELFORMAT_RGBA8.

    The default pixel format for render target images is platform-dependent:
        - for Metal and D3D11 it is YK__SG_PIXELFORMAT_BGRA8
        - for GL backends it is YK__SG_PIXELFORMAT_RGBA8

    This is mainly because of the default framebuffer which is setup outside
    of sokol_gfx.h. On some backends, using BGRA for the default frame buffer
    allows more efficient frame flips. For your own offscreen-render-targets,
    use whatever renderable pixel format is convenient for you.
*/
typedef enum yk__sg_pixel_format {
  YK___SG_PIXELFORMAT_DEFAULT, /* value 0 reserved for default-init */
  YK__SG_PIXELFORMAT_NONE,
  YK__SG_PIXELFORMAT_R8,
  YK__SG_PIXELFORMAT_R8SN,
  YK__SG_PIXELFORMAT_R8UI,
  YK__SG_PIXELFORMAT_R8SI,
  YK__SG_PIXELFORMAT_R16,
  YK__SG_PIXELFORMAT_R16SN,
  YK__SG_PIXELFORMAT_R16UI,
  YK__SG_PIXELFORMAT_R16SI,
  YK__SG_PIXELFORMAT_R16F,
  YK__SG_PIXELFORMAT_RG8,
  YK__SG_PIXELFORMAT_RG8SN,
  YK__SG_PIXELFORMAT_RG8UI,
  YK__SG_PIXELFORMAT_RG8SI,
  YK__SG_PIXELFORMAT_R32UI,
  YK__SG_PIXELFORMAT_R32SI,
  YK__SG_PIXELFORMAT_R32F,
  YK__SG_PIXELFORMAT_RG16,
  YK__SG_PIXELFORMAT_RG16SN,
  YK__SG_PIXELFORMAT_RG16UI,
  YK__SG_PIXELFORMAT_RG16SI,
  YK__SG_PIXELFORMAT_RG16F,
  YK__SG_PIXELFORMAT_RGBA8,
  YK__SG_PIXELFORMAT_RGBA8SN,
  YK__SG_PIXELFORMAT_RGBA8UI,
  YK__SG_PIXELFORMAT_RGBA8SI,
  YK__SG_PIXELFORMAT_BGRA8,
  YK__SG_PIXELFORMAT_RGB10A2,
  YK__SG_PIXELFORMAT_RG11B10F,
  YK__SG_PIXELFORMAT_RG32UI,
  YK__SG_PIXELFORMAT_RG32SI,
  YK__SG_PIXELFORMAT_RG32F,
  YK__SG_PIXELFORMAT_RGBA16,
  YK__SG_PIXELFORMAT_RGBA16SN,
  YK__SG_PIXELFORMAT_RGBA16UI,
  YK__SG_PIXELFORMAT_RGBA16SI,
  YK__SG_PIXELFORMAT_RGBA16F,
  YK__SG_PIXELFORMAT_RGBA32UI,
  YK__SG_PIXELFORMAT_RGBA32SI,
  YK__SG_PIXELFORMAT_RGBA32F,
  YK__SG_PIXELFORMAT_DEPTH,
  YK__SG_PIXELFORMAT_DEPTH_STENCIL,
  YK__SG_PIXELFORMAT_BC1_RGBA,
  YK__SG_PIXELFORMAT_BC2_RGBA,
  YK__SG_PIXELFORMAT_BC3_RGBA,
  YK__SG_PIXELFORMAT_BC4_R,
  YK__SG_PIXELFORMAT_BC4_RSN,
  YK__SG_PIXELFORMAT_BC5_RG,
  YK__SG_PIXELFORMAT_BC5_RGSN,
  YK__SG_PIXELFORMAT_BC6H_RGBF,
  YK__SG_PIXELFORMAT_BC6H_RGBUF,
  YK__SG_PIXELFORMAT_BC7_RGBA,
  YK__SG_PIXELFORMAT_PVRTC_RGB_2BPP,
  YK__SG_PIXELFORMAT_PVRTC_RGB_4BPP,
  YK__SG_PIXELFORMAT_PVRTC_RGBA_2BPP,
  YK__SG_PIXELFORMAT_PVRTC_RGBA_4BPP,
  YK__SG_PIXELFORMAT_ETC2_RGB8,
  YK__SG_PIXELFORMAT_ETC2_RGB8A1,
  YK__SG_PIXELFORMAT_ETC2_RGBA8,
  YK__SG_PIXELFORMAT_ETC2_RG11,
  YK__SG_PIXELFORMAT_ETC2_RG11SN,
  YK___SG_PIXELFORMAT_NUM,
  YK___SG_PIXELFORMAT_FORCE_U32 = 0x7FFFFFFF
} yk__sg_pixel_format;
/*
    Runtime information about a pixel format, returned
    by yk__sg_query_pixelformat().
*/
typedef struct yk__sg_pixelformat_info {
  bool sample;// pixel format can be sampled in shaders
  bool filter;// pixel format can be sampled with filtering
  bool render;// pixel format can be used as render target
  bool blend; // alpha-blending is supported
  bool msaa;  // pixel format can be used as MSAA render target
  bool depth; // pixel format is a depth format
#if defined(YK__SOKOL_ZIG_BINDINGS)
  uint32_t __pad[3];
#endif
} yk__sg_pixelformat_info;
/*
    Runtime information about available optional features,
    returned by yk__sg_query_features()
*/
typedef struct yk__sg_features {
  bool instancing;     // hardware instancing supported
  bool origin_top_left;// framebuffer and texture origin is in top left corner
  bool
      multiple_render_targets;// offscreen render passes can have multiple render targets attached
  bool msaa_render_targets;// offscreen render passes support MSAA antialiasing
  bool imagetype_3d;   // creation of YK__SG_IMAGETYPE_3D images is supported
  bool imagetype_array;// creation of YK__SG_IMAGETYPE_ARRAY images is supported
  bool
      image_clamp_to_border;// border color and clamp-to-border UV-wrap mode is supported
  bool
      mrt_independent_blend_state;// multiple-render-target rendering can use per-render-target blend state
  bool
      mrt_independent_write_mask;// multiple-render-target rendering can use per-render-target color write masks
#if defined(YK__SOKOL_ZIG_BINDINGS)
  uint32_t __pad[3];
#endif
} yk__sg_features;
/*
    Runtime information about resource limits, returned by sg_query_limit()
*/
typedef struct yk__sg_limits {
  int max_image_size_2d;  // max width/height of YK__SG_IMAGETYPE_2D images
  int max_image_size_cube;// max width/height of YK__SG_IMAGETYPE_CUBE images
  int max_image_size_3d;// max width/height/depth of YK__SG_IMAGETYPE_3D images
  int max_image_size_array;// max width/height of YK__SG_IMAGETYPE_ARRAY images
  int max_image_array_layers;// max number of layers in YK__SG_IMAGETYPE_ARRAY images
  int max_vertex_attrs;// <= YK__SG_MAX_VERTEX_ATTRIBUTES or less (on some GLES2 impls)
  int gl_max_vertex_uniform_vectors;// <= GL_MAX_VERTEX_UNIFORM_VECTORS (only on GL backends)
} yk__sg_limits;
/*
    yk__sg_resource_state

    The current state of a resource in its resource pool.
    Resources start in the INITIAL state, which means the
    pool slot is unoccupied and can be allocated. When a resource is
    created, first an id is allocated, and the resource pool slot
    is set to state ALLOC. After allocation, the resource is
    initialized, which may result in the VALID or FAILED state. The
    reason why allocation and initialization are separate is because
    some resource types (e.g. buffers and images) might be asynchronously
    initialized by the user application. If a resource which is not
    in the VALID state is attempted to be used for rendering, rendering
    operations will silently be dropped.

    The special INVALID state is returned in sg_query_xxx_state() if no
    resource object exists for the provided resource id.
*/
typedef enum yk__sg_resource_state {
  YK__SG_RESOURCESTATE_INITIAL,
  YK__SG_RESOURCESTATE_ALLOC,
  YK__SG_RESOURCESTATE_VALID,
  YK__SG_RESOURCESTATE_FAILED,
  YK__SG_RESOURCESTATE_INVALID,
  YK___SG_RESOURCESTATE_FORCE_U32 = 0x7FFFFFFF
} yk__sg_resource_state;
/*
    yk__sg_usage

    A resource usage hint describing the update strategy of
    buffers and images. This is used in the yk__sg_buffer_desc.usage
    and yk__sg_image_desc.usage members when creating buffers
    and images:

    YK__SG_USAGE_IMMUTABLE:     the resource will never be updated with
                            new data, instead the content of the
                            resource must be provided on creation
    YK__SG_USAGE_DYNAMIC:       the resource will be updated infrequently
                            with new data (this could range from "once
                            after creation", to "quite often but not
                            every frame")
    YK__SG_USAGE_STREAM:        the resource will be updated each frame
                            with new content

    The rendering backends use this hint to prevent that the
    CPU needs to wait for the GPU when attempting to update
    a resource that might be currently accessed by the GPU.

    Resource content is updated with the functions yk__sg_update_buffer() or
    yk__sg_append_buffer() for buffer objects, and yk__sg_update_image() for image
    objects. For the sg_update_*() functions, only one update is allowed per
    frame and resource object, while yk__sg_append_buffer() can be called
    multiple times per frame on the same buffer. The application must update
    all data required for rendering (this means that the update data can be
    smaller than the resource size, if only a part of the overall resource
    size is used for rendering, you only need to make sure that the data that
    *is* used is valid).

    The default usage is YK__SG_USAGE_IMMUTABLE.
*/
typedef enum yk__sg_usage {
  YK___SG_USAGE_DEFAULT, /* value 0 reserved for default-init */
  YK__SG_USAGE_IMMUTABLE,
  YK__SG_USAGE_DYNAMIC,
  YK__SG_USAGE_STREAM,
  YK___SG_USAGE_NUM,
  YK___SG_USAGE_FORCE_U32 = 0x7FFFFFFF
} yk__sg_usage;
/*
    yk__sg_buffer_type

    This indicates whether a buffer contains vertex- or index-data,
    used in the yk__sg_buffer_desc.type member when creating a buffer.

    The default value is YK__SG_BUFFERTYPE_VERTEXBUFFER.
*/
typedef enum yk__sg_buffer_type {
  YK___SG_BUFFERTYPE_DEFAULT, /* value 0 reserved for default-init */
  YK__SG_BUFFERTYPE_VERTEXBUFFER,
  YK__SG_BUFFERTYPE_INDEXBUFFER,
  YK___SG_BUFFERTYPE_NUM,
  YK___SG_BUFFERTYPE_FORCE_U32 = 0x7FFFFFFF
} yk__sg_buffer_type;
/*
    yk__sg_index_type

    Indicates whether indexed rendering (fetching vertex-indices from an
    index buffer) is used, and if yes, the index data type (16- or 32-bits).
    This is used in the yk__sg_pipeline_desc.index_type member when creating a
    pipeline object.

    The default index type is YK__SG_INDEXTYPE_NONE.
*/
typedef enum yk__sg_index_type {
  YK___SG_INDEXTYPE_DEFAULT, /* value 0 reserved for default-init */
  YK__SG_INDEXTYPE_NONE,
  YK__SG_INDEXTYPE_UINT16,
  YK__SG_INDEXTYPE_UINT32,
  YK___SG_INDEXTYPE_NUM,
  YK___SG_INDEXTYPE_FORCE_U32 = 0x7FFFFFFF
} yk__sg_index_type;
/*
    yk__sg_image_type

    Indicates the basic type of an image object (2D-texture, cubemap,
    3D-texture or 2D-array-texture). 3D- and array-textures are not supported
    on the GLES2/WebGL backend (use yk__sg_query_features().imagetype_3d and
    yk__sg_query_features().imagetype_array to check for support). The image type
    is used in the yk__sg_image_desc.type member when creating an image, and
    in yk__sg_shader_image_desc when describing a shader's texture sampler binding.

    The default image type when creating an image is YK__SG_IMAGETYPE_2D.
*/
typedef enum yk__sg_image_type {
  YK___SG_IMAGETYPE_DEFAULT, /* value 0 reserved for default-init */
  YK__SG_IMAGETYPE_2D,
  YK__SG_IMAGETYPE_CUBE,
  YK__SG_IMAGETYPE_3D,
  YK__SG_IMAGETYPE_ARRAY,
  YK___SG_IMAGETYPE_NUM,
  YK___SG_IMAGETYPE_FORCE_U32 = 0x7FFFFFFF
} yk__sg_image_type;
/*
    yk__sg_sampler_type

    Indicates the basic data type of a shader's texture sampler which
    can be float , unsigned integer or signed integer. The sampler
    type is used in the yk__sg_shader_image_desc to describe the
    sampler type of a shader's texture sampler binding.

    The default sampler type is YK__SG_SAMPLERTYPE_FLOAT.
*/
typedef enum yk__sg_sampler_type {
  YK___SG_SAMPLERTYPE_DEFAULT, /* value 0 reserved for default-init */
  YK__SG_SAMPLERTYPE_FLOAT,
  YK__SG_SAMPLERTYPE_SINT,
  YK__SG_SAMPLERTYPE_UINT,
} yk__sg_sampler_type;
/*
    yk__sg_cube_face

    The cubemap faces. Use these as indices in the yk__sg_image_desc.content
    array.
*/
typedef enum yk__sg_cube_face {
  YK__SG_CUBEFACE_POS_X,
  YK__SG_CUBEFACE_NEG_X,
  YK__SG_CUBEFACE_POS_Y,
  YK__SG_CUBEFACE_NEG_Y,
  YK__SG_CUBEFACE_POS_Z,
  YK__SG_CUBEFACE_NEG_Z,
  YK__SG_CUBEFACE_NUM,
  YK___SG_CUBEFACE_FORCE_U32 = 0x7FFFFFFF
} yk__sg_cube_face;
/*
    yk__sg_shader_stage

    There are 2 shader stages: vertex- and fragment-shader-stage.
    Each shader stage consists of:

    - one slot for a shader function (provided as source- or byte-code)
    - YK__SG_MAX_SHADERSTAGE_UBS slots for uniform blocks
    - YK__SG_MAX_SHADERSTAGE_IMAGES slots for images used as textures by
      the shader function
*/
typedef enum yk__sg_shader_stage {
  YK__SG_SHADERSTAGE_VS,
  YK__SG_SHADERSTAGE_FS,
  YK___SG_SHADERSTAGE_FORCE_U32 = 0x7FFFFFFF
} yk__sg_shader_stage;
/*
    yk__sg_primitive_type

    This is the common subset of 3D primitive types supported across all 3D
    APIs. This is used in the yk__sg_pipeline_desc.primitive_type member when
    creating a pipeline object.

    The default primitive type is YK__SG_PRIMITIVETYPE_TRIANGLES.
*/
typedef enum yk__sg_primitive_type {
  YK___SG_PRIMITIVETYPE_DEFAULT, /* value 0 reserved for default-init */
  YK__SG_PRIMITIVETYPE_POINTS,
  YK__SG_PRIMITIVETYPE_LINES,
  YK__SG_PRIMITIVETYPE_LINE_STRIP,
  YK__SG_PRIMITIVETYPE_TRIANGLES,
  YK__SG_PRIMITIVETYPE_TRIANGLE_STRIP,
  YK___SG_PRIMITIVETYPE_NUM,
  YK___SG_PRIMITIVETYPE_FORCE_U32 = 0x7FFFFFFF
} yk__sg_primitive_type;
/*
    yk__sg_filter

    The filtering mode when sampling a texture image. This is
    used in the yk__sg_image_desc.min_filter and yk__sg_image_desc.mag_filter
    members when creating an image object.

    The default filter mode is YK__SG_FILTER_NEAREST.
*/
typedef enum yk__sg_filter {
  YK___SG_FILTER_DEFAULT, /* value 0 reserved for default-init */
  YK__SG_FILTER_NEAREST,
  YK__SG_FILTER_LINEAR,
  YK__SG_FILTER_NEAREST_MIPMAP_NEAREST,
  YK__SG_FILTER_NEAREST_MIPMAP_LINEAR,
  YK__SG_FILTER_LINEAR_MIPMAP_NEAREST,
  YK__SG_FILTER_LINEAR_MIPMAP_LINEAR,
  YK___SG_FILTER_NUM,
  YK___SG_FILTER_FORCE_U32 = 0x7FFFFFFF
} yk__sg_filter;
/*
    yk__sg_wrap

    The texture coordinates wrapping mode when sampling a texture
    image. This is used in the yk__sg_image_desc.wrap_u, .wrap_v
    and .wrap_w members when creating an image.

    The default wrap mode is YK__SG_WRAP_REPEAT.

    NOTE: YK__SG_WRAP_CLAMP_TO_BORDER is not supported on all backends
    and platforms. To check for support, call yk__sg_query_features()
    and check the "clamp_to_border" boolean in the returned
    yk__sg_features struct.

    Platforms which don't support YK__SG_WRAP_CLAMP_TO_BORDER will silently fall back
    to YK__SG_WRAP_CLAMP_TO_EDGE without a validation error.

    Platforms which support clamp-to-border are:

        - all desktop GL platforms
        - Metal on macOS
        - D3D11

    Platforms which do not support clamp-to-border:

        - GLES2/3 and WebGL/WebGL2
        - Metal on iOS
*/
typedef enum yk__sg_wrap {
  YK___SG_WRAP_DEFAULT, /* value 0 reserved for default-init */
  YK__SG_WRAP_REPEAT,
  YK__SG_WRAP_CLAMP_TO_EDGE,
  YK__SG_WRAP_CLAMP_TO_BORDER,
  YK__SG_WRAP_MIRRORED_REPEAT,
  YK___SG_WRAP_NUM,
  YK___SG_WRAP_FORCE_U32 = 0x7FFFFFFF
} yk__sg_wrap;
/*
    yk__sg_border_color

    The border color to use when sampling a texture, and the UV wrap
    mode is YK__SG_WRAP_CLAMP_TO_BORDER.

    The default border color is YK__SG_BORDERCOLOR_OPAQUE_BLACK
*/
typedef enum yk__sg_border_color {
  YK___SG_BORDERCOLOR_DEFAULT, /* value 0 reserved for default-init */
  YK__SG_BORDERCOLOR_TRANSPARENT_BLACK,
  YK__SG_BORDERCOLOR_OPAQUE_BLACK,
  YK__SG_BORDERCOLOR_OPAQUE_WHITE,
  YK___SG_BORDERCOLOR_NUM,
  YK___SG_BORDERCOLOR_FORCE_U32 = 0x7FFFFFFF
} yk__sg_border_color;
/*
    yk__sg_vertex_format

    The data type of a vertex component. This is used to describe
    the layout of vertex data when creating a pipeline object.
*/
typedef enum yk__sg_vertex_format {
  YK__SG_VERTEXFORMAT_INVALID,
  YK__SG_VERTEXFORMAT_FLOAT,
  YK__SG_VERTEXFORMAT_FLOAT2,
  YK__SG_VERTEXFORMAT_FLOAT3,
  YK__SG_VERTEXFORMAT_FLOAT4,
  YK__SG_VERTEXFORMAT_BYTE4,
  YK__SG_VERTEXFORMAT_BYTE4N,
  YK__SG_VERTEXFORMAT_UBYTE4,
  YK__SG_VERTEXFORMAT_UBYTE4N,
  YK__SG_VERTEXFORMAT_SHORT2,
  YK__SG_VERTEXFORMAT_SHORT2N,
  YK__SG_VERTEXFORMAT_USHORT2N,
  YK__SG_VERTEXFORMAT_SHORT4,
  YK__SG_VERTEXFORMAT_SHORT4N,
  YK__SG_VERTEXFORMAT_USHORT4N,
  YK__SG_VERTEXFORMAT_UINT10_N2,
  YK___SG_VERTEXFORMAT_NUM,
  YK___SG_VERTEXFORMAT_FORCE_U32 = 0x7FFFFFFF
} yk__sg_vertex_format;
/*
    yk__sg_vertex_step

    Defines whether the input pointer of a vertex input stream is advanced
    'per vertex' or 'per instance'. The default step-func is
    YK__SG_VERTEXSTEP_PER_VERTEX. YK__SG_VERTEXSTEP_PER_INSTANCE is used with
    instanced-rendering.

    The vertex-step is part of the vertex-layout definition
    when creating pipeline objects.
*/
typedef enum yk__sg_vertex_step {
  YK___SG_VERTEXSTEP_DEFAULT, /* value 0 reserved for default-init */
  YK__SG_VERTEXSTEP_PER_VERTEX,
  YK__SG_VERTEXSTEP_PER_INSTANCE,
  YK___SG_VERTEXSTEP_NUM,
  YK___SG_VERTEXSTEP_FORCE_U32 = 0x7FFFFFFF
} yk__sg_vertex_step;
/*
    yk__sg_uniform_type

    The data type of a uniform block member. This is used to
    describe the internal layout of uniform blocks when creating
    a shader object.
*/
typedef enum yk__sg_uniform_type {
  YK__SG_UNIFORMTYPE_INVALID,
  YK__SG_UNIFORMTYPE_FLOAT,
  YK__SG_UNIFORMTYPE_FLOAT2,
  YK__SG_UNIFORMTYPE_FLOAT3,
  YK__SG_UNIFORMTYPE_FLOAT4,
  YK__SG_UNIFORMTYPE_MAT4,
  YK___SG_UNIFORMTYPE_NUM,
  YK___SG_UNIFORMTYPE_FORCE_U32 = 0x7FFFFFFF
} yk__sg_uniform_type;
/*
    yk__sg_cull_mode

    The face-culling mode, this is used in the
    yk__sg_pipeline_desc.cull_mode member when creating a
    pipeline object.

    The default cull mode is YK__SG_CULLMODE_NONE
*/
typedef enum yk__sg_cull_mode {
  YK___SG_CULLMODE_DEFAULT, /* value 0 reserved for default-init */
  YK__SG_CULLMODE_NONE,
  YK__SG_CULLMODE_FRONT,
  YK__SG_CULLMODE_BACK,
  YK___SG_CULLMODE_NUM,
  YK___SG_CULLMODE_FORCE_U32 = 0x7FFFFFFF
} yk__sg_cull_mode;
/*
    yk__sg_face_winding

    The vertex-winding rule that determines a front-facing primitive. This
    is used in the member yk__sg_pipeline_desc.face_winding
    when creating a pipeline object.

    The default winding is YK__SG_FACEWINDING_CW (clockwise)
*/
typedef enum yk__sg_face_winding {
  YK___SG_FACEWINDING_DEFAULT, /* value 0 reserved for default-init */
  YK__SG_FACEWINDING_CCW,
  YK__SG_FACEWINDING_CW,
  YK___SG_FACEWINDING_NUM,
  YK___SG_FACEWINDING_FORCE_U32 = 0x7FFFFFFF
} yk__sg_face_winding;
/*
    yk__sg_compare_func

    The compare-function for depth- and stencil-ref tests.
    This is used when creating pipeline objects in the members:

    yk__sg_pipeline_desc
        .depth
            .compare
        .stencil
            .front.compare
            .back.compar

    The default compare func for depth- and stencil-tests is
    YK__SG_COMPAREFUNC_ALWAYS.
*/
typedef enum yk__sg_compare_func {
  YK___SG_COMPAREFUNC_DEFAULT, /* value 0 reserved for default-init */
  YK__SG_COMPAREFUNC_NEVER,
  YK__SG_COMPAREFUNC_LESS,
  YK__SG_COMPAREFUNC_EQUAL,
  YK__SG_COMPAREFUNC_LESS_EQUAL,
  YK__SG_COMPAREFUNC_GREATER,
  YK__SG_COMPAREFUNC_NOT_EQUAL,
  YK__SG_COMPAREFUNC_GREATER_EQUAL,
  YK__SG_COMPAREFUNC_ALWAYS,
  YK___SG_COMPAREFUNC_NUM,
  YK___SG_COMPAREFUNC_FORCE_U32 = 0x7FFFFFFF
} yk__sg_compare_func;
/*
    yk__sg_stencil_op

    The operation performed on a currently stored stencil-value when a
    comparison test passes or fails. This is used when creating a pipeline
    object in the members:

    yk__sg_pipeline_desc
        .stencil
            .front
                .fail_op
                .depth_fail_op
                .pass_op
            .back
                .fail_op
                .depth_fail_op
                .pass_op

    The default value is YK__SG_STENCILOP_KEEP.
*/
typedef enum yk__sg_stencil_op {
  YK___SG_STENCILOP_DEFAULT, /* value 0 reserved for default-init */
  YK__SG_STENCILOP_KEEP,
  YK__SG_STENCILOP_ZERO,
  YK__SG_STENCILOP_REPLACE,
  YK__SG_STENCILOP_INCR_CLAMP,
  YK__SG_STENCILOP_DECR_CLAMP,
  YK__SG_STENCILOP_INVERT,
  YK__SG_STENCILOP_INCR_WRAP,
  YK__SG_STENCILOP_DECR_WRAP,
  YK___SG_STENCILOP_NUM,
  YK___SG_STENCILOP_FORCE_U32 = 0x7FFFFFFF
} yk__sg_stencil_op;
/*
    yk__sg_blend_factor

    The source and destination factors in blending operations.
    This is used in the following members when creating a pipeline object:

    yk__sg_pipeline_desc
        .colors[i]
            .blend
                .src_factor_rgb
                .dst_factor_rgb
                .src_factor_alpha
                .dst_factor_alpha

    The default value is YK__SG_BLENDFACTOR_ONE for source
    factors, and YK__SG_BLENDFACTOR_ZERO for destination factors.
*/
typedef enum yk__sg_blend_factor {
  YK___SG_BLENDFACTOR_DEFAULT, /* value 0 reserved for default-init */
  YK__SG_BLENDFACTOR_ZERO,
  YK__SG_BLENDFACTOR_ONE,
  YK__SG_BLENDFACTOR_SRC_COLOR,
  YK__SG_BLENDFACTOR_ONE_MINUS_SRC_COLOR,
  YK__SG_BLENDFACTOR_SRC_ALPHA,
  YK__SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
  YK__SG_BLENDFACTOR_DST_COLOR,
  YK__SG_BLENDFACTOR_ONE_MINUS_DST_COLOR,
  YK__SG_BLENDFACTOR_DST_ALPHA,
  YK__SG_BLENDFACTOR_ONE_MINUS_DST_ALPHA,
  YK__SG_BLENDFACTOR_SRC_ALPHA_SATURATED,
  YK__SG_BLENDFACTOR_BLEND_COLOR,
  YK__SG_BLENDFACTOR_ONE_MINUS_BLEND_COLOR,
  YK__SG_BLENDFACTOR_BLEND_ALPHA,
  YK__SG_BLENDFACTOR_ONE_MINUS_BLEND_ALPHA,
  YK___SG_BLENDFACTOR_NUM,
  YK___SG_BLENDFACTOR_FORCE_U32 = 0x7FFFFFFF
} yk__sg_blend_factor;
/*
    yk__sg_blend_op

    Describes how the source and destination values are combined in the
    fragment blending operation. It is used in the following members when
    creating a pipeline object:

    yk__sg_pipeline_desc
        .colors[i]
            .blend
                .op_rgb
                .op_alpha

    The default value is YK__SG_BLENDOP_ADD.
*/
typedef enum yk__sg_blend_op {
  YK___SG_BLENDOP_DEFAULT, /* value 0 reserved for default-init */
  YK__SG_BLENDOP_ADD,
  YK__SG_BLENDOP_SUBTRACT,
  YK__SG_BLENDOP_REVERSE_SUBTRACT,
  YK___SG_BLENDOP_NUM,
  YK___SG_BLENDOP_FORCE_U32 = 0x7FFFFFFF
} yk__sg_blend_op;
/*
    yk__sg_color_mask

    Selects the active color channels when writing a fragment color to the
    framebuffer. This is used in the members
    yk__sg_pipeline_desc.colors[i].write_mask when creating a pipeline object.

    The default colormask is YK__SG_COLORMASK_RGBA (write all colors channels)

    NOTE: since the color mask value 0 is reserved for the default value
    (YK__SG_COLORMASK_RGBA), use YK__SG_COLORMASK_NONE if all color channels
    should be disabled.
*/
typedef enum yk__sg_color_mask {
  YK___SG_COLORMASK_DEFAULT = 0, /* value 0 reserved for default-init */
  YK__SG_COLORMASK_NONE = 0x10,  /* special value for 'all channels disabled */
  YK__SG_COLORMASK_R = 0x1,
  YK__SG_COLORMASK_G = 0x2,
  YK__SG_COLORMASK_RG = 0x3,
  YK__SG_COLORMASK_B = 0x4,
  YK__SG_COLORMASK_RB = 0x5,
  YK__SG_COLORMASK_GB = 0x6,
  YK__SG_COLORMASK_RGB = 0x7,
  YK__SG_COLORMASK_A = 0x8,
  YK__SG_COLORMASK_RA = 0x9,
  YK__SG_COLORMASK_GA = 0xA,
  YK__SG_COLORMASK_RGA = 0xB,
  YK__SG_COLORMASK_BA = 0xC,
  YK__SG_COLORMASK_RBA = 0xD,
  YK__SG_COLORMASK_GBA = 0xE,
  YK__SG_COLORMASK_RGBA = 0xF,
  YK___SG_COLORMASK_FORCE_U32 = 0x7FFFFFFF
} yk__sg_color_mask;
/*
    yk__sg_action

    Defines what action should be performed at the start of a render pass:

    YK__SG_ACTION_CLEAR:    clear the render target image
    YK__SG_ACTION_LOAD:     load the previous content of the render target image
    YK__SG_ACTION_DONTCARE: leave the render target image content undefined

    This is used in the yk__sg_pass_action structure.

    The default action for all pass attachments is YK__SG_ACTION_CLEAR, with the
    clear color rgba = {0.5f, 0.5f, 0.5f, 1.0f], depth=1.0 and stencil=0.

    If you want to override the default behaviour, it is important to not
    only set the clear color, but the 'action' field as well (as long as this
    is in its YK___SG_ACTION_DEFAULT, the value fields will be ignored).
*/
typedef enum yk__sg_action {
  YK___SG_ACTION_DEFAULT,
  YK__SG_ACTION_CLEAR,
  YK__SG_ACTION_LOAD,
  YK__SG_ACTION_DONTCARE,
  YK___SG_ACTION_NUM,
  YK___SG_ACTION_FORCE_U32 = 0x7FFFFFFF
} yk__sg_action;
/*
    yk__sg_pass_action

    The yk__sg_pass_action struct defines the actions to be performed
    at the start of a rendering pass in the functions yk__sg_begin_pass()
    and yk__sg_begin_default_pass().

    A separate action and clear values can be defined for each
    color attachment, and for the depth-stencil attachment.

    The default clear values are defined by the macros:

    - YK__SG_DEFAULT_CLEAR_RED:     0.5f
    - YK__SG_DEFAULT_CLEAR_GREEN:   0.5f
    - YK__SG_DEFAULT_CLEAR_BLUE:    0.5f
    - YK__SG_DEFAULT_CLEAR_ALPHA:   1.0f
    - YK__SG_DEFAULT_CLEAR_DEPTH:   1.0f
    - YK__SG_DEFAULT_CLEAR_STENCIL: 0
*/
typedef struct yk__sg_color_attachment_action {
  yk__sg_action action;
  yk__sg_color value;
} yk__sg_color_attachment_action;
typedef struct yk__sg_depth_attachment_action {
  yk__sg_action action;
  float value;
} yk__sg_depth_attachment_action;
typedef struct yk__sg_stencil_attachment_action {
  yk__sg_action action;
  uint8_t value;
} yk__sg_stencil_attachment_action;
typedef struct yk__sg_pass_action {
  uint32_t _start_canary;
  yk__sg_color_attachment_action colors[YK__SG_MAX_COLOR_ATTACHMENTS];
  yk__sg_depth_attachment_action depth;
  yk__sg_stencil_attachment_action stencil;
  uint32_t _end_canary;
} yk__sg_pass_action;
/*
    yk__sg_bindings

    The yk__sg_bindings structure defines the resource binding slots
    of the sokol_gfx render pipeline, used as argument to the
    yk__sg_apply_bindings() function.

    A resource binding struct contains:

    - 1..N vertex buffers
    - 0..N vertex buffer offsets
    - 0..1 index buffers
    - 0..1 index buffer offsets
    - 0..N vertex shader stage images
    - 0..N fragment shader stage images

    The max number of vertex buffer and shader stage images
    are defined by the YK__SG_MAX_SHADERSTAGE_BUFFERS and
    YK__SG_MAX_SHADERSTAGE_IMAGES configuration constants.

    The optional buffer offsets can be used to put different unrelated
    chunks of vertex- and/or index-data into the same buffer objects.
*/
typedef struct yk__sg_bindings {
  uint32_t _start_canary;
  yk__sg_buffer vertex_buffers[YK__SG_MAX_SHADERSTAGE_BUFFERS];
  int vertex_buffer_offsets[YK__SG_MAX_SHADERSTAGE_BUFFERS];
  yk__sg_buffer index_buffer;
  int index_buffer_offset;
  yk__sg_image vs_images[YK__SG_MAX_SHADERSTAGE_IMAGES];
  yk__sg_image fs_images[YK__SG_MAX_SHADERSTAGE_IMAGES];
  uint32_t _end_canary;
} yk__sg_bindings;
/*
    yk__sg_buffer_desc

    Creation parameters for yk__sg_buffer objects, used in the
    yk__sg_make_buffer() call.

    The default configuration is:

    .size:      0       (*must* be >0 for buffers without data)
    .type:      YK__SG_BUFFERTYPE_VERTEXBUFFER
    .usage:     YK__SG_USAGE_IMMUTABLE
    .data.ptr   0       (*must* be valid for immutable buffers)
    .data.size  0       (*must* be > 0 for immutable buffers)
    .label      0       (optional string label for trace hooks)

    The label will be ignored by sokol_gfx.h, it is only useful
    when hooking into yk__sg_make_buffer() or yk__sg_init_buffer() via
    the yk__sg_install_trace_hooks() function.

    For immutable buffers which are initialized with initial data,
    keep the .size item zero-initialized, and set the size together with the
    pointer to the initial data in the .data item.

    For mutable buffers without initial data, keep the .data item
    zero-initialized, and set the buffer size in the .size item instead.

    You can also set both size values, but currently both size values must
    be identical (this may change in the future when the dynamic resource
    management may become more flexible).

    ADVANCED TOPIC: Injecting native 3D-API buffers:

    The following struct members allow to inject your own GL, Metal
    or D3D11 buffers into sokol_gfx:

    .gl_buffers[YK__SG_NUM_INFLIGHT_FRAMES]
    .mtl_buffers[YK__SG_NUM_INFLIGHT_FRAMES]
    .d3d11_buffer

    You must still provide all other struct items except the .data item, and
    these must match the creation parameters of the native buffers you
    provide. For YK__SG_USAGE_IMMUTABLE, only provide a single native 3D-API
    buffer, otherwise you need to provide YK__SG_NUM_INFLIGHT_FRAMES buffers
    (only for GL and Metal, not D3D11). Providing multiple buffers for GL and
    Metal is necessary because sokol_gfx will rotate through them when
    calling yk__sg_update_buffer() to prevent lock-stalls.

    Note that it is expected that immutable injected buffer have already been
    initialized with content, and the .content member must be 0!

    Also you need to call yk__sg_reset_state_cache() after calling native 3D-API
    functions, and before calling any sokol_gfx function.
*/
typedef struct yk__sg_buffer_desc {
  uint32_t _start_canary;
  size_t size;
  yk__sg_buffer_type type;
  yk__sg_usage usage;
  yk__sg_range data;
  const char *label;
  /* GL specific */
  uint32_t gl_buffers[YK__SG_NUM_INFLIGHT_FRAMES];
  /* Metal specific */
  const void *mtl_buffers[YK__SG_NUM_INFLIGHT_FRAMES];
  /* D3D11 specific */
  const void *d3d11_buffer;
  /* WebGPU specific */
  const void *wgpu_buffer;
  uint32_t _end_canary;
} yk__sg_buffer_desc;
/*
    yk__sg_image_data

    Defines the content of an image through a 2D array of yk__sg_range structs.
    The first array dimension is the cubemap face, and the second array
    dimension the mipmap level.
*/
typedef struct yk__sg_image_data {
  yk__sg_range subimage[YK__SG_CUBEFACE_NUM][YK__SG_MAX_MIPMAPS];
} yk__sg_image_data;
/*
    yk__sg_image_desc

    Creation parameters for yk__sg_image objects, used in the yk__sg_make_image()
    call.

    The default configuration is:

    .type:              YK__SG_IMAGETYPE_2D
    .render_target:     false
    .width              0 (must be set to >0)
    .height             0 (must be set to >0)
    .num_slices         1 (3D textures: depth; array textures: number of layers)
    .num_mipmaps:       1
    .usage:             YK__SG_USAGE_IMMUTABLE
    .pixel_format:      YK__SG_PIXELFORMAT_RGBA8 for textures, or yk__sg_desc.context.color_format for render targets
    .sample_count:      1 for textures, or yk__sg_desc.context.sample_count for render targets
    .min_filter:        YK__SG_FILTER_NEAREST
    .mag_filter:        YK__SG_FILTER_NEAREST
    .wrap_u:            YK__SG_WRAP_REPEAT
    .wrap_v:            YK__SG_WRAP_REPEAT
    .wrap_w:            YK__SG_WRAP_REPEAT (only YK__SG_IMAGETYPE_3D)
    .border_color       YK__SG_BORDERCOLOR_OPAQUE_BLACK
    .max_anisotropy     1 (must be 1..16)
    .min_lod            0.0f
    .max_lod            FLT_MAX
    .data               an yk__sg_image_data struct to define the initial content
    .label              0       (optional string label for trace hooks)

    Q: Why is the default sample_count for render targets identical with the
    "default sample count" from yk__sg_desc.context.sample_count?

    A: So that it matches the default sample count in pipeline objects. Even
    though it is a bit strange/confusing that offscreen render targets by default
    get the same sample count as the default framebuffer, but it's better that
    an offscreen render target created with default parameters matches
    a pipeline object created with default parameters.

    NOTE:

    YK__SG_IMAGETYPE_ARRAY and YK__SG_IMAGETYPE_3D are not supported on WebGL/GLES2,
    use yk__sg_query_features().imagetype_array and
    yk__sg_query_features().imagetype_3d at runtime to check if array- and
    3D-textures are supported.

    Images with usage YK__SG_USAGE_IMMUTABLE must be fully initialized by
    providing a valid .data member which points to initialization data.

    ADVANCED TOPIC: Injecting native 3D-API textures:

    The following struct members allow to inject your own GL, Metal or D3D11
    textures into sokol_gfx:

    .gl_textures[YK__SG_NUM_INFLIGHT_FRAMES]
    .mtl_textures[YK__SG_NUM_INFLIGHT_FRAMES]
    .d3d11_texture
    .d3d11_shader_resource_view

    For GL, you can also specify the texture target or leave it empty to use
    the default texture target for the image type (GL_TEXTURE_2D for
    YK__SG_IMAGETYPE_2D etc)

    For D3D11, you can provide either a D3D11 texture, or a
    shader-resource-view, or both. If only a texture is provided, a matching
    shader-resource-view will be created. If only a shader-resource-view is
    provided, the texture will be looked up from the shader-resource-view.

    The same rules apply as for injecting native buffers (see yk__sg_buffer_desc
    documentation for more details).
*/
typedef struct yk__sg_image_desc {
  uint32_t _start_canary;
  yk__sg_image_type type;
  bool render_target;
  int width;
  int height;
  int num_slices;
  int num_mipmaps;
  yk__sg_usage usage;
  yk__sg_pixel_format pixel_format;
  int sample_count;
  yk__sg_filter min_filter;
  yk__sg_filter mag_filter;
  yk__sg_wrap wrap_u;
  yk__sg_wrap wrap_v;
  yk__sg_wrap wrap_w;
  yk__sg_border_color border_color;
  uint32_t max_anisotropy;
  float min_lod;
  float max_lod;
  yk__sg_image_data data;
  const char *label;
  /* GL specific */
  uint32_t gl_textures[YK__SG_NUM_INFLIGHT_FRAMES];
  uint32_t gl_texture_target;
  /* Metal specific */
  const void *mtl_textures[YK__SG_NUM_INFLIGHT_FRAMES];
  /* D3D11 specific */
  const void *d3d11_texture;
  const void *d3d11_shader_resource_view;
  /* WebGPU specific */
  const void *wgpu_texture;
  uint32_t _end_canary;
} yk__sg_image_desc;
/*
    yk__sg_shader_desc

    The structure yk__sg_shader_desc defines all creation parameters for shader
    programs, used as input to the yk__sg_make_shader() function:

    - reflection information for vertex attributes (vertex shader inputs):
        - vertex attribute name (required for GLES2, optional for GLES3 and GL)
        - a semantic name and index (required for D3D11)
    - for each shader-stage (vertex and fragment):
        - the shader source or bytecode
        - an optional entry function name
        - an optional compile target (only for D3D11 when source is provided,
          defaults are "vs_4_0" and "ps_4_0")
        - reflection info for each uniform block used by the shader stage:
            - the size of the uniform block in bytes
            - reflection info for each uniform block member (only required for GL backends):
                - member name
                - member type (SG_UNIFORMTYPE_xxx)
                - if the member is an array, the number of array items
        - reflection info for the texture images used by the shader stage:
            - the image type (SG_IMAGETYPE_xxx)
            - the sampler type (SG_SAMPLERTYPE_xxx, default is YK__SG_SAMPLERTYPE_FLOAT)
            - the name of the texture sampler (required for GLES2, optional everywhere else)

    For all GL backends, shader source-code must be provided. For D3D11 and Metal,
    either shader source-code or byte-code can be provided.

    For D3D11, if source code is provided, the d3dcompiler_47.dll will be loaded
    on demand. If this fails, shader creation will fail. When compiling HLSL
    source code, you can provide an optional target string via
    yk__sg_shader_stage_desc.d3d11_target, the default target is "vs_4_0" for the
    vertex shader stage and "ps_4_0" for the pixel shader stage.
*/
typedef struct yk__sg_shader_attr_desc {
  const char
      *name;// GLSL vertex attribute name (only strictly required for GLES2)
  const char *sem_name;// HLSL semantic name
  int sem_index;       // HLSL semantic index
} yk__sg_shader_attr_desc;
typedef struct yk__sg_shader_uniform_desc {
  const char *name;
  yk__sg_uniform_type type;
  int array_count;
} yk__sg_shader_uniform_desc;
typedef struct yk__sg_shader_uniform_block_desc {
  size_t size;
  yk__sg_shader_uniform_desc uniforms[YK__SG_MAX_UB_MEMBERS];
} yk__sg_shader_uniform_block_desc;
typedef struct yk__sg_shader_image_desc {
  const char *name;
  yk__sg_image_type image_type;
  yk__sg_sampler_type sampler_type;
} yk__sg_shader_image_desc;
typedef struct yk__sg_shader_stage_desc {
  const char *source;
  yk__sg_range bytecode;
  const char *entry;
  const char *d3d11_target;
  yk__sg_shader_uniform_block_desc uniform_blocks[YK__SG_MAX_SHADERSTAGE_UBS];
  yk__sg_shader_image_desc images[YK__SG_MAX_SHADERSTAGE_IMAGES];
} yk__sg_shader_stage_desc;
typedef struct yk__sg_shader_desc {
  uint32_t _start_canary;
  yk__sg_shader_attr_desc attrs[YK__SG_MAX_VERTEX_ATTRIBUTES];
  yk__sg_shader_stage_desc vs;
  yk__sg_shader_stage_desc fs;
  const char *label;
  uint32_t _end_canary;
} yk__sg_shader_desc;
/*
    yk__sg_pipeline_desc

    The yk__sg_pipeline_desc struct defines all creation parameters for an
    yk__sg_pipeline object, used as argument to the yk__sg_make_pipeline() function:

    - the vertex layout for all input vertex buffers
    - a shader object
    - the 3D primitive type (points, lines, triangles, ...)
    - the index type (none, 16- or 32-bit)
    - all the fixed-function-pipeline state (depth-, stencil-, blend-state, etc...)

    If the vertex data has no gaps between vertex components, you can omit
    the .layout.buffers[].stride and layout.attrs[].offset items (leave them
    default-initialized to 0), yk__sokol-gfx will then compute the offsets and
    strides from the vertex component formats (.layout.attrs[].format).
    Please note that ALL vertex attribute offsets must be 0 in order for the
    automatic offset computation to kick in.

    The default configuration is as follows:

    .shader:            0 (must be initialized with a valid yk__sg_shader id!)
    .layout:
        .buffers[]:         vertex buffer layouts
            .stride:        0 (if no stride is given it will be computed)
            .step_func      YK__SG_VERTEXSTEP_PER_VERTEX
            .step_rate      1
        .attrs[]:           vertex attribute declarations
            .buffer_index   0 the vertex buffer bind slot
            .offset         0 (offsets can be omitted if the vertex layout has no gaps)
            .format         YK__SG_VERTEXFORMAT_INVALID (must be initialized!)
    .depth:
        .pixel_format:      yk__sg_desc.context.depth_format
        .compare:           YK__SG_COMPAREFUNC_ALWAYS
        .write_enabled:     false
        .bias:              0.0f
        .bias_slope_scale:  0.0f
        .bias_clamp:        0.0f
    .stencil:
        .enabled:           false
        .front/back:
            .compare:       YK__SG_COMPAREFUNC_ALWAYS
            .depth_fail_op: YK__SG_STENCILOP_KEEP
            .pass_op:       YK__SG_STENCILOP_KEEP
            .compare:       YK__SG_COMPAREFUNC_ALWAYS
        .read_mask:         0
        .write_mask:        0
        .ref:               0
    .color_count            1
    .colors[0..color_count]
        .pixel_format       yk__sg_desc.context.color_format
        .write_mask:        YK__SG_COLORMASK_RGBA
        .blend:
            .enabled:           false
            .src_factor_rgb:    YK__SG_BLENDFACTOR_ONE
            .dst_factor_rgb:    YK__SG_BLENDFACTOR_ZERO
            .op_rgb:            YK__SG_BLENDOP_ADD
            .src_factor_alpha:  YK__SG_BLENDFACTOR_ONE
            .dst_factor_alpha:  YK__SG_BLENDFACTOR_ZERO
            .op_alpha:          YK__SG_BLENDOP_ADD
    .primitive_type:            YK__SG_PRIMITIVETYPE_TRIANGLES
    .index_type:                YK__SG_INDEXTYPE_NONE
    .cull_mode:                 YK__SG_CULLMODE_NONE
    .face_winding:              YK__SG_FACEWINDING_CW
    .sample_count:              yk__sg_desc.context.sample_count
    .blend_color:               (yk__sg_color) { 0.0f, 0.0f, 0.0f, 0.0f }
    .alpha_to_coverage_enabled: false
    .label  0       (optional string label for trace hooks)
*/
typedef struct yk__sg_buffer_layout_desc {
  int stride;
  yk__sg_vertex_step step_func;
  int step_rate;
#if defined(YK__SOKOL_ZIG_BINDINGS)
  uint32_t __pad[2];
#endif
} yk__sg_buffer_layout_desc;
typedef struct yk__sg_vertex_attr_desc {
  int buffer_index;
  int offset;
  yk__sg_vertex_format format;
#if defined(YK__SOKOL_ZIG_BINDINGS)
  uint32_t __pad[2];
#endif
} yk__sg_vertex_attr_desc;
typedef struct yk__sg_layout_desc {
  yk__sg_buffer_layout_desc buffers[YK__SG_MAX_SHADERSTAGE_BUFFERS];
  yk__sg_vertex_attr_desc attrs[YK__SG_MAX_VERTEX_ATTRIBUTES];
} yk__sg_layout_desc;
typedef struct yk__sg_stencil_face_state {
  yk__sg_compare_func compare;
  yk__sg_stencil_op fail_op;
  yk__sg_stencil_op depth_fail_op;
  yk__sg_stencil_op pass_op;
} yk__sg_stencil_face_state;
typedef struct yk__sg_stencil_state {
  bool enabled;
  yk__sg_stencil_face_state front;
  yk__sg_stencil_face_state back;
  uint8_t read_mask;
  uint8_t write_mask;
  uint8_t ref;
} yk__sg_stencil_state;
typedef struct yk__sg_depth_state {
  yk__sg_pixel_format pixel_format;
  yk__sg_compare_func compare;
  bool write_enabled;
  float bias;
  float bias_slope_scale;
  float bias_clamp;
} yk__sg_depth_state;
typedef struct yk__sg_blend_state {
  bool enabled;
  yk__sg_blend_factor src_factor_rgb;
  yk__sg_blend_factor dst_factor_rgb;
  yk__sg_blend_op op_rgb;
  yk__sg_blend_factor src_factor_alpha;
  yk__sg_blend_factor dst_factor_alpha;
  yk__sg_blend_op op_alpha;
} yk__sg_blend_state;
typedef struct yk__sg_color_state {
  yk__sg_pixel_format pixel_format;
  yk__sg_color_mask write_mask;
  yk__sg_blend_state blend;
} yk__sg_color_state;
typedef struct yk__sg_pipeline_desc {
  uint32_t _start_canary;
  yk__sg_shader shader;
  yk__sg_layout_desc layout;
  yk__sg_depth_state depth;
  yk__sg_stencil_state stencil;
  int color_count;
  yk__sg_color_state colors[YK__SG_MAX_COLOR_ATTACHMENTS];
  yk__sg_primitive_type primitive_type;
  yk__sg_index_type index_type;
  yk__sg_cull_mode cull_mode;
  yk__sg_face_winding face_winding;
  int sample_count;
  yk__sg_color blend_color;
  bool alpha_to_coverage_enabled;
  const char *label;
  uint32_t _end_canary;
} yk__sg_pipeline_desc;
/*
    yk__sg_pass_desc

    Creation parameters for an yk__sg_pass object, used as argument
    to the yk__sg_make_pass() function.

    A pass object contains 1..4 color-attachments and none, or one,
    depth-stencil-attachment. Each attachment consists of
    an image, and two additional indices describing
    which subimage the pass will render to: one mipmap index, and
    if the image is a cubemap, array-texture or 3D-texture, the
    face-index, array-layer or depth-slice.

    Pass images must fulfill the following requirements:

    All images must have:
    - been created as render target (yk__sg_image_desc.render_target = true)
    - the same size
    - the same sample count

    In addition, all color-attachment images must have the same pixel format.
*/
typedef struct yk__sg_pass_attachment_desc {
  yk__sg_image image;
  int mip_level;
  int slice; /* cube texture: face; array texture: layer; 3D texture: slice */
} yk__sg_pass_attachment_desc;
typedef struct yk__sg_pass_desc {
  uint32_t _start_canary;
  yk__sg_pass_attachment_desc color_attachments[YK__SG_MAX_COLOR_ATTACHMENTS];
  yk__sg_pass_attachment_desc depth_stencil_attachment;
  const char *label;
  uint32_t _end_canary;
} yk__sg_pass_desc;
/*
    yk__sg_trace_hooks

    Installable callback functions to keep track of the yk__sokol-gfx calls,
    this is useful for debugging, or keeping track of resource creation
    and destruction.

    Trace hooks are installed with yk__sg_install_trace_hooks(), this returns
    another yk__sg_trace_hooks struct with the previous set of
    trace hook function pointers. These should be invoked by the
    new trace hooks to form a proper call chain.
*/
typedef struct yk__sg_trace_hooks {
  void *user_data;
  void (*reset_state_cache)(void *user_data);
  void (*make_buffer)(const yk__sg_buffer_desc *desc, yk__sg_buffer result,
                      void *user_data);
  void (*make_image)(const yk__sg_image_desc *desc, yk__sg_image result,
                     void *user_data);
  void (*make_shader)(const yk__sg_shader_desc *desc, yk__sg_shader result,
                      void *user_data);
  void (*make_pipeline)(const yk__sg_pipeline_desc *desc,
                        yk__sg_pipeline result, void *user_data);
  void (*make_pass)(const yk__sg_pass_desc *desc, yk__sg_pass result,
                    void *user_data);
  void (*destroy_buffer)(yk__sg_buffer buf, void *user_data);
  void (*destroy_image)(yk__sg_image img, void *user_data);
  void (*destroy_shader)(yk__sg_shader shd, void *user_data);
  void (*destroy_pipeline)(yk__sg_pipeline pip, void *user_data);
  void (*destroy_pass)(yk__sg_pass pass, void *user_data);
  void (*update_buffer)(yk__sg_buffer buf, const yk__sg_range *data,
                        void *user_data);
  void (*update_image)(yk__sg_image img, const yk__sg_image_data *data,
                       void *user_data);
  void (*append_buffer)(yk__sg_buffer buf, const yk__sg_range *data, int result,
                        void *user_data);
  void (*begin_default_pass)(const yk__sg_pass_action *pass_action, int width,
                             int height, void *user_data);
  void (*begin_pass)(yk__sg_pass pass, const yk__sg_pass_action *pass_action,
                     void *user_data);
  void (*apply_viewport)(int x, int y, int width, int height,
                         bool origin_top_left, void *user_data);
  void (*apply_scissor_rect)(int x, int y, int width, int height,
                             bool origin_top_left, void *user_data);
  void (*apply_pipeline)(yk__sg_pipeline pip, void *user_data);
  void (*apply_bindings)(const yk__sg_bindings *bindings, void *user_data);
  void (*apply_uniforms)(yk__sg_shader_stage stage, int ub_index,
                         const yk__sg_range *data, void *user_data);
  void (*draw)(int base_element, int num_elements, int num_instances,
               void *user_data);
  void (*end_pass)(void *user_data);
  void (*commit)(void *user_data);
  void (*alloc_buffer)(yk__sg_buffer result, void *user_data);
  void (*alloc_image)(yk__sg_image result, void *user_data);
  void (*alloc_shader)(yk__sg_shader result, void *user_data);
  void (*alloc_pipeline)(yk__sg_pipeline result, void *user_data);
  void (*alloc_pass)(yk__sg_pass result, void *user_data);
  void (*dealloc_buffer)(yk__sg_buffer buf_id, void *user_data);
  void (*dealloc_image)(yk__sg_image img_id, void *user_data);
  void (*dealloc_shader)(yk__sg_shader shd_id, void *user_data);
  void (*dealloc_pipeline)(yk__sg_pipeline pip_id, void *user_data);
  void (*dealloc_pass)(yk__sg_pass pass_id, void *user_data);
  void (*init_buffer)(yk__sg_buffer buf_id, const yk__sg_buffer_desc *desc,
                      void *user_data);
  void (*init_image)(yk__sg_image img_id, const yk__sg_image_desc *desc,
                     void *user_data);
  void (*init_shader)(yk__sg_shader shd_id, const yk__sg_shader_desc *desc,
                      void *user_data);
  void (*init_pipeline)(yk__sg_pipeline pip_id,
                        const yk__sg_pipeline_desc *desc, void *user_data);
  void (*init_pass)(yk__sg_pass pass_id, const yk__sg_pass_desc *desc,
                    void *user_data);
  void (*uninit_buffer)(yk__sg_buffer buf_id, void *user_data);
  void (*uninit_image)(yk__sg_image img_id, void *user_data);
  void (*uninit_shader)(yk__sg_shader shd_id, void *user_data);
  void (*uninit_pipeline)(yk__sg_pipeline pip_id, void *user_data);
  void (*uninit_pass)(yk__sg_pass pass_id, void *user_data);
  void (*fail_buffer)(yk__sg_buffer buf_id, void *user_data);
  void (*fail_image)(yk__sg_image img_id, void *user_data);
  void (*fail_shader)(yk__sg_shader shd_id, void *user_data);
  void (*fail_pipeline)(yk__sg_pipeline pip_id, void *user_data);
  void (*fail_pass)(yk__sg_pass pass_id, void *user_data);
  void (*push_debug_group)(const char *name, void *user_data);
  void (*pop_debug_group)(void *user_data);
  void (*err_buffer_pool_exhausted)(void *user_data);
  void (*err_image_pool_exhausted)(void *user_data);
  void (*err_shader_pool_exhausted)(void *user_data);
  void (*err_pipeline_pool_exhausted)(void *user_data);
  void (*err_pass_pool_exhausted)(void *user_data);
  void (*err_context_mismatch)(void *user_data);
  void (*err_pass_invalid)(void *user_data);
  void (*err_draw_invalid)(void *user_data);
  void (*err_bindings_invalid)(void *user_data);
} yk__sg_trace_hooks;
/*
    yk__sg_buffer_info
    yk__sg_image_info
    yk__sg_shader_info
    yk__sg_pipeline_info
    yk__sg_pass_info

    These structs contain various internal resource attributes which
    might be useful for debug-inspection. Please don't rely on the
    actual content of those structs too much, as they are quite closely
    tied to sokol_gfx.h internals and may change more frequently than
    the other public API elements.

    The *_info structs are used as the return values of the following functions:

    yk__sg_query_buffer_info()
    yk__sg_query_image_info()
    yk__sg_query_shader_info()
    yk__sg_query_pipeline_info()
    yk__sg_query_pass_info()
*/
typedef struct yk__sg_slot_info {
  yk__sg_resource_state state; /* the current state of this resource slot */
  uint32_t res_id; /* type-neutral resource if (e.g. yk__sg_buffer.id) */
  uint32_t ctx_id; /* the context this resource belongs to */
} yk__sg_slot_info;
typedef struct yk__sg_buffer_info {
  yk__sg_slot_info slot;       /* resource pool slot info */
  uint32_t update_frame_index; /* frame index of last yk__sg_update_buffer() */
  uint32_t append_frame_index; /* frame index of last yk__sg_append_buffer() */
  int append_pos; /* current position in buffer for yk__sg_append_buffer() */
  bool
      append_overflow; /* is buffer in overflow state (due to yk__sg_append_buffer) */
  int num_slots; /* number of renaming-slots for dynamically updated buffers */
  int active_slot; /* currently active write-slot for dynamically updated buffers */
} yk__sg_buffer_info;
typedef struct yk__sg_image_info {
  yk__sg_slot_info slot;    /* resource pool slot info */
  uint32_t upd_frame_index; /* frame index of last yk__sg_update_image() */
  int num_slots; /* number of renaming-slots for dynamically updated images */
  int active_slot; /* currently active write-slot for dynamically updated images */
  int width;       /* image width */
  int height;      /* image height */
} yk__sg_image_info;
typedef struct yk__sg_shader_info {
  yk__sg_slot_info slot; /* resoure pool slot info */
} yk__sg_shader_info;
typedef struct yk__sg_pipeline_info {
  yk__sg_slot_info slot; /* resource pool slot info */
} yk__sg_pipeline_info;
typedef struct yk__sg_pass_info {
  yk__sg_slot_info slot; /* resource pool slot info */
} yk__sg_pass_info;
/*
    yk__sg_desc

    The yk__sg_desc struct contains configuration values for sokol_gfx,
    it is used as parameter to the yk__sg_setup() call.

    NOTE that all callback function pointers come in two versions, one without
    a userdata pointer, and one with a userdata pointer. You would
    either initialize one or the other depending on whether you pass data
    to your callbacks.

    FIXME: explain the various configuration options

    The default configuration is:

    .buffer_pool_size       128
    .image_pool_size        128
    .shader_pool_size       32
    .pipeline_pool_size     64
    .pass_pool_size         16
    .context_pool_size      16
    .sampler_cache_size     64
    .uniform_buffer_size    4 MB (4*1024*1024)
    .staging_buffer_size    8 MB (8*1024*1024)

    .context.color_format: default value depends on selected backend:
        all GL backends:    YK__SG_PIXELFORMAT_RGBA8
        Metal and D3D11:    YK__SG_PIXELFORMAT_BGRA8
        WGPU:               *no default* (must be queried from WGPU swapchain)
    .context.depth_format   YK__SG_PIXELFORMAT_DEPTH_STENCIL
    .context.sample_count   1

    GL specific:
        .context.gl.force_gles2
            if this is true the GL backend will act in "GLES2 fallback mode" even
            when compiled with YK__SOKOL_GLES3, this is useful to fall back
            to traditional WebGL if a browser doesn't support a WebGL2 context

    Metal specific:
        (NOTE: All Objective-C object references are transferred through
        a bridged (const void*) to sokol_gfx, which will use a unretained
        bridged cast (__bridged id<xxx>) to retrieve the Objective-C
        references back. Since the bridge cast is unretained, the caller
        must hold a strong reference to the Objective-C object for the
        duration of the sokol_gfx call!

        .context.metal.device
            a pointer to the MTLDevice object
        .context.metal.renderpass_descriptor_cb
        .context.metal_renderpass_descriptor_userdata_cb
            A C callback function to obtain the MTLRenderPassDescriptor for the
            current frame when rendering to the default framebuffer, will be called
            in yk__sg_begin_default_pass().
        .context.metal.drawable_cb
        .context.metal.drawable_userdata_cb
            a C callback function to obtain a MTLDrawable for the current
            frame when rendering to the default framebuffer, will be called in
            yk__sg_end_pass() of the default pass
        .context.metal.user_data
            optional user data pointer passed to the userdata versions of
            callback functions

    D3D11 specific:
        .context.d3d11.device
            a pointer to the ID3D11Device object, this must have been created
            before yk__sg_setup() is called
        .context.d3d11.device_context
            a pointer to the ID3D11DeviceContext object
        .context.d3d11.render_target_view_cb
        .context.d3d11.render_target_view_userdata_cb
            a C callback function to obtain a pointer to the current
            ID3D11RenderTargetView object of the default framebuffer,
            this function will be called in yk__sg_begin_pass() when rendering
            to the default framebuffer
        .context.d3d11.depth_stencil_view_cb
        .context.d3d11.depth_stencil_view_userdata_cb
            a C callback function to obtain a pointer to the current
            ID3D11DepthStencilView object of the default framebuffer,
            this function will be called in yk__sg_begin_pass() when rendering
            to the default framebuffer
        .context.metal.user_data
            optional user data pointer passed to the userdata versions of
            callback functions

    WebGPU specific:
        .context.wgpu.device
            a WGPUDevice handle
        .context.wgpu.render_format
            WGPUTextureFormat of the swap chain surface
        .context.wgpu.render_view_cb
        .context.wgpu.render_view_userdata_cb
            callback to get the current WGPUTextureView of the swapchain's
            rendering attachment (may be an MSAA surface)
        .context.wgpu.resolve_view_cb
        .context.wgpu.resolve_view_userdata_cb
            callback to get the current WGPUTextureView of the swapchain's
            MSAA-resolve-target surface, must return 0 if not MSAA rendering
        .context.wgpu.depth_stencil_view_cb
        .context.wgpu.depth_stencil_view_userdata_cb
            callback to get current default-pass depth-stencil-surface WGPUTextureView
            the pixel format of the default WGPUTextureView must be WGPUTextureFormat_Depth24Plus8
        .context.metal.user_data
            optional user data pointer passed to the userdata versions of
            callback functions

    When using sokol_gfx.h and sokol_app.h together, consider using the
    helper function yk__sapp_sgcontext() in the sokol_glue.h header to
    initialize the yk__sg_desc.context nested struct. yk__sapp_sgcontext() returns
    a completely initialized yk__sg_context_desc struct with information
    provided by sokol_app.h.
*/
typedef struct yk__sg_gl_context_desc {
  bool force_gles2;
} yk__sg_gl_context_desc;
typedef struct yk__sg_metal_context_desc {
  const void *device;
  const void *(*renderpass_descriptor_cb)(void);
  const void *(*renderpass_descriptor_userdata_cb)(void *);
  const void *(*drawable_cb)(void);
  const void *(*drawable_userdata_cb)(void *);
  void *user_data;
} yk__sg_metal_context_desc;
typedef struct yk__sg_d3d11_context_desc {
  const void *device;
  const void *device_context;
  const void *(*render_target_view_cb)(void);
  const void *(*render_target_view_userdata_cb)(void *);
  const void *(*depth_stencil_view_cb)(void);
  const void *(*depth_stencil_view_userdata_cb)(void *);
  void *user_data;
} yk__sg_d3d11_context_desc;
typedef struct yk__sg_wgpu_context_desc {
  const void *device;                  /* WGPUDevice */
  const void *(*render_view_cb)(void); /* returns WGPUTextureView */
  const void *(*render_view_userdata_cb)(void *);
  const void *(*resolve_view_cb)(void); /* returns WGPUTextureView */
  const void *(*resolve_view_userdata_cb)(void *);
  const void *(*depth_stencil_view_cb)(
      void); /* returns WGPUTextureView, must be WGPUTextureFormat_Depth24Plus8 */
  const void *(*depth_stencil_view_userdata_cb)(void *);
  void *user_data;
} yk__sg_wgpu_context_desc;
typedef struct yk__sg_context_desc {
  yk__sg_pixel_format color_format;
  yk__sg_pixel_format depth_format;
  int sample_count;
  yk__sg_gl_context_desc gl;
  yk__sg_metal_context_desc metal;
  yk__sg_d3d11_context_desc d3d11;
  yk__sg_wgpu_context_desc wgpu;
} yk__sg_context_desc;
typedef struct yk__sg_desc {
  uint32_t _start_canary;
  int buffer_pool_size;
  int image_pool_size;
  int shader_pool_size;
  int pipeline_pool_size;
  int pass_pool_size;
  int context_pool_size;
  int uniform_buffer_size;
  int staging_buffer_size;
  int sampler_cache_size;
  yk__sg_context_desc context;
  uint32_t _end_canary;
} yk__sg_desc;
/* setup and misc functions */
YK__SOKOL_GFX_API_DECL void yk__sg_setup(const yk__sg_desc *desc);
YK__SOKOL_GFX_API_DECL void yk__sg_shutdown(void);
YK__SOKOL_GFX_API_DECL bool yk__sg_isvalid(void);
YK__SOKOL_GFX_API_DECL void yk__sg_reset_state_cache(void);
YK__SOKOL_GFX_API_DECL yk__sg_trace_hooks
yk__sg_install_trace_hooks(const yk__sg_trace_hooks *trace_hooks);
YK__SOKOL_GFX_API_DECL void yk__sg_push_debug_group(const char *name);
YK__SOKOL_GFX_API_DECL void yk__sg_pop_debug_group(void);
/* resource creation, destruction and updating */
YK__SOKOL_GFX_API_DECL yk__sg_buffer
yk__sg_make_buffer(const yk__sg_buffer_desc *desc);
YK__SOKOL_GFX_API_DECL yk__sg_image
yk__sg_make_image(const yk__sg_image_desc *desc);
YK__SOKOL_GFX_API_DECL yk__sg_shader
yk__sg_make_shader(const yk__sg_shader_desc *desc);
YK__SOKOL_GFX_API_DECL yk__sg_pipeline
yk__sg_make_pipeline(const yk__sg_pipeline_desc *desc);
YK__SOKOL_GFX_API_DECL yk__sg_pass
yk__sg_make_pass(const yk__sg_pass_desc *desc);
YK__SOKOL_GFX_API_DECL void yk__sg_destroy_buffer(yk__sg_buffer buf);
YK__SOKOL_GFX_API_DECL void yk__sg_destroy_image(yk__sg_image img);
YK__SOKOL_GFX_API_DECL void yk__sg_destroy_shader(yk__sg_shader shd);
YK__SOKOL_GFX_API_DECL void yk__sg_destroy_pipeline(yk__sg_pipeline pip);
YK__SOKOL_GFX_API_DECL void yk__sg_destroy_pass(yk__sg_pass pass);
YK__SOKOL_GFX_API_DECL void yk__sg_update_buffer(yk__sg_buffer buf,
                                                 const yk__sg_range *data);
YK__SOKOL_GFX_API_DECL void yk__sg_update_image(yk__sg_image img,
                                                const yk__sg_image_data *data);
YK__SOKOL_GFX_API_DECL int yk__sg_append_buffer(yk__sg_buffer buf,
                                                const yk__sg_range *data);
YK__SOKOL_GFX_API_DECL bool yk__sg_query_buffer_overflow(yk__sg_buffer buf);
/* rendering functions */
YK__SOKOL_GFX_API_DECL void
yk__sg_begin_default_pass(const yk__sg_pass_action *pass_action, int width,
                          int height);
YK__SOKOL_GFX_API_DECL void
yk__sg_begin_default_passf(const yk__sg_pass_action *pass_action, float width,
                           float height);
YK__SOKOL_GFX_API_DECL void
yk__sg_begin_pass(yk__sg_pass pass, const yk__sg_pass_action *pass_action);
YK__SOKOL_GFX_API_DECL void yk__sg_apply_viewport(int x, int y, int width,
                                                  int height,
                                                  bool origin_top_left);
YK__SOKOL_GFX_API_DECL void yk__sg_apply_viewportf(float x, float y,
                                                   float width, float height,
                                                   bool origin_top_left);
YK__SOKOL_GFX_API_DECL void yk__sg_apply_scissor_rect(int x, int y, int width,
                                                      int height,
                                                      bool origin_top_left);
YK__SOKOL_GFX_API_DECL void yk__sg_apply_scissor_rectf(float x, float y,
                                                       float width,
                                                       float height,
                                                       bool origin_top_left);
YK__SOKOL_GFX_API_DECL void yk__sg_apply_pipeline(yk__sg_pipeline pip);
YK__SOKOL_GFX_API_DECL void
yk__sg_apply_bindings(const yk__sg_bindings *bindings);
YK__SOKOL_GFX_API_DECL void yk__sg_apply_uniforms(yk__sg_shader_stage stage,
                                                  int ub_index,
                                                  const yk__sg_range *data);
YK__SOKOL_GFX_API_DECL void yk__sg_draw(int base_element, int num_elements,
                                        int num_instances);
YK__SOKOL_GFX_API_DECL void yk__sg_end_pass(void);
YK__SOKOL_GFX_API_DECL void yk__sg_commit(void);
/* getting information */
YK__SOKOL_GFX_API_DECL yk__sg_desc yk__sg_query_desc(void);
YK__SOKOL_GFX_API_DECL yk__sg_backend yk__sg_query_backend(void);
YK__SOKOL_GFX_API_DECL yk__sg_features yk__sg_query_features(void);
YK__SOKOL_GFX_API_DECL yk__sg_limits yk__sg_query_limits(void);
YK__SOKOL_GFX_API_DECL yk__sg_pixelformat_info
yk__sg_query_pixelformat(yk__sg_pixel_format fmt);
/* get current state of a resource (INITIAL, ALLOC, VALID, FAILED, INVALID) */
YK__SOKOL_GFX_API_DECL yk__sg_resource_state
yk__sg_query_buffer_state(yk__sg_buffer buf);
YK__SOKOL_GFX_API_DECL yk__sg_resource_state
yk__sg_query_image_state(yk__sg_image img);
YK__SOKOL_GFX_API_DECL yk__sg_resource_state
yk__sg_query_shader_state(yk__sg_shader shd);
YK__SOKOL_GFX_API_DECL yk__sg_resource_state
yk__sg_query_pipeline_state(yk__sg_pipeline pip);
YK__SOKOL_GFX_API_DECL yk__sg_resource_state
yk__sg_query_pass_state(yk__sg_pass pass);
/* get runtime information about a resource */
YK__SOKOL_GFX_API_DECL yk__sg_buffer_info
yk__sg_query_buffer_info(yk__sg_buffer buf);
YK__SOKOL_GFX_API_DECL yk__sg_image_info
yk__sg_query_image_info(yk__sg_image img);
YK__SOKOL_GFX_API_DECL yk__sg_shader_info
yk__sg_query_shader_info(yk__sg_shader shd);
YK__SOKOL_GFX_API_DECL yk__sg_pipeline_info
yk__sg_query_pipeline_info(yk__sg_pipeline pip);
YK__SOKOL_GFX_API_DECL yk__sg_pass_info
yk__sg_query_pass_info(yk__sg_pass pass);
/* get resource creation desc struct with their default values replaced */
YK__SOKOL_GFX_API_DECL yk__sg_buffer_desc
yk__sg_query_buffer_defaults(const yk__sg_buffer_desc *desc);
YK__SOKOL_GFX_API_DECL yk__sg_image_desc
yk__sg_query_image_defaults(const yk__sg_image_desc *desc);
YK__SOKOL_GFX_API_DECL yk__sg_shader_desc
yk__sg_query_shader_defaults(const yk__sg_shader_desc *desc);
YK__SOKOL_GFX_API_DECL yk__sg_pipeline_desc
yk__sg_query_pipeline_defaults(const yk__sg_pipeline_desc *desc);
YK__SOKOL_GFX_API_DECL yk__sg_pass_desc
yk__sg_query_pass_defaults(const yk__sg_pass_desc *desc);
/* separate resource allocation and initialization (for async setup) */
YK__SOKOL_GFX_API_DECL yk__sg_buffer yk__sg_alloc_buffer(void);
YK__SOKOL_GFX_API_DECL yk__sg_image yk__sg_alloc_image(void);
YK__SOKOL_GFX_API_DECL yk__sg_shader yk__sg_alloc_shader(void);
YK__SOKOL_GFX_API_DECL yk__sg_pipeline yk__sg_alloc_pipeline(void);
YK__SOKOL_GFX_API_DECL yk__sg_pass yk__sg_alloc_pass(void);
YK__SOKOL_GFX_API_DECL void yk__sg_dealloc_buffer(yk__sg_buffer buf_id);
YK__SOKOL_GFX_API_DECL void yk__sg_dealloc_image(yk__sg_image img_id);
YK__SOKOL_GFX_API_DECL void yk__sg_dealloc_shader(yk__sg_shader shd_id);
YK__SOKOL_GFX_API_DECL void yk__sg_dealloc_pipeline(yk__sg_pipeline pip_id);
YK__SOKOL_GFX_API_DECL void yk__sg_dealloc_pass(yk__sg_pass pass_id);
YK__SOKOL_GFX_API_DECL void yk__sg_init_buffer(yk__sg_buffer buf_id,
                                               const yk__sg_buffer_desc *desc);
YK__SOKOL_GFX_API_DECL void yk__sg_init_image(yk__sg_image img_id,
                                              const yk__sg_image_desc *desc);
YK__SOKOL_GFX_API_DECL void yk__sg_init_shader(yk__sg_shader shd_id,
                                               const yk__sg_shader_desc *desc);
YK__SOKOL_GFX_API_DECL void
yk__sg_init_pipeline(yk__sg_pipeline pip_id, const yk__sg_pipeline_desc *desc);
YK__SOKOL_GFX_API_DECL void yk__sg_init_pass(yk__sg_pass pass_id,
                                             const yk__sg_pass_desc *desc);
YK__SOKOL_GFX_API_DECL bool yk__sg_uninit_buffer(yk__sg_buffer buf_id);
YK__SOKOL_GFX_API_DECL bool yk__sg_uninit_image(yk__sg_image img_id);
YK__SOKOL_GFX_API_DECL bool yk__sg_uninit_shader(yk__sg_shader shd_id);
YK__SOKOL_GFX_API_DECL bool yk__sg_uninit_pipeline(yk__sg_pipeline pip_id);
YK__SOKOL_GFX_API_DECL bool yk__sg_uninit_pass(yk__sg_pass pass_id);
YK__SOKOL_GFX_API_DECL void yk__sg_fail_buffer(yk__sg_buffer buf_id);
YK__SOKOL_GFX_API_DECL void yk__sg_fail_image(yk__sg_image img_id);
YK__SOKOL_GFX_API_DECL void yk__sg_fail_shader(yk__sg_shader shd_id);
YK__SOKOL_GFX_API_DECL void yk__sg_fail_pipeline(yk__sg_pipeline pip_id);
YK__SOKOL_GFX_API_DECL void yk__sg_fail_pass(yk__sg_pass pass_id);
/* rendering contexts (optional) */
YK__SOKOL_GFX_API_DECL yk__sg_context yk__sg_setup_context(void);
YK__SOKOL_GFX_API_DECL void yk__sg_activate_context(yk__sg_context ctx_id);
YK__SOKOL_GFX_API_DECL void yk__sg_discard_context(yk__sg_context ctx_id);
/* Backend-specific helper functions, these may come in handy for mixing
   yk__sokol-gfx rendering with 'native backend' rendering functions.

   This group of functions will be expanded as needed.
*/
/* D3D11: return ID3D11Device */
YK__SOKOL_GFX_API_DECL const void *yk__sg_d3d11_device(void);
/* Metal: return __bridge-casted MTLDevice */
YK__SOKOL_GFX_API_DECL const void *yk__sg_mtl_device(void);
/* Metal: return __bridge-casted MTLRenderCommandEncoder in current pass (or zero if outside pass) */
YK__SOKOL_GFX_API_DECL const void *yk__sg_mtl_render_command_encoder(void);
#ifdef __cplusplus
} /* extern "C" */
/* reference-based equivalents for c++ */
inline void yk__sg_setup(const yk__sg_desc &desc) {
  return yk__sg_setup(&desc);
}
inline yk__sg_buffer yk__sg_make_buffer(const yk__sg_buffer_desc &desc) {
  return yk__sg_make_buffer(&desc);
}
inline yk__sg_image yk__sg_make_image(const yk__sg_image_desc &desc) {
  return yk__sg_make_image(&desc);
}
inline yk__sg_shader yk__sg_make_shader(const yk__sg_shader_desc &desc) {
  return yk__sg_make_shader(&desc);
}
inline yk__sg_pipeline yk__sg_make_pipeline(const yk__sg_pipeline_desc &desc) {
  return yk__sg_make_pipeline(&desc);
}
inline yk__sg_pass yk__sg_make_pass(const yk__sg_pass_desc &desc) {
  return yk__sg_make_pass(&desc);
}
inline void yk__sg_update_image(yk__sg_image img,
                                const yk__sg_image_data &data) {
  return yk__sg_update_image(img, &data);
}
inline void yk__sg_begin_default_pass(const yk__sg_pass_action &pass_action,
                                      int width, int height) {
  return yk__sg_begin_default_pass(&pass_action, width, height);
}
inline void yk__sg_begin_default_passf(const yk__sg_pass_action &pass_action,
                                       float width, float height) {
  return yk__sg_begin_default_passf(&pass_action, width, height);
}
inline void yk__sg_begin_pass(yk__sg_pass pass,
                              const yk__sg_pass_action &pass_action) {
  return yk__sg_begin_pass(pass, &pass_action);
}
inline void yk__sg_apply_bindings(const yk__sg_bindings &bindings) {
  return yk__sg_apply_bindings(&bindings);
}
inline void yk__sg_apply_uniforms(yk__sg_shader_stage stage, int ub_index,
                                  const yk__sg_range &data) {
  return yk__sg_apply_uniforms(stage, ub_index, &data);
}
inline yk__sg_buffer_desc
yk__sg_query_buffer_defaults(const yk__sg_buffer_desc &desc) {
  return yk__sg_query_buffer_defaults(&desc);
}
inline yk__sg_image_desc
yk__sg_query_image_defaults(const yk__sg_image_desc &desc) {
  return yk__sg_query_image_defaults(&desc);
}
inline yk__sg_shader_desc
yk__sg_query_shader_defaults(const yk__sg_shader_desc &desc) {
  return yk__sg_query_shader_defaults(&desc);
}
inline yk__sg_pipeline_desc
yk__sg_query_pipeline_defaults(const yk__sg_pipeline_desc &desc) {
  return yk__sg_query_pipeline_defaults(&desc);
}
inline yk__sg_pass_desc
yk__sg_query_pass_defaults(const yk__sg_pass_desc &desc) {
  return yk__sg_query_pass_defaults(&desc);
}
inline void yk__sg_init_buffer(yk__sg_buffer buf_id,
                               const yk__sg_buffer_desc &desc) {
  return yk__sg_init_buffer(buf_id, &desc);
}
inline void yk__sg_init_image(yk__sg_image img_id,
                              const yk__sg_image_desc &desc) {
  return yk__sg_init_image(img_id, &desc);
}
inline void yk__sg_init_shader(yk__sg_shader shd_id,
                               const yk__sg_shader_desc &desc) {
  return yk__sg_init_shader(shd_id, &desc);
}
inline void yk__sg_init_pipeline(yk__sg_pipeline pip_id,
                                 const yk__sg_pipeline_desc &desc) {
  return yk__sg_init_pipeline(pip_id, &desc);
}
inline void yk__sg_init_pass(yk__sg_pass pass_id,
                             const yk__sg_pass_desc &desc) {
  return yk__sg_init_pass(pass_id, &desc);
}
inline void yk__sg_update_buffer(yk__sg_buffer buf_id,
                                 const yk__sg_range &data) {
  return yk__sg_update_buffer(buf_id, &data);
}
inline int yk__sg_append_buffer(yk__sg_buffer buf_id,
                                const yk__sg_range &data) {
  return yk__sg_append_buffer(buf_id, &data);
}
#endif
#endif// YK__SOKOL_GFX_INCLUDED
/*--- IMPLEMENTATION ---------------------------------------------------------*/
#ifdef YK__SOKOL_GFX_IMPL
#define YK__SOKOL_GFX_IMPL_INCLUDED (1)
#if !(defined(YK__SOKOL_GLCORE33) || defined(YK__SOKOL_GLES2) ||               \
      defined(YK__SOKOL_GLES3) || defined(YK__SOKOL_D3D11) ||                  \
      defined(YK__SOKOL_METAL) || defined(YK__SOKOL_WGPU) ||                   \
      defined(YK__SOKOL_DUMMY_BACKEND))
#error                                                                         \
    "Please select a backend with YK__SOKOL_GLCORE33, YK__SOKOL_GLES2, YK__SOKOL_GLES3, YK__SOKOL_D3D11, YK__SOKOL_METAL, YK__SOKOL_WGPU or YK__SOKOL_DUMMY_BACKEND"
#endif
#include <string.h> /* memset */
#include <float.h>  /* FLT_MAX */
#ifndef YK__SOKOL_API_IMPL
#define YK__SOKOL_API_IMPL
#endif
#ifndef YK__SOKOL_DEBUG
#ifndef NDEBUG
#define YK__SOKOL_DEBUG (1)
#endif
#endif
#ifndef YK__SOKOL_ASSERT
#include <assert.h>
#define YK__SOKOL_ASSERT(c) assert(c)
#endif
#ifndef YK__SOKOL_VALIDATE_BEGIN
#define YK__SOKOL_VALIDATE_BEGIN() yk___sg_validate_begin()
#endif
#ifndef YK__SOKOL_VALIDATE
#define YK__SOKOL_VALIDATE(cond, err) yk___sg_validate((cond), err)
#endif
#ifndef YK__SOKOL_VALIDATE_END
#define YK__SOKOL_VALIDATE_END() yk___sg_validate_end()
#endif
#ifndef YK__SOKOL_UNREACHABLE
#define YK__SOKOL_UNREACHABLE YK__SOKOL_ASSERT(false)
#endif
#ifndef YK__SOKOL_MALLOC
#include <stdlib.h>
#define YK__SOKOL_MALLOC(s) malloc(s)
#define YK__SOKOL_FREE(p) free(p)
#endif
#ifndef YK__SOKOL_LOG
#ifdef YK__SOKOL_DEBUG
#include <stdio.h>
#define YK__SOKOL_LOG(s)                                                       \
  {                                                                            \
    YK__SOKOL_ASSERT(s);                                                       \
    puts(s);                                                                   \
  }
#else
#define YK__SOKOL_LOG(s)
#endif
#endif
#ifndef YK___SOKOL_PRIVATE
#if defined(__GNUC__) || defined(__clang__)
#define YK___SOKOL_PRIVATE __attribute__((unused)) static
#else
#define YK___SOKOL_PRIVATE static
#endif
#endif
#ifndef YK___SOKOL_UNUSED
#define YK___SOKOL_UNUSED(x) (void) (x)
#endif
#if defined(YK__SOKOL_TRACE_HOOKS)
#define YK___SG_TRACE_ARGS(fn, ...)                                            \
  if (yk___sg.hooks.fn) {                                                      \
    yk___sg.hooks.fn(__VA_ARGS__, yk___sg.hooks.user_data);                    \
  }
#define YK___SG_TRACE_NOARGS(fn)                                               \
  if (yk___sg.hooks.fn) { yk___sg.hooks.fn(yk___sg.hooks.user_data); }
#else
#define YK___SG_TRACE_ARGS(fn, ...)
#define YK___SG_TRACE_NOARGS(fn)
#endif
/* default clear values */
#ifndef YK__SG_DEFAULT_CLEAR_RED
#define YK__SG_DEFAULT_CLEAR_RED (0.5f)
#endif
#ifndef YK__SG_DEFAULT_CLEAR_GREEN
#define YK__SG_DEFAULT_CLEAR_GREEN (0.5f)
#endif
#ifndef YK__SG_DEFAULT_CLEAR_BLUE
#define YK__SG_DEFAULT_CLEAR_BLUE (0.5f)
#endif
#ifndef YK__SG_DEFAULT_CLEAR_ALPHA
#define YK__SG_DEFAULT_CLEAR_ALPHA (1.0f)
#endif
#ifndef YK__SG_DEFAULT_CLEAR_DEPTH
#define YK__SG_DEFAULT_CLEAR_DEPTH (1.0f)
#endif
#ifndef YK__SG_DEFAULT_CLEAR_STENCIL
#define YK__SG_DEFAULT_CLEAR_STENCIL (0)
#endif
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4115) /* named type definition in parentheses */
#pragma warning(                                                               \
    disable : 4505) /* unreferenced local function has been removed */
#pragma warning(                                                               \
    disable : 4201) /* nonstandard extension used: nameless struct/union (needed by d3d11.h) */
#pragma warning(disable : 4054) /* 'type cast': from function pointer */
#pragma warning(disable : 4055) /* 'type cast': from data pointer */
#endif
#if defined(YK__SOKOL_D3D11)
#ifndef D3D11_NO_HELPERS
#define D3D11_NO_HELPERS
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <d3d11.h>
#include <d3dcompiler.h>
#ifdef _MSC_VER
#if (defined(WINAPI_FAMILY_PARTITION) &&                                       \
     !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP))
#pragma comment(lib, "WindowsApp")
#else
#pragma comment(lib, "kernel32")
#pragma comment(lib, "user32")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d11")
#endif
#endif
#elif defined(YK__SOKOL_METAL)
// see https://clang.llvm.org/docs/LanguageExtensions.html#automatic-reference-counting
#if !defined(__cplusplus)
#if __has_feature(objc_arc) && !__has_feature(objc_arc_fields)
#error                                                                         \
    "sokol_gfx.h requires __has_feature(objc_arc_field) if ARC is enabled (use a more recent compiler version)"
#endif
#endif
#include <TargetConditionals.h>
#if defined(TARGET_OS_IPHONE) && !TARGET_OS_IPHONE
#define YK___SG_TARGET_MACOS (1)
#else
#define YK___SG_TARGET_IOS (1)
#if defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR
#define YK___SG_TARGET_IOS_SIMULATOR (1)
#endif
#endif
#import <Metal/Metal.h>
#elif defined(YK__SOKOL_WGPU)
#if defined(__EMSCRIPTEN__)
#include <webgpu/webgpu.h>
#else
#include <dawn/webgpu.h>
#endif
#elif defined(YK__SOKOL_GLCORE33) || defined(YK__SOKOL_GLES2) ||               \
    defined(YK__SOKOL_GLES3)
#define YK___SOKOL_ANY_GL (1)
// include platform specific GL headers (or on Win32: use an embedded GL loader)
#if !defined(YK__SOKOL_EXTERNAL_GL_LOADER)
#if defined(_WIN32)
#if defined(YK__SOKOL_GLCORE33) && !defined(YK__SOKOL_EXTERNAL_GL_LOADER)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#define YK___SOKOL_USE_WIN32_GL_LOADER (1)
#pragma comment(lib, "kernel32")// GetProcAddress()
#endif
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif
#if defined(TARGET_OS_IPHONE) && !TARGET_OS_IPHONE
#include <OpenGL/gl3.h>
#else
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#endif
#elif defined(__EMSCRIPTEN__) || defined(__ANDROID__)
#if defined(YK__SOKOL_GLES3)
#include <GLES3/gl3.h>
#elif defined(YK__SOKOL_GLES2)
#ifndef GL_EXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif
#elif defined(__linux__) || defined(__unix__)
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif
#endif
// optional GL loader definitions (only on Win32)
#if defined(YK___SOKOL_USE_WIN32_GL_LOADER)
#define __gl_h_ 1
#define __gl32_h_ 1
#define __gl31_h_ 1
#define __GL_H__ 1
#define __glext_h_ 1
#define __GLEXT_H_ 1
#define __gltypes_h_ 1
#define __glcorearb_h_ 1
#define __gl_glcorearb_h_ 1
#define GL_APIENTRY APIENTRY
typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;
typedef double GLclampd;
typedef unsigned short GLushort;
typedef unsigned char GLubyte;
typedef unsigned char GLboolean;
typedef uint64_t GLuint64;
typedef double GLdouble;
typedef unsigned short GLhalf;
typedef float GLclampf;
typedef unsigned int GLbitfield;
typedef signed char GLbyte;
typedef short GLshort;
typedef void GLvoid;
typedef int64_t GLint64;
typedef float GLfloat;
typedef struct __GLsync *GLsync;
typedef int GLint;
#define GL_INT_2_10_10_10_REV 0x8D9F
#define GL_R32F 0x822E
#define GL_PROGRAM_POINT_SIZE 0x8642
#define GL_STENCIL_ATTACHMENT 0x8D20
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_COLOR_ATTACHMENT2 0x8CE2
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_R16F 0x822D
#define GL_COLOR_ATTACHMENT22 0x8CF6
#define GL_DRAW_FRAMEBUFFER 0x8CA9
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_NUM_EXTENSIONS 0x821D
#define GL_INFO_LOG_LENGTH 0x8B84
#define GL_VERTEX_SHADER 0x8B31
#define GL_INCR 0x1E02
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_STATIC_DRAW 0x88E4
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0x8519
#define GL_TEXTURE_CUBE_MAP 0x8513
#define GL_FUNC_SUBTRACT 0x800A
#define GL_FUNC_REVERSE_SUBTRACT 0x800B
#define GL_CONSTANT_COLOR 0x8001
#define GL_DECR_WRAP 0x8508
#define GL_R8 0x8229
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_SHORT 0x1402
#define GL_DEPTH_TEST 0x0B71
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x8518
#define GL_LINK_STATUS 0x8B82
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0x8517
#define GL_SAMPLE_ALPHA_TO_COVERAGE 0x809E
#define GL_RGBA16F 0x881A
#define GL_CONSTANT_ALPHA 0x8003
#define GL_READ_FRAMEBUFFER 0x8CA8
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE_MIN_LOD 0x813A
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_UNSIGNED_SHORT_5_6_5 0x8363
#define GL_TEXTURE_WRAP_R 0x8072
#define GL_UNSIGNED_SHORT_5_5_5_1 0x8034
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_UNSIGNED_SHORT_4_4_4_4 0x8033
#define GL_SRC_ALPHA_SATURATE 0x0308
#define GL_STREAM_DRAW 0x88E0
#define GL_ONE 1
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#define GL_RGB10_A2 0x8059
#define GL_RGBA8 0x8058
#define GL_COLOR_ATTACHMENT1 0x8CE1
#define GL_RGBA4 0x8056
#define GL_RGB8 0x8051
#define GL_ARRAY_BUFFER 0x8892
#define GL_STENCIL 0x1802
#define GL_TEXTURE_2D 0x0DE1
#define GL_DEPTH 0x1801
#define GL_FRONT 0x0404
#define GL_STENCIL_BUFFER_BIT 0x00000400
#define GL_REPEAT 0x2901
#define GL_RGBA 0x1908
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
#define GL_DECR 0x1E03
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_FLOAT 0x1406
#define GL_TEXTURE_MAX_LOD 0x813B
#define GL_DEPTH_COMPONENT 0x1902
#define GL_ONE_MINUS_DST_ALPHA 0x0305
#define GL_COLOR 0x1800
#define GL_TEXTURE_2D_ARRAY 0x8C1A
#define GL_TRIANGLES 0x0004
#define GL_UNSIGNED_BYTE 0x1401
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_ONE_MINUS_CONSTANT_ALPHA 0x8004
#define GL_NONE 0
#define GL_SRC_COLOR 0x0300
#define GL_BYTE 0x1400
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A
#define GL_LINE_STRIP 0x0003
#define GL_TEXTURE_3D 0x806F
#define GL_CW 0x0900
#define GL_LINEAR 0x2601
#define GL_RENDERBUFFER 0x8D41
#define GL_GEQUAL 0x0206
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_RGBA32F 0x8814
#define GL_BLEND 0x0BE2
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_ONE_MINUS_CONSTANT_COLOR 0x8002
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#define GL_EXTENSIONS 0x1F03
#define GL_NO_ERROR 0
#define GL_REPLACE 0x1E01
#define GL_KEEP 0x1E00
#define GL_CCW 0x0901
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0x8516
#define GL_RGB 0x1907
#define GL_TRIANGLE_STRIP 0x0005
#define GL_FALSE 0
#define GL_ZERO 0
#define GL_CULL_FACE 0x0B44
#define GL_INVERT 0x150A
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_UNSIGNED_SHORT 0x1403
#define GL_NEAREST 0x2600
#define GL_SCISSOR_TEST 0x0C11
#define GL_LEQUAL 0x0203
#define GL_STENCIL_TEST 0x0B90
#define GL_DITHER 0x0BD0
#define GL_DEPTH_COMPONENT16 0x81A5
#define GL_EQUAL 0x0202
#define GL_FRAMEBUFFER 0x8D40
#define GL_RGB5 0x8050
#define GL_LINES 0x0001
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_SRC_ALPHA 0x0302
#define GL_INCR_WRAP 0x8507
#define GL_LESS 0x0201
#define GL_MULTISAMPLE 0x809D
#define GL_FRAMEBUFFER_BINDING 0x8CA6
#define GL_BACK 0x0405
#define GL_ALWAYS 0x0207
#define GL_FUNC_ADD 0x8006
#define GL_ONE_MINUS_DST_COLOR 0x0307
#define GL_NOTEQUAL 0x0205
#define GL_DST_COLOR 0x0306
#define GL_COMPILE_STATUS 0x8B81
#define GL_RED 0x1903
#define GL_COLOR_ATTACHMENT3 0x8CE3
#define GL_DST_ALPHA 0x0304
#define GL_RGB5_A1 0x8057
#define GL_GREATER 0x0204
#define GL_POLYGON_OFFSET_FILL 0x8037
#define GL_TRUE 1
#define GL_NEVER 0x0200
#define GL_POINTS 0x0000
#define GL_ONE_MINUS_SRC_COLOR 0x0301
#define GL_MIRRORED_REPEAT 0x8370
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D
#define GL_R11F_G11F_B10F 0x8C3A
#define GL_UNSIGNED_INT_10F_11F_11F_REV 0x8C3B
#define GL_RGBA32UI 0x8D70
#define GL_RGB32UI 0x8D71
#define GL_RGBA16UI 0x8D76
#define GL_RGB16UI 0x8D77
#define GL_RGBA8UI 0x8D7C
#define GL_RGB8UI 0x8D7D
#define GL_RGBA32I 0x8D82
#define GL_RGB32I 0x8D83
#define GL_RGBA16I 0x8D88
#define GL_RGB16I 0x8D89
#define GL_RGBA8I 0x8D8E
#define GL_RGB8I 0x8D8F
#define GL_RED_INTEGER 0x8D94
#define GL_RG 0x8227
#define GL_RG_INTEGER 0x8228
#define GL_R8 0x8229
#define GL_R16 0x822A
#define GL_RG8 0x822B
#define GL_RG16 0x822C
#define GL_R16F 0x822D
#define GL_R32F 0x822E
#define GL_RG16F 0x822F
#define GL_RG32F 0x8230
#define GL_R8I 0x8231
#define GL_R8UI 0x8232
#define GL_R16I 0x8233
#define GL_R16UI 0x8234
#define GL_R32I 0x8235
#define GL_R32UI 0x8236
#define GL_RG8I 0x8237
#define GL_RG8UI 0x8238
#define GL_RG16I 0x8239
#define GL_RG16UI 0x823A
#define GL_RG32I 0x823B
#define GL_RG32UI 0x823C
#define GL_RGBA_INTEGER 0x8D99
#define GL_R8_SNORM 0x8F94
#define GL_RG8_SNORM 0x8F95
#define GL_RGB8_SNORM 0x8F96
#define GL_RGBA8_SNORM 0x8F97
#define GL_R16_SNORM 0x8F98
#define GL_RG16_SNORM 0x8F99
#define GL_RGB16_SNORM 0x8F9A
#define GL_RGBA16_SNORM 0x8F9B
#define GL_RGBA16 0x805B
#define GL_MAX_TEXTURE_SIZE 0x0D33
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE 0x851C
#define GL_MAX_3D_TEXTURE_SIZE 0x8073
#define GL_MAX_ARRAY_TEXTURE_LAYERS 0x88FF
#define GL_MAX_VERTEX_ATTRIBS 0x8869
#define GL_CLAMP_TO_BORDER 0x812D
#define GL_TEXTURE_BORDER_COLOR 0x1004
#define GL_CURRENT_PROGRAM 0x8B8D
#define GL_MAX_VERTEX_UNIFORM_VECTORS 0x8DFB
#endif
#ifndef GL_UNSIGNED_INT_2_10_10_10_REV
#define GL_UNSIGNED_INT_2_10_10_10_REV 0x8368
#endif
#ifndef GL_UNSIGNED_INT_24_8
#define GL_UNSIGNED_INT_24_8 0x84FA
#endif
#ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#endif
#ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#endif
#ifndef GL_COMPRESSED_RED_RGTC1
#define GL_COMPRESSED_RED_RGTC1 0x8DBB
#endif
#ifndef GL_COMPRESSED_SIGNED_RED_RGTC1
#define GL_COMPRESSED_SIGNED_RED_RGTC1 0x8DBC
#endif
#ifndef GL_COMPRESSED_RED_GREEN_RGTC2
#define GL_COMPRESSED_RED_GREEN_RGTC2 0x8DBD
#endif
#ifndef GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2
#define GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2 0x8DBE
#endif
#ifndef GL_COMPRESSED_RGBA_BPTC_UNORM_ARB
#define GL_COMPRESSED_RGBA_BPTC_UNORM_ARB 0x8E8C
#endif
#ifndef GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB
#define GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB 0x8E8D
#endif
#ifndef GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB
#define GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB 0x8E8E
#endif
#ifndef GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB
#define GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB 0x8E8F
#endif
#ifndef GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG 0x8C01
#endif
#ifndef GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG
#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG 0x8C00
#endif
#ifndef GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG 0x8C03
#endif
#ifndef GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 0x8C02
#endif
#ifndef GL_COMPRESSED_RGB8_ETC2
#define GL_COMPRESSED_RGB8_ETC2 0x9274
#endif
#ifndef GL_COMPRESSED_RGBA8_ETC2_EAC
#define GL_COMPRESSED_RGBA8_ETC2_EAC 0x9278
#endif
#ifndef GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2
#define GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9276
#endif
#ifndef GL_COMPRESSED_RG11_EAC
#define GL_COMPRESSED_RG11_EAC 0x9272
#endif
#ifndef GL_COMPRESSED_SIGNED_RG11_EAC
#define GL_COMPRESSED_SIGNED_RG11_EAC 0x9273
#endif
#ifndef GL_DEPTH24_STENCIL8
#define GL_DEPTH24_STENCIL8 0x88F0
#endif
#ifndef GL_HALF_FLOAT
#define GL_HALF_FLOAT 0x140B
#endif
#ifndef GL_DEPTH_STENCIL
#define GL_DEPTH_STENCIL 0x84F9
#endif
#ifndef GL_LUMINANCE
#define GL_LUMINANCE 0x1909
#endif
#ifdef YK__SOKOL_GLES2
#ifdef GL_ANGLE_instanced_arrays
#define YK___SOKOL_GL_INSTANCING_ENABLED
#define glDrawArraysInstanced(mode, first, count, instancecount)               \
  glDrawArraysInstancedANGLE(mode, first, count, instancecount)
#define glDrawElementsInstanced(mode, count, type, indices, instancecount)     \
  glDrawElementsInstancedANGLE(mode, count, type, indices, instancecount)
#define glVertexAttribDivisor(index, divisor)                                  \
  glVertexAttribDivisorANGLE(index, divisor)
#elif defined(GL_EXT_draw_instanced) && defined(GL_EXT_instanced_arrays)
#define YK___SOKOL_GL_INSTANCING_ENABLED
#define glDrawArraysInstanced(mode, first, count, instancecount)               \
  glDrawArraysInstancedEXT(mode, first, count, instancecount)
#define glDrawElementsInstanced(mode, count, type, indices, instancecount)     \
  glDrawElementsInstancedEXT(mode, count, type, indices, instancecount)
#define glVertexAttribDivisor(index, divisor)                                  \
  glVertexAttribDivisorEXT(index, divisor)
#else
#define YK___SOKOL_GLES2_INSTANCING_ERROR                                      \
  "Select GL_ANGLE_instanced_arrays or (GL_EXT_draw_instanced & "              \
  "GL_EXT_instanced_arrays) to enable instancing in GLES2"
#define glDrawArraysInstanced(mode, first, count, instancecount)               \
  YK__SOKOL_ASSERT(0 && YK___SOKOL_GLES2_INSTANCING_ERROR)
#define glDrawElementsInstanced(mode, count, type, indices, instancecount)     \
  YK__SOKOL_ASSERT(0 && YK___SOKOL_GLES2_INSTANCING_ERROR)
#define glVertexAttribDivisor(index, divisor)                                  \
  YK__SOKOL_ASSERT(0 && YK___SOKOL_GLES2_INSTANCING_ERROR)
#endif
#else
#define YK___SOKOL_GL_INSTANCING_ENABLED
#endif
#define YK___SG_GL_CHECK_ERROR()                                               \
  { YK__SOKOL_ASSERT(glGetError() == GL_NO_ERROR); }
#endif
/*=== COMMON BACKEND STUFF ===================================================*/
/* resource pool slots */
typedef struct {
  uint32_t id;
  uint32_t ctx_id;
  yk__sg_resource_state state;
} yk___sg_slot_t;
/* constants */
enum {
  YK___SG_STRING_SIZE = 16,
  YK___SG_SLOT_SHIFT = 16,
  YK___SG_SLOT_MASK = (1 << YK___SG_SLOT_SHIFT) - 1,
  YK___SG_MAX_POOL_SIZE = (1 << YK___SG_SLOT_SHIFT),
  YK___SG_DEFAULT_BUFFER_POOL_SIZE = 128,
  YK___SG_DEFAULT_IMAGE_POOL_SIZE = 128,
  YK___SG_DEFAULT_SHADER_POOL_SIZE = 32,
  YK___SG_DEFAULT_PIPELINE_POOL_SIZE = 64,
  YK___SG_DEFAULT_PASS_POOL_SIZE = 16,
  YK___SG_DEFAULT_CONTEXT_POOL_SIZE = 16,
  YK___SG_DEFAULT_SAMPLER_CACHE_CAPACITY = 64,
  YK___SG_DEFAULT_UB_SIZE = 4 * 1024 * 1024,
  YK___SG_DEFAULT_STAGING_SIZE = 8 * 1024 * 1024,
};
/* fixed-size string */
typedef struct {
  char buf[YK___SG_STRING_SIZE];
} yk___sg_str_t;
/* helper macros */
#define yk___sg_def(val, def) (((val) == 0) ? (def) : (val))
#define yk___sg_def_flt(val, def) (((val) == 0.0f) ? (def) : (val))
#define yk___sg_min(a, b) (((a) < (b)) ? (a) : (b))
#define yk___sg_max(a, b) (((a) > (b)) ? (a) : (b))
#define yk___sg_clamp(v, v0, v1)                                               \
  (((v) < (v0)) ? (v0) : (((v) > (v1)) ? (v1) : (v)))
#define yk___sg_fequal(val, cmp, delta)                                        \
  ((((val) - (cmp)) > -(delta)) && (((val) - (cmp)) < (delta)))
typedef struct {
  int size;
  int append_pos;
  bool append_overflow;
  yk__sg_buffer_type type;
  yk__sg_usage usage;
  uint32_t update_frame_index;
  uint32_t append_frame_index;
  int num_slots;
  int active_slot;
} yk___sg_buffer_common_t;
YK___SOKOL_PRIVATE void
yk___sg_buffer_common_init(yk___sg_buffer_common_t *cmn,
                           const yk__sg_buffer_desc *desc) {
  cmn->size = (int) desc->size;
  cmn->append_pos = 0;
  cmn->append_overflow = false;
  cmn->type = desc->type;
  cmn->usage = desc->usage;
  cmn->update_frame_index = 0;
  cmn->append_frame_index = 0;
  cmn->num_slots =
      (cmn->usage == YK__SG_USAGE_IMMUTABLE) ? 1 : YK__SG_NUM_INFLIGHT_FRAMES;
  cmn->active_slot = 0;
}
typedef struct {
  yk__sg_image_type type;
  bool render_target;
  int width;
  int height;
  int num_slices;
  int num_mipmaps;
  yk__sg_usage usage;
  yk__sg_pixel_format pixel_format;
  int sample_count;
  yk__sg_filter min_filter;
  yk__sg_filter mag_filter;
  yk__sg_wrap wrap_u;
  yk__sg_wrap wrap_v;
  yk__sg_wrap wrap_w;
  yk__sg_border_color border_color;
  uint32_t max_anisotropy;
  uint32_t upd_frame_index;
  int num_slots;
  int active_slot;
} yk___sg_image_common_t;
YK___SOKOL_PRIVATE void
yk___sg_image_common_init(yk___sg_image_common_t *cmn,
                          const yk__sg_image_desc *desc) {
  cmn->type = desc->type;
  cmn->render_target = desc->render_target;
  cmn->width = desc->width;
  cmn->height = desc->height;
  cmn->num_slices = desc->num_slices;
  cmn->num_mipmaps = desc->num_mipmaps;
  cmn->usage = desc->usage;
  cmn->pixel_format = desc->pixel_format;
  cmn->sample_count = desc->sample_count;
  cmn->min_filter = desc->min_filter;
  cmn->mag_filter = desc->mag_filter;
  cmn->wrap_u = desc->wrap_u;
  cmn->wrap_v = desc->wrap_v;
  cmn->wrap_w = desc->wrap_w;
  cmn->border_color = desc->border_color;
  cmn->max_anisotropy = desc->max_anisotropy;
  cmn->upd_frame_index = 0;
  cmn->num_slots =
      (cmn->usage == YK__SG_USAGE_IMMUTABLE) ? 1 : YK__SG_NUM_INFLIGHT_FRAMES;
  cmn->active_slot = 0;
}
typedef struct {
  size_t size;
} yk___sg_uniform_block_t;
typedef struct {
  yk__sg_image_type image_type;
  yk__sg_sampler_type sampler_type;
} yk___sg_shader_image_t;
typedef struct {
  int num_uniform_blocks;
  int num_images;
  yk___sg_uniform_block_t uniform_blocks[YK__SG_MAX_SHADERSTAGE_UBS];
  yk___sg_shader_image_t images[YK__SG_MAX_SHADERSTAGE_IMAGES];
} yk___sg_shader_stage_t;
typedef struct {
  yk___sg_shader_stage_t stage[YK__SG_NUM_SHADER_STAGES];
} yk___sg_shader_common_t;
YK___SOKOL_PRIVATE void
yk___sg_shader_common_init(yk___sg_shader_common_t *cmn,
                           const yk__sg_shader_desc *desc) {
  for (int stage_index = 0; stage_index < YK__SG_NUM_SHADER_STAGES;
       stage_index++) {
    const yk__sg_shader_stage_desc *stage_desc =
        (stage_index == YK__SG_SHADERSTAGE_VS) ? &desc->vs : &desc->fs;
    yk___sg_shader_stage_t *stage = &cmn->stage[stage_index];
    YK__SOKOL_ASSERT(stage->num_uniform_blocks == 0);
    for (int ub_index = 0; ub_index < YK__SG_MAX_SHADERSTAGE_UBS; ub_index++) {
      const yk__sg_shader_uniform_block_desc *ub_desc =
          &stage_desc->uniform_blocks[ub_index];
      if (0 == ub_desc->size) { break; }
      stage->uniform_blocks[ub_index].size = ub_desc->size;
      stage->num_uniform_blocks++;
    }
    YK__SOKOL_ASSERT(stage->num_images == 0);
    for (int img_index = 0; img_index < YK__SG_MAX_SHADERSTAGE_IMAGES;
         img_index++) {
      const yk__sg_shader_image_desc *img_desc = &stage_desc->images[img_index];
      if (img_desc->image_type == YK___SG_IMAGETYPE_DEFAULT) { break; }
      stage->images[img_index].image_type = img_desc->image_type;
      stage->images[img_index].sampler_type = img_desc->sampler_type;
      stage->num_images++;
    }
  }
}
typedef struct {
  yk__sg_shader shader_id;
  yk__sg_index_type index_type;
  bool use_instanced_draw;
  bool vertex_layout_valid[YK__SG_MAX_SHADERSTAGE_BUFFERS];
  int color_attachment_count;
  yk__sg_pixel_format color_formats[YK__SG_MAX_COLOR_ATTACHMENTS];
  yk__sg_pixel_format depth_format;
  int sample_count;
  float depth_bias;
  float depth_bias_slope_scale;
  float depth_bias_clamp;
  yk__sg_color blend_color;
} yk___sg_pipeline_common_t;
YK___SOKOL_PRIVATE void
yk___sg_pipeline_common_init(yk___sg_pipeline_common_t *cmn,
                             const yk__sg_pipeline_desc *desc) {
  YK__SOKOL_ASSERT((desc->color_count >= 1) &&
                   (desc->color_count <= YK__SG_MAX_COLOR_ATTACHMENTS));
  cmn->shader_id = desc->shader;
  cmn->index_type = desc->index_type;
  cmn->use_instanced_draw = false;
  for (int i = 0; i < YK__SG_MAX_SHADERSTAGE_BUFFERS; i++) {
    cmn->vertex_layout_valid[i] = false;
  }
  cmn->color_attachment_count = desc->color_count;
  for (int i = 0; i < cmn->color_attachment_count; i++) {
    cmn->color_formats[i] = desc->colors[i].pixel_format;
  }
  cmn->depth_format = desc->depth.pixel_format;
  cmn->sample_count = desc->sample_count;
  cmn->depth_bias = desc->depth.bias;
  cmn->depth_bias_slope_scale = desc->depth.bias_slope_scale;
  cmn->depth_bias_clamp = desc->depth.bias_clamp;
  cmn->blend_color = desc->blend_color;
}
typedef struct {
  yk__sg_image image_id;
  int mip_level;
  int slice;
} yk___sg_pass_attachment_common_t;
typedef struct {
  int num_color_atts;
  yk___sg_pass_attachment_common_t color_atts[YK__SG_MAX_COLOR_ATTACHMENTS];
  yk___sg_pass_attachment_common_t ds_att;
} yk___sg_pass_common_t;
YK___SOKOL_PRIVATE void yk___sg_pass_common_init(yk___sg_pass_common_t *cmn,
                                                 const yk__sg_pass_desc *desc) {
  const yk__sg_pass_attachment_desc *att_desc;
  yk___sg_pass_attachment_common_t *att;
  for (int i = 0; i < YK__SG_MAX_COLOR_ATTACHMENTS; i++) {
    att_desc = &desc->color_attachments[i];
    if (att_desc->image.id != YK__SG_INVALID_ID) {
      cmn->num_color_atts++;
      att = &cmn->color_atts[i];
      att->image_id = att_desc->image;
      att->mip_level = att_desc->mip_level;
      att->slice = att_desc->slice;
    }
  }
  att_desc = &desc->depth_stencil_attachment;
  if (att_desc->image.id != YK__SG_INVALID_ID) {
    att = &cmn->ds_att;
    att->image_id = att_desc->image;
    att->mip_level = att_desc->mip_level;
    att->slice = att_desc->slice;
  }
}
/*=== GENERIC SAMPLER CACHE ==================================================*/
/*
    this is used by the Metal and WGPU backends to reduce the
    number of sampler state objects created through the backend API
*/
typedef struct {
  yk__sg_filter min_filter;
  yk__sg_filter mag_filter;
  yk__sg_wrap wrap_u;
  yk__sg_wrap wrap_v;
  yk__sg_wrap wrap_w;
  yk__sg_border_color border_color;
  uint32_t max_anisotropy;
  int min_lod; /* orig min/max_lod is float, this is int(min/max_lod*1000.0) */
  int max_lod;
  uintptr_t sampler_handle;
} yk___sg_sampler_cache_item_t;
typedef struct {
  int capacity;
  int num_items;
  yk___sg_sampler_cache_item_t *items;
} yk___sg_sampler_cache_t;
YK___SOKOL_PRIVATE void yk___sg_smpcache_init(yk___sg_sampler_cache_t *cache,
                                              int capacity) {
  YK__SOKOL_ASSERT(cache && (capacity > 0));
  memset(cache, 0, sizeof(yk___sg_sampler_cache_t));
  cache->capacity = capacity;
  const size_t size =
      (size_t) cache->capacity * sizeof(yk___sg_sampler_cache_item_t);
  cache->items = (yk___sg_sampler_cache_item_t *) YK__SOKOL_MALLOC(size);
  YK__SOKOL_ASSERT(cache->items);
  memset(cache->items, 0, size);
}
YK___SOKOL_PRIVATE void
yk___sg_smpcache_discard(yk___sg_sampler_cache_t *cache) {
  YK__SOKOL_ASSERT(cache && cache->items);
  YK__SOKOL_FREE(cache->items);
  cache->items = 0;
  cache->num_items = 0;
  cache->capacity = 0;
}
YK___SOKOL_PRIVATE int yk___sg_smpcache_minlod_int(float min_lod) {
  return (int) (min_lod * 1000.0f);
}
YK___SOKOL_PRIVATE int yk___sg_smpcache_maxlod_int(float max_lod) {
  return (int) (yk___sg_clamp(max_lod, 0.0f, 1000.0f) * 1000.0f);
}
YK___SOKOL_PRIVATE int
yk___sg_smpcache_find_item(const yk___sg_sampler_cache_t *cache,
                           const yk__sg_image_desc *img_desc) {
  /* return matching sampler cache item index or -1 */
  YK__SOKOL_ASSERT(cache && cache->items);
  YK__SOKOL_ASSERT(img_desc);
  const int min_lod = yk___sg_smpcache_minlod_int(img_desc->min_lod);
  const int max_lod = yk___sg_smpcache_maxlod_int(img_desc->max_lod);
  for (int i = 0; i < cache->num_items; i++) {
    const yk___sg_sampler_cache_item_t *item = &cache->items[i];
    if ((img_desc->min_filter == item->min_filter) &&
        (img_desc->mag_filter == item->mag_filter) &&
        (img_desc->wrap_u == item->wrap_u) &&
        (img_desc->wrap_v == item->wrap_v) &&
        (img_desc->wrap_w == item->wrap_w) &&
        (img_desc->max_anisotropy == item->max_anisotropy) &&
        (img_desc->border_color == item->border_color) &&
        (min_lod == item->min_lod) && (max_lod == item->max_lod)) {
      return i;
    }
  }
  /* fallthrough: no matching cache item found */
  return -1;
}
YK___SOKOL_PRIVATE void
yk___sg_smpcache_add_item(yk___sg_sampler_cache_t *cache,
                          const yk__sg_image_desc *img_desc,
                          uintptr_t sampler_handle) {
  YK__SOKOL_ASSERT(cache && cache->items);
  YK__SOKOL_ASSERT(img_desc);
  YK__SOKOL_ASSERT(cache->num_items < cache->capacity);
  const int item_index = cache->num_items++;
  yk___sg_sampler_cache_item_t *item = &cache->items[item_index];
  item->min_filter = img_desc->min_filter;
  item->mag_filter = img_desc->mag_filter;
  item->wrap_u = img_desc->wrap_u;
  item->wrap_v = img_desc->wrap_v;
  item->wrap_w = img_desc->wrap_w;
  item->border_color = img_desc->border_color;
  item->max_anisotropy = img_desc->max_anisotropy;
  item->min_lod = yk___sg_smpcache_minlod_int(img_desc->min_lod);
  item->max_lod = yk___sg_smpcache_maxlod_int(img_desc->max_lod);
  item->sampler_handle = sampler_handle;
}
YK___SOKOL_PRIVATE uintptr_t
yk___sg_smpcache_sampler(yk___sg_sampler_cache_t *cache, int item_index) {
  YK__SOKOL_ASSERT(cache && cache->items);
  YK__SOKOL_ASSERT(item_index < cache->num_items);
  return cache->items[item_index].sampler_handle;
}
/*=== DUMMY BACKEND DECLARATIONS =============================================*/
#if defined(YK__SOKOL_DUMMY_BACKEND)
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_buffer_common_t cmn;
} yk___sg_dummy_buffer_t;
typedef yk___sg_dummy_buffer_t yk___sg_buffer_t;
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_image_common_t cmn;
} yk___sg_dummy_image_t;
typedef yk___sg_dummy_image_t yk___sg_image_t;
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_shader_common_t cmn;
} yk___sg_dummy_shader_t;
typedef yk___sg_dummy_shader_t yk___sg_shader_t;
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_shader_t *shader;
  yk___sg_pipeline_common_t cmn;
} yk___sg_dummy_pipeline_t;
typedef yk___sg_dummy_pipeline_t yk___sg_pipeline_t;
typedef struct {
  yk___sg_image_t *image;
} yk___sg_dummy_attachment_t;
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_pass_common_t cmn;
  struct {
    yk___sg_dummy_attachment_t color_atts[YK__SG_MAX_COLOR_ATTACHMENTS];
    yk___sg_dummy_attachment_t ds_att;
  } dmy;
} yk___sg_dummy_pass_t;
typedef yk___sg_dummy_pass_t yk___sg_pass_t;
typedef yk___sg_pass_attachment_common_t yk___sg_pass_attachment_t;
typedef struct {
  yk___sg_slot_t slot;
} yk___sg_dummy_context_t;
typedef yk___sg_dummy_context_t yk___sg_context_t;
/*== GL BACKEND DECLARATIONS =================================================*/
#elif defined(YK___SOKOL_ANY_GL)
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_buffer_common_t cmn;
  struct {
    GLuint buf[YK__SG_NUM_INFLIGHT_FRAMES];
    bool
        ext_buffers; /* if true, external buffers were injected with yk__sg_buffer_desc.gl_buffers */
  } gl;
} yk___sg_gl_buffer_t;
typedef yk___sg_gl_buffer_t yk___sg_buffer_t;
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_image_common_t cmn;
  struct {
    GLenum target;
    GLuint depth_render_buffer;
    GLuint msaa_render_buffer;
    GLuint tex[YK__SG_NUM_INFLIGHT_FRAMES];
    bool
        ext_textures; /* if true, external textures were injected with yk__sg_image_desc.gl_textures */
  } gl;
} yk___sg_gl_image_t;
typedef yk___sg_gl_image_t yk___sg_image_t;
typedef struct {
  GLint gl_loc;
  yk__sg_uniform_type type;
  uint16_t count;
  uint16_t offset;
} yk___sg_gl_uniform_t;
typedef struct {
  int num_uniforms;
  yk___sg_gl_uniform_t uniforms[YK__SG_MAX_UB_MEMBERS];
} yk___sg_gl_uniform_block_t;
typedef struct {
  int gl_tex_slot;
} yk___sg_gl_shader_image_t;
typedef struct {
  yk___sg_str_t name;
} yk___sg_gl_shader_attr_t;
typedef struct {
  yk___sg_gl_uniform_block_t uniform_blocks[YK__SG_MAX_SHADERSTAGE_UBS];
  yk___sg_gl_shader_image_t images[YK__SG_MAX_SHADERSTAGE_IMAGES];
} yk___sg_gl_shader_stage_t;
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_shader_common_t cmn;
  struct {
    GLuint prog;
    yk___sg_gl_shader_attr_t attrs[YK__SG_MAX_VERTEX_ATTRIBUTES];
    yk___sg_gl_shader_stage_t stage[YK__SG_NUM_SHADER_STAGES];
  } gl;
} yk___sg_gl_shader_t;
typedef yk___sg_gl_shader_t yk___sg_shader_t;
typedef struct {
  int8_t vb_index; /* -1 if attr is not enabled */
  int8_t divisor;  /* -1 if not initialized */
  uint8_t stride;
  uint8_t size;
  uint8_t normalized;
  int offset;
  GLenum type;
} yk___sg_gl_attr_t;
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_pipeline_common_t cmn;
  yk___sg_shader_t *shader;
  struct {
    yk___sg_gl_attr_t attrs[YK__SG_MAX_VERTEX_ATTRIBUTES];
    yk__sg_depth_state depth;
    yk__sg_stencil_state stencil;
    yk__sg_primitive_type primitive_type;
    yk__sg_blend_state blend;
    yk__sg_color_mask color_write_mask[YK__SG_MAX_COLOR_ATTACHMENTS];
    yk__sg_cull_mode cull_mode;
    yk__sg_face_winding face_winding;
    int sample_count;
    bool alpha_to_coverage_enabled;
  } gl;
} yk___sg_gl_pipeline_t;
typedef yk___sg_gl_pipeline_t yk___sg_pipeline_t;
typedef struct {
  yk___sg_image_t *image;
  GLuint gl_msaa_resolve_buffer;
} yk___sg_gl_attachment_t;
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_pass_common_t cmn;
  struct {
    GLuint fb;
    yk___sg_gl_attachment_t color_atts[YK__SG_MAX_COLOR_ATTACHMENTS];
    yk___sg_gl_attachment_t ds_att;
  } gl;
} yk___sg_gl_pass_t;
typedef yk___sg_gl_pass_t yk___sg_pass_t;
typedef yk___sg_pass_attachment_common_t yk___sg_pass_attachment_t;
typedef struct {
  yk___sg_slot_t slot;
#if !defined(YK__SOKOL_GLES2)
  GLuint vao;
#endif
  GLuint default_framebuffer;
} yk___sg_gl_context_t;
typedef yk___sg_gl_context_t yk___sg_context_t;
typedef struct {
  yk___sg_gl_attr_t gl_attr;
  GLuint gl_vbuf;
} yk___sg_gl_cache_attr_t;
typedef struct {
  GLenum target;
  GLuint texture;
} yk___sg_gl_texture_bind_slot;
typedef struct {
  yk__sg_depth_state depth;
  yk__sg_stencil_state stencil;
  yk__sg_blend_state blend;
  yk__sg_color_mask color_write_mask[YK__SG_MAX_COLOR_ATTACHMENTS];
  yk__sg_cull_mode cull_mode;
  yk__sg_face_winding face_winding;
  bool polygon_offset_enabled;
  int sample_count;
  yk__sg_color blend_color;
  bool alpha_to_coverage_enabled;
  yk___sg_gl_cache_attr_t attrs[YK__SG_MAX_VERTEX_ATTRIBUTES];
  GLuint vertex_buffer;
  GLuint index_buffer;
  GLuint stored_vertex_buffer;
  GLuint stored_index_buffer;
  GLuint prog;
  yk___sg_gl_texture_bind_slot textures[YK__SG_MAX_SHADERSTAGE_IMAGES];
  yk___sg_gl_texture_bind_slot stored_texture;
  int cur_ib_offset;
  GLenum cur_primitive_type;
  GLenum cur_index_type;
  GLenum cur_active_texture;
  yk___sg_pipeline_t *cur_pipeline;
  yk__sg_pipeline cur_pipeline_id;
} yk___sg_gl_state_cache_t;
typedef struct {
  bool valid;
  bool gles2;
  bool in_pass;
  int cur_pass_width;
  int cur_pass_height;
  yk___sg_context_t *cur_context;
  yk___sg_pass_t *cur_pass;
  yk__sg_pass cur_pass_id;
  yk___sg_gl_state_cache_t cache;
  bool ext_anisotropic;
  GLint max_anisotropy;
  GLint max_combined_texture_image_units;
#if YK___SOKOL_USE_WIN32_GL_LOADER
  HINSTANCE opengl32_dll;
#endif
} yk___sg_gl_backend_t;
/*== D3D11 BACKEND DECLARATIONS ==============================================*/
#elif defined(YK__SOKOL_D3D11)
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_buffer_common_t cmn;
  struct {
    ID3D11Buffer *buf;
  } d3d11;
} yk___sg_d3d11_buffer_t;
typedef yk___sg_d3d11_buffer_t yk___sg_buffer_t;
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_image_common_t cmn;
  struct {
    DXGI_FORMAT format;
    ID3D11Texture2D *tex2d;
    ID3D11Texture3D *tex3d;
    ID3D11Texture2D *texds;
    ID3D11Texture2D *texmsaa;
    ID3D11ShaderResourceView *srv;
    ID3D11SamplerState *smp;
  } d3d11;
} yk___sg_d3d11_image_t;
typedef yk___sg_d3d11_image_t yk___sg_image_t;
typedef struct {
  yk___sg_str_t sem_name;
  int sem_index;
} yk___sg_d3d11_shader_attr_t;
typedef struct {
  ID3D11Buffer *cbufs[YK__SG_MAX_SHADERSTAGE_UBS];
} yk___sg_d3d11_shader_stage_t;
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_shader_common_t cmn;
  struct {
    yk___sg_d3d11_shader_attr_t attrs[YK__SG_MAX_VERTEX_ATTRIBUTES];
    yk___sg_d3d11_shader_stage_t stage[YK__SG_NUM_SHADER_STAGES];
    ID3D11VertexShader *vs;
    ID3D11PixelShader *fs;
    void *vs_blob;
    size_t vs_blob_length;
  } d3d11;
} yk___sg_d3d11_shader_t;
typedef yk___sg_d3d11_shader_t yk___sg_shader_t;
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_pipeline_common_t cmn;
  yk___sg_shader_t *shader;
  struct {
    UINT stencil_ref;
    UINT vb_strides[YK__SG_MAX_SHADERSTAGE_BUFFERS];
    D3D_PRIMITIVE_TOPOLOGY topology;
    DXGI_FORMAT index_format;
    ID3D11InputLayout *il;
    ID3D11RasterizerState *rs;
    ID3D11DepthStencilState *dss;
    ID3D11BlendState *bs;
  } d3d11;
} yk___sg_d3d11_pipeline_t;
typedef yk___sg_d3d11_pipeline_t yk___sg_pipeline_t;
typedef struct {
  yk___sg_image_t *image;
  ID3D11RenderTargetView *rtv;
} yk___sg_d3d11_color_attachment_t;
typedef struct {
  yk___sg_image_t *image;
  ID3D11DepthStencilView *dsv;
} yk___sg_d3d11_ds_attachment_t;
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_pass_common_t cmn;
  struct {
    yk___sg_d3d11_color_attachment_t color_atts[YK__SG_MAX_COLOR_ATTACHMENTS];
    yk___sg_d3d11_ds_attachment_t ds_att;
  } d3d11;
} yk___sg_d3d11_pass_t;
typedef yk___sg_d3d11_pass_t yk___sg_pass_t;
typedef yk___sg_pass_attachment_common_t yk___sg_pass_attachment_t;
typedef struct {
  yk___sg_slot_t slot;
} yk___sg_d3d11_context_t;
typedef yk___sg_d3d11_context_t yk___sg_context_t;
typedef struct {
  bool valid;
  ID3D11Device *dev;
  ID3D11DeviceContext *ctx;
  const void *(*rtv_cb)(void);
  const void *(*rtv_userdata_cb)(void *);
  const void *(*dsv_cb)(void);
  const void *(*dsv_userdata_cb)(void *);
  void *user_data;
  bool in_pass;
  bool use_indexed_draw;
  bool use_instanced_draw;
  int cur_width;
  int cur_height;
  int num_rtvs;
  yk___sg_pass_t *cur_pass;
  yk__sg_pass cur_pass_id;
  yk___sg_pipeline_t *cur_pipeline;
  yk__sg_pipeline cur_pipeline_id;
  ID3D11RenderTargetView *cur_rtvs[YK__SG_MAX_COLOR_ATTACHMENTS];
  ID3D11DepthStencilView *cur_dsv;
  /* on-demand loaded d3dcompiler_47.dll handles */
  HINSTANCE d3dcompiler_dll;
  bool d3dcompiler_dll_load_failed;
  pD3DCompile D3DCompile_func;
  /* global subresourcedata array for texture updates */
  D3D11_SUBRESOURCE_DATA
      subres_data[YK__SG_MAX_MIPMAPS * YK__SG_MAX_TEXTUREARRAY_LAYERS];
} yk___sg_d3d11_backend_t;
/*=== METAL BACKEND DECLARATIONS =============================================*/
#elif defined(YK__SOKOL_METAL)
#if defined(YK___SG_TARGET_MACOS) || defined(YK___SG_TARGET_IOS_SIMULATOR)
#define YK___SG_MTL_UB_ALIGN (256)
#else
#define YK___SG_MTL_UB_ALIGN (16)
#endif
#define YK___SG_MTL_INVALID_SLOT_INDEX (0)
typedef struct {
  uint32_t
      frame_index; /* frame index at which it is safe to release this resource */
  int slot_index;
} yk___sg_mtl_release_item_t;
typedef struct {
  NSMutableArray *pool;
  int num_slots;
  int free_queue_top;
  int *free_queue;
  int release_queue_front;
  int release_queue_back;
  yk___sg_mtl_release_item_t *release_queue;
} yk___sg_mtl_idpool_t;
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_buffer_common_t cmn;
  struct {
    int buf[YK__SG_NUM_INFLIGHT_FRAMES]; /* index into _sg_mtl_pool */
  } mtl;
} yk___sg_mtl_buffer_t;
typedef yk___sg_mtl_buffer_t yk___sg_buffer_t;
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_image_common_t cmn;
  struct {
    int tex[YK__SG_NUM_INFLIGHT_FRAMES];
    int depth_tex;
    int msaa_tex;
    int sampler_state;
  } mtl;
} yk___sg_mtl_image_t;
typedef yk___sg_mtl_image_t yk___sg_image_t;
typedef struct {
  int mtl_lib;
  int mtl_func;
} yk___sg_mtl_shader_stage_t;
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_shader_common_t cmn;
  struct {
    yk___sg_mtl_shader_stage_t stage[YK__SG_NUM_SHADER_STAGES];
  } mtl;
} yk___sg_mtl_shader_t;
typedef yk___sg_mtl_shader_t yk___sg_shader_t;
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_pipeline_common_t cmn;
  yk___sg_shader_t *shader;
  struct {
    MTLPrimitiveType prim_type;
    int index_size;
    MTLIndexType index_type;
    MTLCullMode cull_mode;
    MTLWinding winding;
    uint32_t stencil_ref;
    int rps;
    int dss;
  } mtl;
} yk___sg_mtl_pipeline_t;
typedef yk___sg_mtl_pipeline_t yk___sg_pipeline_t;
typedef struct {
  yk___sg_image_t *image;
} yk___sg_mtl_attachment_t;
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_pass_common_t cmn;
  struct {
    yk___sg_mtl_attachment_t color_atts[YK__SG_MAX_COLOR_ATTACHMENTS];
    yk___sg_mtl_attachment_t ds_att;
  } mtl;
} yk___sg_mtl_pass_t;
typedef yk___sg_mtl_pass_t yk___sg_pass_t;
typedef yk___sg_pass_attachment_common_t yk___sg_pass_attachment_t;
typedef struct {
  yk___sg_slot_t slot;
} yk___sg_mtl_context_t;
typedef yk___sg_mtl_context_t yk___sg_context_t;
/* resouce binding state cache */
typedef struct {
  const yk___sg_pipeline_t *cur_pipeline;
  yk__sg_pipeline cur_pipeline_id;
  const yk___sg_buffer_t *cur_indexbuffer;
  int cur_indexbuffer_offset;
  yk__sg_buffer cur_indexbuffer_id;
  const yk___sg_buffer_t *cur_vertexbuffers[YK__SG_MAX_SHADERSTAGE_BUFFERS];
  int cur_vertexbuffer_offsets[YK__SG_MAX_SHADERSTAGE_BUFFERS];
  yk__sg_buffer cur_vertexbuffer_ids[YK__SG_MAX_SHADERSTAGE_BUFFERS];
  const yk___sg_image_t *cur_vs_images[YK__SG_MAX_SHADERSTAGE_IMAGES];
  yk__sg_image cur_vs_image_ids[YK__SG_MAX_SHADERSTAGE_IMAGES];
  const yk___sg_image_t *cur_fs_images[YK__SG_MAX_SHADERSTAGE_IMAGES];
  yk__sg_image cur_fs_image_ids[YK__SG_MAX_SHADERSTAGE_IMAGES];
} yk___sg_mtl_state_cache_t;
typedef struct {
  bool valid;
  const void *(*renderpass_descriptor_cb)(void);
  const void *(*renderpass_descriptor_userdata_cb)(void *);
  const void *(*drawable_cb)(void);
  const void *(*drawable_userdata_cb)(void *);
  void *user_data;
  uint32_t frame_index;
  uint32_t cur_frame_rotate_index;
  int ub_size;
  int cur_ub_offset;
  uint8_t *cur_ub_base_ptr;
  bool in_pass;
  bool pass_valid;
  int cur_width;
  int cur_height;
  yk___sg_mtl_state_cache_t state_cache;
  yk___sg_sampler_cache_t sampler_cache;
  yk___sg_mtl_idpool_t idpool;
  dispatch_semaphore_t sem;
  id<MTLDevice> device;
  id<MTLCommandQueue> cmd_queue;
  id<MTLCommandBuffer> cmd_buffer;
  id<MTLRenderCommandEncoder> cmd_encoder;
  id<MTLBuffer> uniform_buffers[YK__SG_NUM_INFLIGHT_FRAMES];
} yk___sg_mtl_backend_t;
/*=== WGPU BACKEND DECLARATIONS ==============================================*/
#elif defined(YK__SOKOL_WGPU)
#define YK___SG_WGPU_STAGING_ALIGN (256)
#define YK___SG_WGPU_STAGING_PIPELINE_SIZE (8)
#define YK___SG_WGPU_ROWPITCH_ALIGN (256)
#define YK___SG_WGPU_MAX_SHADERSTAGE_IMAGES (8)
#define YK___SG_WGPU_MAX_UNIFORM_UPDATE_SIZE (1 << 16)
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_buffer_common_t cmn;
  struct {
    WGPUBuffer buf;
  } wgpu;
} yk___sg_wgpu_buffer_t;
typedef yk___sg_wgpu_buffer_t yk___sg_buffer_t;
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_image_common_t cmn;
  struct {
    WGPUTexture tex;
    WGPUTextureView tex_view;
    WGPUTexture msaa_tex;
    WGPUSampler sampler;
  } wgpu;
} yk___sg_wgpu_image_t;
typedef yk___sg_wgpu_image_t yk___sg_image_t;
typedef struct {
  WGPUShaderModule module;
  WGPUBindGroupLayout bind_group_layout;
  yk___sg_str_t entry;
} yk___sg_wgpu_shader_stage_t;
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_shader_common_t cmn;
  struct {
    yk___sg_wgpu_shader_stage_t stage[YK__SG_NUM_SHADER_STAGES];
  } wgpu;
} yk___sg_wgpu_shader_t;
typedef yk___sg_wgpu_shader_t yk___sg_shader_t;
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_pipeline_common_t cmn;
  yk___sg_shader_t *shader;
  struct {
    WGPURenderPipeline pip;
    uint32_t stencil_ref;
  } wgpu;
} yk___sg_wgpu_pipeline_t;
typedef yk___sg_wgpu_pipeline_t yk___sg_pipeline_t;
typedef struct {
  yk___sg_image_t *image;
  WGPUTextureView render_tex_view;
  WGPUTextureView resolve_tex_view;
} yk___sg_wgpu_attachment_t;
typedef struct {
  yk___sg_slot_t slot;
  yk___sg_pass_common_t cmn;
  struct {
    yk___sg_wgpu_attachment_t color_atts[YK__SG_MAX_COLOR_ATTACHMENTS];
    yk___sg_wgpu_attachment_t ds_att;
  } wgpu;
} yk___sg_wgpu_pass_t;
typedef yk___sg_wgpu_pass_t yk___sg_pass_t;
typedef yk___sg_pass_attachment_common_t yk___sg_pass_attachment_t;
typedef struct {
  yk___sg_slot_t slot;
} yk___sg_wgpu_context_t;
typedef yk___sg_wgpu_context_t yk___sg_context_t;
/* a pool of per-frame uniform buffers */
typedef struct {
  WGPUBindGroupLayout bindgroup_layout;
  uint32_t num_bytes;
  uint32_t
      offset; /* current offset into current frame's mapped uniform buffer */
  uint32_t bind_offsets[YK__SG_NUM_SHADER_STAGES][YK__SG_MAX_SHADERSTAGE_UBS];
  WGPUBuffer buf; /* the GPU-side uniform buffer */
  WGPUBindGroup bindgroup;
  struct {
    int num;
    int cur;
    WGPUBuffer
        buf[YK___SG_WGPU_STAGING_PIPELINE_SIZE]; /* CPU-side staging buffers */
    uint8_t *ptr
        [YK___SG_WGPU_STAGING_PIPELINE_SIZE]; /* if != 0, staging buffer currently mapped */
  } stage;
} yk___sg_wgpu_ubpool_t;
/* ...a similar pool (like uniform buffer pool) of dynamic-resource staging buffers */
typedef struct {
  uint32_t num_bytes;
  uint32_t offset; /* current offset into current frame's staging buffer */
  int num;         /* number of staging buffers */
  int cur;         /* this frame's staging buffer */
  WGPUBuffer
      buf[YK___SG_WGPU_STAGING_PIPELINE_SIZE]; /* CPU-side staging buffers */
  uint8_t *ptr
      [YK___SG_WGPU_STAGING_PIPELINE_SIZE]; /* if != 0, staging buffer currently mapped */
} yk___sg_wgpu_stagingpool_t;
/* the WGPU backend state */
typedef struct {
  bool valid;
  bool in_pass;
  bool draw_indexed;
  int cur_width;
  int cur_height;
  WGPUDevice dev;
  WGPUTextureView (*render_view_cb)(void);
  WGPUTextureView (*render_view_userdata_cb)(void *);
  WGPUTextureView (*resolve_view_cb)(void);
  WGPUTextureView (*resolve_view_userdata_cb)(void *);
  WGPUTextureView (*depth_stencil_view_cb)(void);
  WGPUTextureView (*depth_stencil_view_userdata_cb)(void *);
  void *user_data;
  WGPUQueue queue;
  WGPUCommandEncoder render_cmd_enc;
  WGPUCommandEncoder staging_cmd_enc;
  WGPURenderPassEncoder pass_enc;
  WGPUBindGroup empty_bind_group;
  const yk___sg_pipeline_t *cur_pipeline;
  yk__sg_pipeline cur_pipeline_id;
  yk___sg_sampler_cache_t sampler_cache;
  yk___sg_wgpu_ubpool_t ub;
  yk___sg_wgpu_stagingpool_t staging;
} yk___sg_wgpu_backend_t;
#endif
/*=== RESOURCE POOL DECLARATIONS =============================================*/
/* this *MUST* remain 0 */
#define YK___SG_INVALID_SLOT_INDEX (0)
typedef struct {
  int size;
  int queue_top;
  uint32_t *gen_ctrs;
  int *free_queue;
} yk___sg_pool_t;
typedef struct {
  yk___sg_pool_t buffer_pool;
  yk___sg_pool_t image_pool;
  yk___sg_pool_t shader_pool;
  yk___sg_pool_t pipeline_pool;
  yk___sg_pool_t pass_pool;
  yk___sg_pool_t context_pool;
  yk___sg_buffer_t *buffers;
  yk___sg_image_t *images;
  yk___sg_shader_t *shaders;
  yk___sg_pipeline_t *pipelines;
  yk___sg_pass_t *passes;
  yk___sg_context_t *contexts;
} yk___sg_pools_t;
/*=== VALIDATION LAYER DECLARATIONS ==========================================*/
typedef enum {
  /* special case 'validation was successful' */
  YK___SG_VALIDATE_SUCCESS,
  /* buffer creation */
  YK___SG_VALIDATE_BUFFERDESC_CANARY,
  YK___SG_VALIDATE_BUFFERDESC_SIZE,
  YK___SG_VALIDATE_BUFFERDESC_DATA,
  YK___SG_VALIDATE_BUFFERDESC_DATA_SIZE,
  YK___SG_VALIDATE_BUFFERDESC_NO_DATA,
  /* image data (for image creation and updating) */
  YK___SG_VALIDATE_IMAGEDATA_NODATA,
  YK___SG_VALIDATE_IMAGEDATA_DATA_SIZE,
  /* image creation */
  YK___SG_VALIDATE_IMAGEDESC_CANARY,
  YK___SG_VALIDATE_IMAGEDESC_WIDTH,
  YK___SG_VALIDATE_IMAGEDESC_HEIGHT,
  YK___SG_VALIDATE_IMAGEDESC_RT_PIXELFORMAT,
  YK___SG_VALIDATE_IMAGEDESC_NONRT_PIXELFORMAT,
  YK___SG_VALIDATE_IMAGEDESC_MSAA_BUT_NO_RT,
  YK___SG_VALIDATE_IMAGEDESC_NO_MSAA_RT_SUPPORT,
  YK___SG_VALIDATE_IMAGEDESC_RT_IMMUTABLE,
  YK___SG_VALIDATE_IMAGEDESC_RT_NO_DATA,
  YK___SG_VALIDATE_IMAGEDESC_INJECTED_NO_DATA,
  YK___SG_VALIDATE_IMAGEDESC_DYNAMIC_NO_DATA,
  YK___SG_VALIDATE_IMAGEDESC_COMPRESSED_IMMUTABLE,
  /* shader creation */
  YK___SG_VALIDATE_SHADERDESC_CANARY,
  YK___SG_VALIDATE_SHADERDESC_SOURCE,
  YK___SG_VALIDATE_SHADERDESC_BYTECODE,
  YK___SG_VALIDATE_SHADERDESC_SOURCE_OR_BYTECODE,
  YK___SG_VALIDATE_SHADERDESC_NO_BYTECODE_SIZE,
  YK___SG_VALIDATE_SHADERDESC_NO_CONT_UBS,
  YK___SG_VALIDATE_SHADERDESC_NO_CONT_IMGS,
  YK___SG_VALIDATE_SHADERDESC_NO_CONT_UB_MEMBERS,
  YK___SG_VALIDATE_SHADERDESC_NO_UB_MEMBERS,
  YK___SG_VALIDATE_SHADERDESC_UB_MEMBER_NAME,
  YK___SG_VALIDATE_SHADERDESC_UB_SIZE_MISMATCH,
  YK___SG_VALIDATE_SHADERDESC_IMG_NAME,
  YK___SG_VALIDATE_SHADERDESC_ATTR_NAMES,
  YK___SG_VALIDATE_SHADERDESC_ATTR_SEMANTICS,
  YK___SG_VALIDATE_SHADERDESC_ATTR_STRING_TOO_LONG,
  /* pipeline creation */
  YK___SG_VALIDATE_PIPELINEDESC_CANARY,
  YK___SG_VALIDATE_PIPELINEDESC_SHADER,
  YK___SG_VALIDATE_PIPELINEDESC_NO_ATTRS,
  YK___SG_VALIDATE_PIPELINEDESC_LAYOUT_STRIDE4,
  YK___SG_VALIDATE_PIPELINEDESC_ATTR_NAME,
  YK___SG_VALIDATE_PIPELINEDESC_ATTR_SEMANTICS,
  /* pass creation */
  YK___SG_VALIDATE_PASSDESC_CANARY,
  YK___SG_VALIDATE_PASSDESC_NO_COLOR_ATTS,
  YK___SG_VALIDATE_PASSDESC_NO_CONT_COLOR_ATTS,
  YK___SG_VALIDATE_PASSDESC_IMAGE,
  YK___SG_VALIDATE_PASSDESC_MIPLEVEL,
  YK___SG_VALIDATE_PASSDESC_FACE,
  YK___SG_VALIDATE_PASSDESC_LAYER,
  YK___SG_VALIDATE_PASSDESC_SLICE,
  YK___SG_VALIDATE_PASSDESC_IMAGE_NO_RT,
  YK___SG_VALIDATE_PASSDESC_COLOR_INV_PIXELFORMAT,
  YK___SG_VALIDATE_PASSDESC_DEPTH_INV_PIXELFORMAT,
  YK___SG_VALIDATE_PASSDESC_IMAGE_SIZES,
  YK___SG_VALIDATE_PASSDESC_IMAGE_SAMPLE_COUNTS,
  /* yk__sg_begin_pass validation */
  YK___SG_VALIDATE_BEGINPASS_PASS,
  YK___SG_VALIDATE_BEGINPASS_IMAGE,
  /* yk__sg_apply_pipeline validation */
  YK___SG_VALIDATE_APIP_PIPELINE_VALID_ID,
  YK___SG_VALIDATE_APIP_PIPELINE_EXISTS,
  YK___SG_VALIDATE_APIP_PIPELINE_VALID,
  YK___SG_VALIDATE_APIP_SHADER_EXISTS,
  YK___SG_VALIDATE_APIP_SHADER_VALID,
  YK___SG_VALIDATE_APIP_ATT_COUNT,
  YK___SG_VALIDATE_APIP_COLOR_FORMAT,
  YK___SG_VALIDATE_APIP_DEPTH_FORMAT,
  YK___SG_VALIDATE_APIP_SAMPLE_COUNT,
  /* yk__sg_apply_bindings validation */
  YK___SG_VALIDATE_ABND_PIPELINE,
  YK___SG_VALIDATE_ABND_PIPELINE_EXISTS,
  YK___SG_VALIDATE_ABND_PIPELINE_VALID,
  YK___SG_VALIDATE_ABND_VBS,
  YK___SG_VALIDATE_ABND_VB_EXISTS,
  YK___SG_VALIDATE_ABND_VB_TYPE,
  YK___SG_VALIDATE_ABND_VB_OVERFLOW,
  YK___SG_VALIDATE_ABND_NO_IB,
  YK___SG_VALIDATE_ABND_IB,
  YK___SG_VALIDATE_ABND_IB_EXISTS,
  YK___SG_VALIDATE_ABND_IB_TYPE,
  YK___SG_VALIDATE_ABND_IB_OVERFLOW,
  YK___SG_VALIDATE_ABND_VS_IMGS,
  YK___SG_VALIDATE_ABND_VS_IMG_EXISTS,
  YK___SG_VALIDATE_ABND_VS_IMG_TYPES,
  YK___SG_VALIDATE_ABND_FS_IMGS,
  YK___SG_VALIDATE_ABND_FS_IMG_EXISTS,
  YK___SG_VALIDATE_ABND_FS_IMG_TYPES,
  /* yk__sg_apply_uniforms validation */
  YK___SG_VALIDATE_AUB_NO_PIPELINE,
  YK___SG_VALIDATE_AUB_NO_UB_AT_SLOT,
  YK___SG_VALIDATE_AUB_SIZE,
  /* yk__sg_update_buffer validation */
  YK___SG_VALIDATE_UPDATEBUF_USAGE,
  YK___SG_VALIDATE_UPDATEBUF_SIZE,
  YK___SG_VALIDATE_UPDATEBUF_ONCE,
  YK___SG_VALIDATE_UPDATEBUF_APPEND,
  /* yk__sg_append_buffer validation */
  YK___SG_VALIDATE_APPENDBUF_USAGE,
  YK___SG_VALIDATE_APPENDBUF_SIZE,
  YK___SG_VALIDATE_APPENDBUF_UPDATE,
  /* yk__sg_update_image validation */
  YK___SG_VALIDATE_UPDIMG_USAGE,
  YK___SG_VALIDATE_UPDIMG_NOTENOUGHDATA,
  YK___SG_VALIDATE_UPDIMG_ONCE
} yk___sg_validate_error_t;
/*=== GENERIC BACKEND STATE ==================================================*/
typedef struct {
  bool valid;
  yk__sg_desc desc; /* original desc with default values patched in */
  uint32_t frame_index;
  yk__sg_context active_context;
  yk__sg_pass cur_pass;
  yk__sg_pipeline cur_pipeline;
  bool pass_valid;
  bool bindings_valid;
  bool next_draw_valid;
#if defined(YK__SOKOL_DEBUG)
  yk___sg_validate_error_t validate_error;
#endif
  yk___sg_pools_t pools;
  yk__sg_backend backend;
  yk__sg_features features;
  yk__sg_limits limits;
  yk__sg_pixelformat_info formats[YK___SG_PIXELFORMAT_NUM];
#if defined(YK___SOKOL_ANY_GL)
  yk___sg_gl_backend_t gl;
#elif defined(YK__SOKOL_METAL)
  yk___sg_mtl_backend_t mtl;
#elif defined(YK__SOKOL_D3D11)
  yk___sg_d3d11_backend_t d3d11;
#elif defined(YK__SOKOL_WGPU)
  yk___sg_wgpu_backend_t wgpu;
#endif
#if defined(YK__SOKOL_TRACE_HOOKS)
  yk__sg_trace_hooks hooks;
#endif
} yk___sg_state_t;
static yk___sg_state_t yk___sg;
/*-- helper functions --------------------------------------------------------*/
YK___SOKOL_PRIVATE bool yk___sg_strempty(const yk___sg_str_t *str) {
  return 0 == str->buf[0];
}
YK___SOKOL_PRIVATE const char *yk___sg_strptr(const yk___sg_str_t *str) {
  return &str->buf[0];
}
YK___SOKOL_PRIVATE void yk___sg_strcpy(yk___sg_str_t *dst, const char *src) {
  YK__SOKOL_ASSERT(dst);
  if (src) {
#if defined(_MSC_VER)
    strncpy_s(dst->buf, YK___SG_STRING_SIZE, src, (YK___SG_STRING_SIZE - 1));
#else
    strncpy(dst->buf, src, YK___SG_STRING_SIZE);
#endif
    dst->buf[YK___SG_STRING_SIZE - 1] = 0;
  } else {
    memset(dst->buf, 0, YK___SG_STRING_SIZE);
  }
}
/* return byte size of a vertex format */
YK___SOKOL_PRIVATE int yk___sg_vertexformat_bytesize(yk__sg_vertex_format fmt) {
  switch (fmt) {
    case YK__SG_VERTEXFORMAT_FLOAT:
      return 4;
    case YK__SG_VERTEXFORMAT_FLOAT2:
      return 8;
    case YK__SG_VERTEXFORMAT_FLOAT3:
      return 12;
    case YK__SG_VERTEXFORMAT_FLOAT4:
      return 16;
    case YK__SG_VERTEXFORMAT_BYTE4:
      return 4;
    case YK__SG_VERTEXFORMAT_BYTE4N:
      return 4;
    case YK__SG_VERTEXFORMAT_UBYTE4:
      return 4;
    case YK__SG_VERTEXFORMAT_UBYTE4N:
      return 4;
    case YK__SG_VERTEXFORMAT_SHORT2:
      return 4;
    case YK__SG_VERTEXFORMAT_SHORT2N:
      return 4;
    case YK__SG_VERTEXFORMAT_USHORT2N:
      return 4;
    case YK__SG_VERTEXFORMAT_SHORT4:
      return 8;
    case YK__SG_VERTEXFORMAT_SHORT4N:
      return 8;
    case YK__SG_VERTEXFORMAT_USHORT4N:
      return 8;
    case YK__SG_VERTEXFORMAT_UINT10_N2:
      return 4;
    case YK__SG_VERTEXFORMAT_INVALID:
      return 0;
    default:
      YK__SOKOL_UNREACHABLE;
      return -1;
  }
}
/* return the byte size of a shader uniform */
YK___SOKOL_PRIVATE int yk___sg_uniform_size(yk__sg_uniform_type type,
                                            int count) {
  switch (type) {
    case YK__SG_UNIFORMTYPE_INVALID:
      return 0;
    case YK__SG_UNIFORMTYPE_FLOAT:
      return 4 * count;
    case YK__SG_UNIFORMTYPE_FLOAT2:
      return 8 * count;
    case YK__SG_UNIFORMTYPE_FLOAT3:
      return 12 * count; /* FIXME: std140??? */
    case YK__SG_UNIFORMTYPE_FLOAT4:
      return 16 * count;
    case YK__SG_UNIFORMTYPE_MAT4:
      return 64 * count;
    default:
      YK__SOKOL_UNREACHABLE;
      return -1;
  }
}
/* return true if pixel format is a compressed format */
YK___SOKOL_PRIVATE bool
yk___sg_is_compressed_pixel_format(yk__sg_pixel_format fmt) {
  switch (fmt) {
    case YK__SG_PIXELFORMAT_BC1_RGBA:
    case YK__SG_PIXELFORMAT_BC2_RGBA:
    case YK__SG_PIXELFORMAT_BC3_RGBA:
    case YK__SG_PIXELFORMAT_BC4_R:
    case YK__SG_PIXELFORMAT_BC4_RSN:
    case YK__SG_PIXELFORMAT_BC5_RG:
    case YK__SG_PIXELFORMAT_BC5_RGSN:
    case YK__SG_PIXELFORMAT_BC6H_RGBF:
    case YK__SG_PIXELFORMAT_BC6H_RGBUF:
    case YK__SG_PIXELFORMAT_BC7_RGBA:
    case YK__SG_PIXELFORMAT_PVRTC_RGB_2BPP:
    case YK__SG_PIXELFORMAT_PVRTC_RGB_4BPP:
    case YK__SG_PIXELFORMAT_PVRTC_RGBA_2BPP:
    case YK__SG_PIXELFORMAT_PVRTC_RGBA_4BPP:
    case YK__SG_PIXELFORMAT_ETC2_RGB8:
    case YK__SG_PIXELFORMAT_ETC2_RGB8A1:
    case YK__SG_PIXELFORMAT_ETC2_RGBA8:
    case YK__SG_PIXELFORMAT_ETC2_RG11:
    case YK__SG_PIXELFORMAT_ETC2_RG11SN:
      return true;
    default:
      return false;
  }
}
/* return true if pixel format is a valid render target format */
YK___SOKOL_PRIVATE bool
yk___sg_is_valid_rendertarget_color_format(yk__sg_pixel_format fmt) {
  const int fmt_index = (int) fmt;
  YK__SOKOL_ASSERT((fmt_index >= 0) && (fmt_index < YK___SG_PIXELFORMAT_NUM));
  return yk___sg.formats[fmt_index].render && !yk___sg.formats[fmt_index].depth;
}
/* return true if pixel format is a valid depth format */
YK___SOKOL_PRIVATE bool
yk___sg_is_valid_rendertarget_depth_format(yk__sg_pixel_format fmt) {
  const int fmt_index = (int) fmt;
  YK__SOKOL_ASSERT((fmt_index >= 0) && (fmt_index < YK___SG_PIXELFORMAT_NUM));
  return yk___sg.formats[fmt_index].render && yk___sg.formats[fmt_index].depth;
}
/* return true if pixel format is a depth-stencil format */
YK___SOKOL_PRIVATE bool
yk___sg_is_depth_stencil_format(yk__sg_pixel_format fmt) {
  return (YK__SG_PIXELFORMAT_DEPTH_STENCIL == fmt);
}
/* return the bytes-per-pixel for a pixel format */
YK___SOKOL_PRIVATE int yk___sg_pixelformat_bytesize(yk__sg_pixel_format fmt) {
  switch (fmt) {
    case YK__SG_PIXELFORMAT_R8:
    case YK__SG_PIXELFORMAT_R8SN:
    case YK__SG_PIXELFORMAT_R8UI:
    case YK__SG_PIXELFORMAT_R8SI:
      return 1;
    case YK__SG_PIXELFORMAT_R16:
    case YK__SG_PIXELFORMAT_R16SN:
    case YK__SG_PIXELFORMAT_R16UI:
    case YK__SG_PIXELFORMAT_R16SI:
    case YK__SG_PIXELFORMAT_R16F:
    case YK__SG_PIXELFORMAT_RG8:
    case YK__SG_PIXELFORMAT_RG8SN:
    case YK__SG_PIXELFORMAT_RG8UI:
    case YK__SG_PIXELFORMAT_RG8SI:
      return 2;
    case YK__SG_PIXELFORMAT_R32UI:
    case YK__SG_PIXELFORMAT_R32SI:
    case YK__SG_PIXELFORMAT_R32F:
    case YK__SG_PIXELFORMAT_RG16:
    case YK__SG_PIXELFORMAT_RG16SN:
    case YK__SG_PIXELFORMAT_RG16UI:
    case YK__SG_PIXELFORMAT_RG16SI:
    case YK__SG_PIXELFORMAT_RG16F:
    case YK__SG_PIXELFORMAT_RGBA8:
    case YK__SG_PIXELFORMAT_RGBA8SN:
    case YK__SG_PIXELFORMAT_RGBA8UI:
    case YK__SG_PIXELFORMAT_RGBA8SI:
    case YK__SG_PIXELFORMAT_BGRA8:
    case YK__SG_PIXELFORMAT_RGB10A2:
    case YK__SG_PIXELFORMAT_RG11B10F:
      return 4;
    case YK__SG_PIXELFORMAT_RG32UI:
    case YK__SG_PIXELFORMAT_RG32SI:
    case YK__SG_PIXELFORMAT_RG32F:
    case YK__SG_PIXELFORMAT_RGBA16:
    case YK__SG_PIXELFORMAT_RGBA16SN:
    case YK__SG_PIXELFORMAT_RGBA16UI:
    case YK__SG_PIXELFORMAT_RGBA16SI:
    case YK__SG_PIXELFORMAT_RGBA16F:
      return 8;
    case YK__SG_PIXELFORMAT_RGBA32UI:
    case YK__SG_PIXELFORMAT_RGBA32SI:
    case YK__SG_PIXELFORMAT_RGBA32F:
      return 16;
    default:
      YK__SOKOL_UNREACHABLE;
      return 0;
  }
}
YK___SOKOL_PRIVATE int yk___sg_roundup(int val, int round_to) {
  return (val + (round_to - 1)) & ~(round_to - 1);
}
/* return row pitch for an image

    see ComputePitch in https://github.com/microsoft/DirectXTex/blob/master/DirectXTex/DirectXTexUtil.cpp

    For the special PVRTC pitch computation, see:
    GL extension requirement (https://www.khronos.org/registry/OpenGL/extensions/IMG/IMG_texture_compression_pvrtc.txt)

    Quote:

    6) How is the imageSize argument calculated for the CompressedTexImage2D
       and CompressedTexSubImage2D functions.

       Resolution: For PVRTC 4BPP formats the imageSize is calculated as:
          ( max(width, 8) * max(height, 8) * 4 + 7) / 8
       For PVRTC 2BPP formats the imageSize is calculated as:
          ( max(width, 16) * max(height, 8) * 2 + 7) / 8
*/
YK___SOKOL_PRIVATE int yk___sg_row_pitch(yk__sg_pixel_format fmt, int width,
                                         int row_align) {
  int pitch;
  switch (fmt) {
    case YK__SG_PIXELFORMAT_BC1_RGBA:
    case YK__SG_PIXELFORMAT_BC4_R:
    case YK__SG_PIXELFORMAT_BC4_RSN:
    case YK__SG_PIXELFORMAT_ETC2_RGB8:
    case YK__SG_PIXELFORMAT_ETC2_RGB8A1:
      pitch = ((width + 3) / 4) * 8;
      pitch = pitch < 8 ? 8 : pitch;
      break;
    case YK__SG_PIXELFORMAT_BC2_RGBA:
    case YK__SG_PIXELFORMAT_BC3_RGBA:
    case YK__SG_PIXELFORMAT_BC5_RG:
    case YK__SG_PIXELFORMAT_BC5_RGSN:
    case YK__SG_PIXELFORMAT_BC6H_RGBF:
    case YK__SG_PIXELFORMAT_BC6H_RGBUF:
    case YK__SG_PIXELFORMAT_BC7_RGBA:
    case YK__SG_PIXELFORMAT_ETC2_RGBA8:
    case YK__SG_PIXELFORMAT_ETC2_RG11:
    case YK__SG_PIXELFORMAT_ETC2_RG11SN:
      pitch = ((width + 3) / 4) * 16;
      pitch = pitch < 16 ? 16 : pitch;
      break;
    case YK__SG_PIXELFORMAT_PVRTC_RGB_4BPP:
    case YK__SG_PIXELFORMAT_PVRTC_RGBA_4BPP:
      pitch = (yk___sg_max(width, 8) * 4 + 7) / 8;
      break;
    case YK__SG_PIXELFORMAT_PVRTC_RGB_2BPP:
    case YK__SG_PIXELFORMAT_PVRTC_RGBA_2BPP:
      pitch = (yk___sg_max(width, 16) * 2 + 7) / 8;
      break;
    default:
      pitch = width * yk___sg_pixelformat_bytesize(fmt);
      break;
  }
  pitch = yk___sg_roundup(pitch, row_align);
  return pitch;
}
/* compute the number of rows in a surface depending on pixel format */
YK___SOKOL_PRIVATE int yk___sg_num_rows(yk__sg_pixel_format fmt, int height) {
  int num_rows;
  switch (fmt) {
    case YK__SG_PIXELFORMAT_BC1_RGBA:
    case YK__SG_PIXELFORMAT_BC4_R:
    case YK__SG_PIXELFORMAT_BC4_RSN:
    case YK__SG_PIXELFORMAT_ETC2_RGB8:
    case YK__SG_PIXELFORMAT_ETC2_RGB8A1:
    case YK__SG_PIXELFORMAT_ETC2_RGBA8:
    case YK__SG_PIXELFORMAT_ETC2_RG11:
    case YK__SG_PIXELFORMAT_ETC2_RG11SN:
    case YK__SG_PIXELFORMAT_BC2_RGBA:
    case YK__SG_PIXELFORMAT_BC3_RGBA:
    case YK__SG_PIXELFORMAT_BC5_RG:
    case YK__SG_PIXELFORMAT_BC5_RGSN:
    case YK__SG_PIXELFORMAT_BC6H_RGBF:
    case YK__SG_PIXELFORMAT_BC6H_RGBUF:
    case YK__SG_PIXELFORMAT_BC7_RGBA:
      num_rows = ((height + 3) / 4);
      break;
    case YK__SG_PIXELFORMAT_PVRTC_RGB_4BPP:
    case YK__SG_PIXELFORMAT_PVRTC_RGBA_4BPP:
    case YK__SG_PIXELFORMAT_PVRTC_RGB_2BPP:
    case YK__SG_PIXELFORMAT_PVRTC_RGBA_2BPP:
      /* NOTE: this is most likely not correct because it ignores any
                PVCRTC block size, but multiplied with yk___sg_row_pitch()
                it gives the correct surface pitch.

                See: https://www.khronos.org/registry/OpenGL/extensions/IMG/IMG_texture_compression_pvrtc.txt
            */
      num_rows = ((yk___sg_max(height, 8) + 7) / 8) * 8;
      break;
    default:
      num_rows = height;
      break;
  }
  if (num_rows < 1) { num_rows = 1; }
  return num_rows;
}
/* return pitch of a 2D subimage / texture slice
    see ComputePitch in https://github.com/microsoft/DirectXTex/blob/master/DirectXTex/DirectXTexUtil.cpp
*/
YK___SOKOL_PRIVATE int yk___sg_surface_pitch(yk__sg_pixel_format fmt, int width,
                                             int height, int row_align) {
  int num_rows = yk___sg_num_rows(fmt, height);
  return num_rows * yk___sg_row_pitch(fmt, width, row_align);
}
/* capability table pixel format helper functions */
YK___SOKOL_PRIVATE void yk___sg_pixelformat_all(yk__sg_pixelformat_info *pfi) {
  pfi->sample = true;
  pfi->filter = true;
  pfi->blend = true;
  pfi->render = true;
  pfi->msaa = true;
}
YK___SOKOL_PRIVATE void yk___sg_pixelformat_s(yk__sg_pixelformat_info *pfi) {
  pfi->sample = true;
}
YK___SOKOL_PRIVATE void yk___sg_pixelformat_sf(yk__sg_pixelformat_info *pfi) {
  pfi->sample = true;
  pfi->filter = true;
}
YK___SOKOL_PRIVATE void yk___sg_pixelformat_sr(yk__sg_pixelformat_info *pfi) {
  pfi->sample = true;
  pfi->render = true;
}
YK___SOKOL_PRIVATE void yk___sg_pixelformat_srmd(yk__sg_pixelformat_info *pfi) {
  pfi->sample = true;
  pfi->render = true;
  pfi->msaa = true;
  pfi->depth = true;
}
YK___SOKOL_PRIVATE void yk___sg_pixelformat_srm(yk__sg_pixelformat_info *pfi) {
  pfi->sample = true;
  pfi->render = true;
  pfi->msaa = true;
}
YK___SOKOL_PRIVATE void yk___sg_pixelformat_sfrm(yk__sg_pixelformat_info *pfi) {
  pfi->sample = true;
  pfi->filter = true;
  pfi->render = true;
  pfi->msaa = true;
}
YK___SOKOL_PRIVATE void yk___sg_pixelformat_sbrm(yk__sg_pixelformat_info *pfi) {
  pfi->sample = true;
  pfi->blend = true;
  pfi->render = true;
  pfi->msaa = true;
}
YK___SOKOL_PRIVATE void yk___sg_pixelformat_sbr(yk__sg_pixelformat_info *pfi) {
  pfi->sample = true;
  pfi->blend = true;
  pfi->render = true;
}
YK___SOKOL_PRIVATE void yk___sg_pixelformat_sfbr(yk__sg_pixelformat_info *pfi) {
  pfi->sample = true;
  pfi->filter = true;
  pfi->blend = true;
  pfi->render = true;
}
/* resolve pass action defaults into a new pass action struct */
YK___SOKOL_PRIVATE void
yk___sg_resolve_default_pass_action(const yk__sg_pass_action *from,
                                    yk__sg_pass_action *to) {
  YK__SOKOL_ASSERT(from && to);
  *to = *from;
  for (int i = 0; i < YK__SG_MAX_COLOR_ATTACHMENTS; i++) {
    if (to->colors[i].action == YK___SG_ACTION_DEFAULT) {
      to->colors[i].action = YK__SG_ACTION_CLEAR;
      to->colors[i].value.r = YK__SG_DEFAULT_CLEAR_RED;
      to->colors[i].value.g = YK__SG_DEFAULT_CLEAR_GREEN;
      to->colors[i].value.b = YK__SG_DEFAULT_CLEAR_BLUE;
      to->colors[i].value.a = YK__SG_DEFAULT_CLEAR_ALPHA;
    }
  }
  if (to->depth.action == YK___SG_ACTION_DEFAULT) {
    to->depth.action = YK__SG_ACTION_CLEAR;
    to->depth.value = YK__SG_DEFAULT_CLEAR_DEPTH;
  }
  if (to->stencil.action == YK___SG_ACTION_DEFAULT) {
    to->stencil.action = YK__SG_ACTION_CLEAR;
    to->stencil.value = YK__SG_DEFAULT_CLEAR_STENCIL;
  }
}
/*== DUMMY BACKEND IMPL ======================================================*/
#if defined(YK__SOKOL_DUMMY_BACKEND)
YK___SOKOL_PRIVATE void yk___sg_dummy_setup_backend(const yk__sg_desc *desc) {
  YK__SOKOL_ASSERT(desc);
  YK___SOKOL_UNUSED(desc);
  yk___sg.backend = YK__SG_BACKEND_DUMMY;
  for (int i = YK__SG_PIXELFORMAT_R8; i < YK__SG_PIXELFORMAT_BC1_RGBA; i++) {
    yk___sg.formats[i].sample = true;
    yk___sg.formats[i].filter = true;
    yk___sg.formats[i].render = true;
    yk___sg.formats[i].blend = true;
    yk___sg.formats[i].msaa = true;
  }
  yk___sg.formats[YK__SG_PIXELFORMAT_DEPTH].depth = true;
  yk___sg.formats[YK__SG_PIXELFORMAT_DEPTH_STENCIL].depth = true;
}
YK___SOKOL_PRIVATE void yk___sg_dummy_discard_backend(void) { /* empty */ }
YK___SOKOL_PRIVATE void yk___sg_dummy_reset_state_cache(void) { /* empty*/ }
YK___SOKOL_PRIVATE yk__sg_resource_state
yk___sg_dummy_create_context(yk___sg_context_t *ctx) {
  YK__SOKOL_ASSERT(ctx);
  YK___SOKOL_UNUSED(ctx);
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_dummy_destroy_context(yk___sg_context_t *ctx) {
  YK__SOKOL_ASSERT(ctx);
  YK___SOKOL_UNUSED(ctx);
}
YK___SOKOL_PRIVATE void yk___sg_dummy_activate_context(yk___sg_context_t *ctx) {
  YK__SOKOL_ASSERT(ctx);
  YK___SOKOL_UNUSED(ctx);
}
YK___SOKOL_PRIVATE yk__sg_resource_state yk___sg_dummy_create_buffer(
    yk___sg_buffer_t *buf, const yk__sg_buffer_desc *desc) {
  YK__SOKOL_ASSERT(buf && desc);
  yk___sg_buffer_common_init(&buf->cmn, desc);
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_dummy_destroy_buffer(yk___sg_buffer_t *buf) {
  YK__SOKOL_ASSERT(buf);
  YK___SOKOL_UNUSED(buf);
}
YK___SOKOL_PRIVATE yk__sg_resource_state yk___sg_dummy_create_image(
    yk___sg_image_t *img, const yk__sg_image_desc *desc) {
  YK__SOKOL_ASSERT(img && desc);
  yk___sg_image_common_init(&img->cmn, desc);
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_dummy_destroy_image(yk___sg_image_t *img) {
  YK__SOKOL_ASSERT(img);
  YK___SOKOL_UNUSED(img);
}
YK___SOKOL_PRIVATE yk__sg_resource_state yk___sg_dummy_create_shader(
    yk___sg_shader_t *shd, const yk__sg_shader_desc *desc) {
  YK__SOKOL_ASSERT(shd && desc);
  yk___sg_shader_common_init(&shd->cmn, desc);
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_dummy_destroy_shader(yk___sg_shader_t *shd) {
  YK__SOKOL_ASSERT(shd);
  YK___SOKOL_UNUSED(shd);
}
YK___SOKOL_PRIVATE yk__sg_resource_state
yk___sg_dummy_create_pipeline(yk___sg_pipeline_t *pip, yk___sg_shader_t *shd,
                              const yk__sg_pipeline_desc *desc) {
  YK__SOKOL_ASSERT(pip && desc);
  pip->shader = shd;
  yk___sg_pipeline_common_init(&pip->cmn, desc);
  for (int attr_index = 0; attr_index < YK__SG_MAX_VERTEX_ATTRIBUTES;
       attr_index++) {
    const yk__sg_vertex_attr_desc *a_desc = &desc->layout.attrs[attr_index];
    if (a_desc->format == YK__SG_VERTEXFORMAT_INVALID) { break; }
    YK__SOKOL_ASSERT(a_desc->buffer_index < YK__SG_MAX_SHADERSTAGE_BUFFERS);
    pip->cmn.vertex_layout_valid[a_desc->buffer_index] = true;
  }
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void
yk___sg_dummy_destroy_pipeline(yk___sg_pipeline_t *pip) {
  YK__SOKOL_ASSERT(pip);
  YK___SOKOL_UNUSED(pip);
}
YK___SOKOL_PRIVATE yk__sg_resource_state
yk___sg_dummy_create_pass(yk___sg_pass_t *pass, yk___sg_image_t **att_images,
                          const yk__sg_pass_desc *desc) {
  YK__SOKOL_ASSERT(pass && desc);
  YK__SOKOL_ASSERT(att_images && att_images[0]);
  yk___sg_pass_common_init(&pass->cmn, desc);
  const yk__sg_pass_attachment_desc *att_desc;
  for (int i = 0; i < pass->cmn.num_color_atts; i++) {
    att_desc = &desc->color_attachments[i];
    YK__SOKOL_ASSERT(att_desc->image.id != YK__SG_INVALID_ID);
    YK__SOKOL_ASSERT(0 == pass->dmy.color_atts[i].image);
    YK__SOKOL_ASSERT(att_images[i] &&
                     (att_images[i]->slot.id == att_desc->image.id));
    YK__SOKOL_ASSERT(yk___sg_is_valid_rendertarget_color_format(
        att_images[i]->cmn.pixel_format));
    pass->dmy.color_atts[i].image = att_images[i];
  }
  YK__SOKOL_ASSERT(0 == pass->dmy.ds_att.image);
  att_desc = &desc->depth_stencil_attachment;
  if (att_desc->image.id != YK__SG_INVALID_ID) {
    const int ds_img_index = YK__SG_MAX_COLOR_ATTACHMENTS;
    YK__SOKOL_ASSERT(att_images[ds_img_index] &&
                     (att_images[ds_img_index]->slot.id == att_desc->image.id));
    YK__SOKOL_ASSERT(yk___sg_is_valid_rendertarget_depth_format(
        att_images[ds_img_index]->cmn.pixel_format));
    pass->dmy.ds_att.image = att_images[ds_img_index];
  }
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_dummy_destroy_pass(yk___sg_pass_t *pass) {
  YK__SOKOL_ASSERT(pass);
  YK___SOKOL_UNUSED(pass);
}
YK___SOKOL_PRIVATE yk___sg_image_t *
yk___sg_dummy_pass_color_image(const yk___sg_pass_t *pass, int index) {
  YK__SOKOL_ASSERT(pass && (index >= 0) &&
                   (index < YK__SG_MAX_COLOR_ATTACHMENTS));
  /* NOTE: may return null */
  return pass->dmy.color_atts[index].image;
}
YK___SOKOL_PRIVATE yk___sg_image_t *
yk___sg_dummy_pass_ds_image(const yk___sg_pass_t *pass) {
  /* NOTE: may return null */
  YK__SOKOL_ASSERT(pass);
  return pass->dmy.ds_att.image;
}
YK___SOKOL_PRIVATE void
yk___sg_dummy_begin_pass(yk___sg_pass_t *pass, const yk__sg_pass_action *action,
                         int w, int h) {
  YK__SOKOL_ASSERT(action);
  YK___SOKOL_UNUSED(pass);
  YK___SOKOL_UNUSED(action);
  YK___SOKOL_UNUSED(w);
  YK___SOKOL_UNUSED(h);
}
YK___SOKOL_PRIVATE void yk___sg_dummy_end_pass(void) { /* empty */ }
YK___SOKOL_PRIVATE void yk___sg_dummy_commit(void) { /* empty */ }
YK___SOKOL_PRIVATE void yk___sg_dummy_apply_viewport(int x, int y, int w, int h,
                                                     bool origin_top_left) {
  YK___SOKOL_UNUSED(x);
  YK___SOKOL_UNUSED(y);
  YK___SOKOL_UNUSED(w);
  YK___SOKOL_UNUSED(h);
  YK___SOKOL_UNUSED(origin_top_left);
}
YK___SOKOL_PRIVATE void yk___sg_dummy_apply_scissor_rect(int x, int y, int w,
                                                         int h,
                                                         bool origin_top_left) {
  YK___SOKOL_UNUSED(x);
  YK___SOKOL_UNUSED(y);
  YK___SOKOL_UNUSED(w);
  YK___SOKOL_UNUSED(h);
  YK___SOKOL_UNUSED(origin_top_left);
}
YK___SOKOL_PRIVATE void yk___sg_dummy_apply_pipeline(yk___sg_pipeline_t *pip) {
  YK__SOKOL_ASSERT(pip);
  YK___SOKOL_UNUSED(pip);
}
YK___SOKOL_PRIVATE void yk___sg_dummy_apply_bindings(
    yk___sg_pipeline_t *pip, yk___sg_buffer_t **vbs, const int *vb_offsets,
    int num_vbs, yk___sg_buffer_t *ib, int ib_offset, yk___sg_image_t **vs_imgs,
    int num_vs_imgs, yk___sg_image_t **fs_imgs, int num_fs_imgs) {
  YK__SOKOL_ASSERT(pip);
  YK__SOKOL_ASSERT(vbs && vb_offsets);
  YK__SOKOL_ASSERT(vs_imgs);
  YK__SOKOL_ASSERT(fs_imgs);
  YK___SOKOL_UNUSED(pip);
  YK___SOKOL_UNUSED(vbs);
  YK___SOKOL_UNUSED(vb_offsets);
  YK___SOKOL_UNUSED(num_vbs);
  YK___SOKOL_UNUSED(ib);
  YK___SOKOL_UNUSED(ib_offset);
  YK___SOKOL_UNUSED(vs_imgs);
  YK___SOKOL_UNUSED(num_vs_imgs);
  YK___SOKOL_UNUSED(fs_imgs);
  YK___SOKOL_UNUSED(num_fs_imgs);
}
YK___SOKOL_PRIVATE void
yk___sg_dummy_apply_uniforms(yk__sg_shader_stage stage_index, int ub_index,
                             const yk__sg_range *data) {
  YK___SOKOL_UNUSED(stage_index);
  YK___SOKOL_UNUSED(ub_index);
  YK___SOKOL_UNUSED(data);
}
YK___SOKOL_PRIVATE void yk___sg_dummy_draw(int base_element, int num_elements,
                                           int num_instances) {
  YK___SOKOL_UNUSED(base_element);
  YK___SOKOL_UNUSED(num_elements);
  YK___SOKOL_UNUSED(num_instances);
}
YK___SOKOL_PRIVATE void yk___sg_dummy_update_buffer(yk___sg_buffer_t *buf,
                                                    const yk__sg_range *data) {
  YK__SOKOL_ASSERT(buf && data && data->ptr && (data->size > 0));
  YK___SOKOL_UNUSED(data);
  if (++buf->cmn.active_slot >= buf->cmn.num_slots) {
    buf->cmn.active_slot = 0;
  }
}
YK___SOKOL_PRIVATE int yk___sg_dummy_append_buffer(yk___sg_buffer_t *buf,
                                                   const yk__sg_range *data,
                                                   bool new_frame) {
  YK__SOKOL_ASSERT(buf && data && data->ptr && (data->size > 0));
  YK___SOKOL_UNUSED(data);
  if (new_frame) {
    if (++buf->cmn.active_slot >= buf->cmn.num_slots) {
      buf->cmn.active_slot = 0;
    }
  }
  /* NOTE: this is a requirement from WebGPU, but we want identical behaviour across all backend */
  return yk___sg_roundup((int) data->size, 4);
}
YK___SOKOL_PRIVATE void
yk___sg_dummy_update_image(yk___sg_image_t *img,
                           const yk__sg_image_data *data) {
  YK__SOKOL_ASSERT(img && data);
  YK___SOKOL_UNUSED(data);
  if (++img->cmn.active_slot >= img->cmn.num_slots) {
    img->cmn.active_slot = 0;
  }
}
/*== GL BACKEND ==============================================================*/
#elif defined(YK___SOKOL_ANY_GL)
/*=== OPTIONAL GL LOADER FOR WIN32 ===========================================*/
#if defined(YK___SOKOL_USE_WIN32_GL_LOADER)
// X Macro list of GL function names and signatures
#define YK___SG_GL_FUNCS                                                       \
  YK___SG_XMACRO(glBindVertexArray, void, (GLuint array))                      \
  YK___SG_XMACRO(glFramebufferTextureLayer, void,                              \
                 (GLenum target, GLenum attachment, GLuint texture,            \
                  GLint level, GLint layer))                                   \
  YK___SG_XMACRO(glGenFramebuffers, void, (GLsizei n, GLuint * framebuffers))  \
  YK___SG_XMACRO(glBindFramebuffer, void, (GLenum target, GLuint framebuffer)) \
  YK___SG_XMACRO(glBindRenderbuffer, void,                                     \
                 (GLenum target, GLuint renderbuffer))                         \
  YK___SG_XMACRO(glGetStringi, const GLubyte *, (GLenum name, GLuint index))   \
  YK___SG_XMACRO(                                                              \
      glClearBufferfi, void,                                                   \
      (GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil))         \
  YK___SG_XMACRO(glClearBufferfv, void,                                        \
                 (GLenum buffer, GLint drawbuffer, const GLfloat *value))      \
  YK___SG_XMACRO(glClearBufferuiv, void,                                       \
                 (GLenum buffer, GLint drawbuffer, const GLuint *value))       \
  YK___SG_XMACRO(glClearBufferiv, void,                                        \
                 (GLenum buffer, GLint drawbuffer, const GLint *value))        \
  YK___SG_XMACRO(glDeleteRenderbuffers, void,                                  \
                 (GLsizei n, const GLuint *renderbuffers))                     \
  YK___SG_XMACRO(glUniform4fv, void,                                           \
                 (GLint location, GLsizei count, const GLfloat *value))        \
  YK___SG_XMACRO(glUniform2fv, void,                                           \
                 (GLint location, GLsizei count, const GLfloat *value))        \
  YK___SG_XMACRO(glUseProgram, void, (GLuint program))                         \
  YK___SG_XMACRO(glShaderSource, void,                                         \
                 (GLuint shader, GLsizei count, const GLchar *const *string,   \
                  const GLint *length))                                        \
  YK___SG_XMACRO(glLinkProgram, void, (GLuint program))                        \
  YK___SG_XMACRO(glGetUniformLocation, GLint,                                  \
                 (GLuint program, const GLchar *name))                         \
  YK___SG_XMACRO(glGetShaderiv, void,                                          \
                 (GLuint shader, GLenum pname, GLint * params))                \
  YK___SG_XMACRO(                                                              \
      glGetProgramInfoLog, void,                                               \
      (GLuint program, GLsizei bufSize, GLsizei * length, GLchar * infoLog))   \
  YK___SG_XMACRO(glGetAttribLocation, GLint,                                   \
                 (GLuint program, const GLchar *name))                         \
  YK___SG_XMACRO(glDisableVertexAttribArray, void, (GLuint index))             \
  YK___SG_XMACRO(glDeleteShader, void, (GLuint shader))                        \
  YK___SG_XMACRO(glDeleteProgram, void, (GLuint program))                      \
  YK___SG_XMACRO(glCompileShader, void, (GLuint shader))                       \
  YK___SG_XMACRO(glStencilFuncSeparate, void,                                  \
                 (GLenum face, GLenum func, GLint ref, GLuint mask))           \
  YK___SG_XMACRO(glStencilOpSeparate, void,                                    \
                 (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass))    \
  YK___SG_XMACRO(glRenderbufferStorageMultisample, void,                       \
                 (GLenum target, GLsizei samples, GLenum internalformat,       \
                  GLsizei width, GLsizei height))                              \
  YK___SG_XMACRO(glDrawBuffers, void, (GLsizei n, const GLenum *bufs))         \
  YK___SG_XMACRO(glVertexAttribDivisor, void, (GLuint index, GLuint divisor))  \
  YK___SG_XMACRO(                                                              \
      glBufferSubData, void,                                                   \
      (GLenum target, GLintptr offset, GLsizeiptr size, const void *data))     \
  YK___SG_XMACRO(glGenBuffers, void, (GLsizei n, GLuint * buffers))            \
  YK___SG_XMACRO(glCheckFramebufferStatus, GLenum, (GLenum target))            \
  YK___SG_XMACRO(glFramebufferRenderbuffer, void,                              \
                 (GLenum target, GLenum attachment, GLenum renderbuffertarget, \
                  GLuint renderbuffer))                                        \
  YK___SG_XMACRO(glCompressedTexImage2D, void,                                 \
                 (GLenum target, GLint level, GLenum internalformat,           \
                  GLsizei width, GLsizei height, GLint border,                 \
                  GLsizei imageSize, const void *data))                        \
  YK___SG_XMACRO(glCompressedTexImage3D, void,                                 \
                 (GLenum target, GLint level, GLenum internalformat,           \
                  GLsizei width, GLsizei height, GLsizei depth, GLint border,  \
                  GLsizei imageSize, const void *data))                        \
  YK___SG_XMACRO(glActiveTexture, void, (GLenum texture))                      \
  YK___SG_XMACRO(glTexSubImage3D, void,                                        \
                 (GLenum target, GLint level, GLint xoffset, GLint yoffset,    \
                  GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, \
                  GLenum format, GLenum type, const void *pixels))             \
  YK___SG_XMACRO(glUniformMatrix4fv, void,                                     \
                 (GLint location, GLsizei count, GLboolean transpose,          \
                  const GLfloat *value))                                       \
  YK___SG_XMACRO(                                                              \
      glRenderbufferStorage, void,                                             \
      (GLenum target, GLenum internalformat, GLsizei width, GLsizei height))   \
  YK___SG_XMACRO(glGenTextures, void, (GLsizei n, GLuint * textures))          \
  YK___SG_XMACRO(glPolygonOffset, void, (GLfloat factor, GLfloat units))       \
  YK___SG_XMACRO(                                                              \
      glDrawElements, void,                                                    \
      (GLenum mode, GLsizei count, GLenum type, const void *indices))          \
  YK___SG_XMACRO(glDeleteFramebuffers, void,                                   \
                 (GLsizei n, const GLuint *framebuffers))                      \
  YK___SG_XMACRO(glBlendEquationSeparate, void,                                \
                 (GLenum modeRGB, GLenum modeAlpha))                           \
  YK___SG_XMACRO(glDeleteTextures, void, (GLsizei n, const GLuint *textures))  \
  YK___SG_XMACRO(glGetProgramiv, void,                                         \
                 (GLuint program, GLenum pname, GLint * params))               \
  YK___SG_XMACRO(glBindTexture, void, (GLenum target, GLuint texture))         \
  YK___SG_XMACRO(glTexImage3D, void,                                           \
                 (GLenum target, GLint level, GLint internalformat,            \
                  GLsizei width, GLsizei height, GLsizei depth, GLint border,  \
                  GLenum format, GLenum type, const void *pixels))             \
  YK___SG_XMACRO(glCreateShader, GLuint, (GLenum type))                        \
  YK___SG_XMACRO(glTexSubImage2D, void,                                        \
                 (GLenum target, GLint level, GLint xoffset, GLint yoffset,    \
                  GLsizei width, GLsizei height, GLenum format, GLenum type,   \
                  const void *pixels))                                         \
  YK___SG_XMACRO(glClearDepth, void, (GLdouble depth))                         \
  YK___SG_XMACRO(glFramebufferTexture2D, void,                                 \
                 (GLenum target, GLenum attachment, GLenum textarget,          \
                  GLuint texture, GLint level))                                \
  YK___SG_XMACRO(glCreateProgram, GLuint, (void) )                             \
  YK___SG_XMACRO(glViewport, void,                                             \
                 (GLint x, GLint y, GLsizei width, GLsizei height))            \
  YK___SG_XMACRO(glDeleteBuffers, void, (GLsizei n, const GLuint *buffers))    \
  YK___SG_XMACRO(glDrawArrays, void,                                           \
                 (GLenum mode, GLint first, GLsizei count))                    \
  YK___SG_XMACRO(glDrawElementsInstanced, void,                                \
                 (GLenum mode, GLsizei count, GLenum type,                     \
                  const void *indices, GLsizei instancecount))                 \
  YK___SG_XMACRO(glVertexAttribPointer, void,                                  \
                 (GLuint index, GLint size, GLenum type, GLboolean normalized, \
                  GLsizei stride, const void *pointer))                        \
  YK___SG_XMACRO(glUniform1i, void, (GLint location, GLint v0))                \
  YK___SG_XMACRO(glDisable, void, (GLenum cap))                                \
  YK___SG_XMACRO(                                                              \
      glColorMask, void,                                                       \
      (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha))       \
  YK___SG_XMACRO(glColorMaski, void,                                           \
                 (GLuint buf, GLboolean red, GLboolean green, GLboolean blue,  \
                  GLboolean alpha))                                            \
  YK___SG_XMACRO(glBindBuffer, void, (GLenum target, GLuint buffer))           \
  YK___SG_XMACRO(glDeleteVertexArrays, void,                                   \
                 (GLsizei n, const GLuint *arrays))                            \
  YK___SG_XMACRO(glDepthMask, void, (GLboolean flag))                          \
  YK___SG_XMACRO(                                                              \
      glDrawArraysInstanced, void,                                             \
      (GLenum mode, GLint first, GLsizei count, GLsizei instancecount))        \
  YK___SG_XMACRO(glClearStencil, void, (GLint s))                              \
  YK___SG_XMACRO(glScissor, void,                                              \
                 (GLint x, GLint y, GLsizei width, GLsizei height))            \
  YK___SG_XMACRO(glUniform3fv, void,                                           \
                 (GLint location, GLsizei count, const GLfloat *value))        \
  YK___SG_XMACRO(glGenRenderbuffers, void,                                     \
                 (GLsizei n, GLuint * renderbuffers))                          \
  YK___SG_XMACRO(                                                              \
      glBufferData, void,                                                      \
      (GLenum target, GLsizeiptr size, const void *data, GLenum usage))        \
  YK___SG_XMACRO(glBlendFuncSeparate, void,                                    \
                 (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha,   \
                  GLenum dfactorAlpha))                                        \
  YK___SG_XMACRO(glTexParameteri, void,                                        \
                 (GLenum target, GLenum pname, GLint param))                   \
  YK___SG_XMACRO(glGetIntegerv, void, (GLenum pname, GLint * data))            \
  YK___SG_XMACRO(glEnable, void, (GLenum cap))                                 \
  YK___SG_XMACRO(glBlitFramebuffer, void,                                      \
                 (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,          \
                  GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,          \
                  GLbitfield mask, GLenum filter))                             \
  YK___SG_XMACRO(glStencilMask, void, (GLuint mask))                           \
  YK___SG_XMACRO(glAttachShader, void, (GLuint program, GLuint shader))        \
  YK___SG_XMACRO(glGetError, GLenum, (void) )                                  \
  YK___SG_XMACRO(glClearColor, void,                                           \
                 (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha))    \
  YK___SG_XMACRO(glBlendColor, void,                                           \
                 (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha))    \
  YK___SG_XMACRO(glTexParameterf, void,                                        \
                 (GLenum target, GLenum pname, GLfloat param))                 \
  YK___SG_XMACRO(glTexParameterfv, void,                                       \
                 (GLenum target, GLenum pname, GLfloat * params))              \
  YK___SG_XMACRO(                                                              \
      glGetShaderInfoLog, void,                                                \
      (GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * infoLog))    \
  YK___SG_XMACRO(glDepthFunc, void, (GLenum func))                             \
  YK___SG_XMACRO(glStencilOp, void, (GLenum fail, GLenum zfail, GLenum zpass)) \
  YK___SG_XMACRO(glStencilFunc, void, (GLenum func, GLint ref, GLuint mask))   \
  YK___SG_XMACRO(glEnableVertexAttribArray, void, (GLuint index))              \
  YK___SG_XMACRO(glBlendFunc, void, (GLenum sfactor, GLenum dfactor))          \
  YK___SG_XMACRO(glUniform1fv, void,                                           \
                 (GLint location, GLsizei count, const GLfloat *value))        \
  YK___SG_XMACRO(glReadBuffer, void, (GLenum src))                             \
  YK___SG_XMACRO(glReadPixels, void,                                           \
                 (GLint x, GLint y, GLsizei width, GLsizei height,             \
                  GLenum format, GLenum type, void *data))                     \
  YK___SG_XMACRO(glClear, void, (GLbitfield mask))                             \
  YK___SG_XMACRO(glTexImage2D, void,                                           \
                 (GLenum target, GLint level, GLint internalformat,            \
                  GLsizei width, GLsizei height, GLint border, GLenum format,  \
                  GLenum type, const void *pixels))                            \
  YK___SG_XMACRO(glGenVertexArrays, void, (GLsizei n, GLuint * arrays))        \
  YK___SG_XMACRO(glFrontFace, void, (GLenum mode))                             \
  YK___SG_XMACRO(glCullFace, void, (GLenum mode))
// generate GL function pointer typedefs
#define YK___SG_XMACRO(name, ret, args)                                        \
  typedef ret(GL_APIENTRY *PFN_##name) args;
YK___SG_GL_FUNCS
#undef YK___SG_XMACRO
// generate GL function pointers
#define YK___SG_XMACRO(name, ret, args) static PFN_##name name;
YK___SG_GL_FUNCS
#undef YK___SG_XMACRO
// helper function to lookup GL functions in GL DLL
typedef PROC(WINAPI *yk___sg_wglGetProcAddress)(LPCSTR);
YK___SOKOL_PRIVATE void *
yk___sg_gl_getprocaddr(const char *name,
                       yk___sg_wglGetProcAddress wgl_getprocaddress) {
  void *proc_addr = (void *) wgl_getprocaddress(name);
  if (0 == proc_addr) {
    proc_addr = (void *) GetProcAddress(yk___sg.gl.opengl32_dll, name);
  }
  YK__SOKOL_ASSERT(proc_addr);
  return proc_addr;
}
// populate GL function pointers
YK___SOKOL_PRIVATE void yk___sg_gl_load_opengl(void) {
  YK__SOKOL_ASSERT(0 == yk___sg.gl.opengl32_dll);
  yk___sg.gl.opengl32_dll = LoadLibraryA("opengl32.dll");
  YK__SOKOL_ASSERT(yk___sg.gl.opengl32_dll);
  yk___sg_wglGetProcAddress wgl_getprocaddress =
      (yk___sg_wglGetProcAddress) GetProcAddress(yk___sg.gl.opengl32_dll,
                                                 "wglGetProcAddress");
  YK__SOKOL_ASSERT(wgl_getprocaddress);
#define YK___SG_XMACRO(name, ret, args)                                        \
  name = (PFN_##name) yk___sg_gl_getprocaddr(#name, wgl_getprocaddress);
  YK___SG_GL_FUNCS
#undef YK___SG_XMACRO
}
YK___SOKOL_PRIVATE void yk___sg_gl_unload_opengl(void) {
  YK__SOKOL_ASSERT(yk___sg.gl.opengl32_dll);
  FreeLibrary(yk___sg.gl.opengl32_dll);
  yk___sg.gl.opengl32_dll = 0;
}
#endif// YK___SOKOL_USE_WIN32_GL_LOADER
/*-- type translation --------------------------------------------------------*/
YK___SOKOL_PRIVATE GLenum yk___sg_gl_buffer_target(yk__sg_buffer_type t) {
  switch (t) {
    case YK__SG_BUFFERTYPE_VERTEXBUFFER:
      return GL_ARRAY_BUFFER;
    case YK__SG_BUFFERTYPE_INDEXBUFFER:
      return GL_ELEMENT_ARRAY_BUFFER;
    default:
      YK__SOKOL_UNREACHABLE;
      return 0;
  }
}
YK___SOKOL_PRIVATE GLenum yk___sg_gl_texture_target(yk__sg_image_type t) {
  switch (t) {
    case YK__SG_IMAGETYPE_2D:
      return GL_TEXTURE_2D;
    case YK__SG_IMAGETYPE_CUBE:
      return GL_TEXTURE_CUBE_MAP;
#if !defined(YK__SOKOL_GLES2)
    case YK__SG_IMAGETYPE_3D:
      return GL_TEXTURE_3D;
    case YK__SG_IMAGETYPE_ARRAY:
      return GL_TEXTURE_2D_ARRAY;
#endif
    default:
      YK__SOKOL_UNREACHABLE;
      return 0;
  }
}
YK___SOKOL_PRIVATE GLenum yk___sg_gl_usage(yk__sg_usage u) {
  switch (u) {
    case YK__SG_USAGE_IMMUTABLE:
      return GL_STATIC_DRAW;
    case YK__SG_USAGE_DYNAMIC:
      return GL_DYNAMIC_DRAW;
    case YK__SG_USAGE_STREAM:
      return GL_STREAM_DRAW;
    default:
      YK__SOKOL_UNREACHABLE;
      return 0;
  }
}
YK___SOKOL_PRIVATE GLenum yk___sg_gl_shader_stage(yk__sg_shader_stage stage) {
  switch (stage) {
    case YK__SG_SHADERSTAGE_VS:
      return GL_VERTEX_SHADER;
    case YK__SG_SHADERSTAGE_FS:
      return GL_FRAGMENT_SHADER;
    default:
      YK__SOKOL_UNREACHABLE;
      return 0;
  }
}
YK___SOKOL_PRIVATE GLint
yk___sg_gl_vertexformat_size(yk__sg_vertex_format fmt) {
  switch (fmt) {
    case YK__SG_VERTEXFORMAT_FLOAT:
      return 1;
    case YK__SG_VERTEXFORMAT_FLOAT2:
      return 2;
    case YK__SG_VERTEXFORMAT_FLOAT3:
      return 3;
    case YK__SG_VERTEXFORMAT_FLOAT4:
      return 4;
    case YK__SG_VERTEXFORMAT_BYTE4:
      return 4;
    case YK__SG_VERTEXFORMAT_BYTE4N:
      return 4;
    case YK__SG_VERTEXFORMAT_UBYTE4:
      return 4;
    case YK__SG_VERTEXFORMAT_UBYTE4N:
      return 4;
    case YK__SG_VERTEXFORMAT_SHORT2:
      return 2;
    case YK__SG_VERTEXFORMAT_SHORT2N:
      return 2;
    case YK__SG_VERTEXFORMAT_USHORT2N:
      return 2;
    case YK__SG_VERTEXFORMAT_SHORT4:
      return 4;
    case YK__SG_VERTEXFORMAT_SHORT4N:
      return 4;
    case YK__SG_VERTEXFORMAT_USHORT4N:
      return 4;
    case YK__SG_VERTEXFORMAT_UINT10_N2:
      return 4;
    default:
      YK__SOKOL_UNREACHABLE;
      return 0;
  }
}
YK___SOKOL_PRIVATE GLenum
yk___sg_gl_vertexformat_type(yk__sg_vertex_format fmt) {
  switch (fmt) {
    case YK__SG_VERTEXFORMAT_FLOAT:
    case YK__SG_VERTEXFORMAT_FLOAT2:
    case YK__SG_VERTEXFORMAT_FLOAT3:
    case YK__SG_VERTEXFORMAT_FLOAT4:
      return GL_FLOAT;
    case YK__SG_VERTEXFORMAT_BYTE4:
    case YK__SG_VERTEXFORMAT_BYTE4N:
      return GL_BYTE;
    case YK__SG_VERTEXFORMAT_UBYTE4:
    case YK__SG_VERTEXFORMAT_UBYTE4N:
      return GL_UNSIGNED_BYTE;
    case YK__SG_VERTEXFORMAT_SHORT2:
    case YK__SG_VERTEXFORMAT_SHORT2N:
    case YK__SG_VERTEXFORMAT_SHORT4:
    case YK__SG_VERTEXFORMAT_SHORT4N:
      return GL_SHORT;
    case YK__SG_VERTEXFORMAT_USHORT2N:
    case YK__SG_VERTEXFORMAT_USHORT4N:
      return GL_UNSIGNED_SHORT;
    case YK__SG_VERTEXFORMAT_UINT10_N2:
      return GL_UNSIGNED_INT_2_10_10_10_REV;
    default:
      YK__SOKOL_UNREACHABLE;
      return 0;
  }
}
YK___SOKOL_PRIVATE GLboolean
yk___sg_gl_vertexformat_normalized(yk__sg_vertex_format fmt) {
  switch (fmt) {
    case YK__SG_VERTEXFORMAT_BYTE4N:
    case YK__SG_VERTEXFORMAT_UBYTE4N:
    case YK__SG_VERTEXFORMAT_SHORT2N:
    case YK__SG_VERTEXFORMAT_USHORT2N:
    case YK__SG_VERTEXFORMAT_SHORT4N:
    case YK__SG_VERTEXFORMAT_USHORT4N:
    case YK__SG_VERTEXFORMAT_UINT10_N2:
      return GL_TRUE;
    default:
      return GL_FALSE;
  }
}
YK___SOKOL_PRIVATE GLenum yk___sg_gl_primitive_type(yk__sg_primitive_type t) {
  switch (t) {
    case YK__SG_PRIMITIVETYPE_POINTS:
      return GL_POINTS;
    case YK__SG_PRIMITIVETYPE_LINES:
      return GL_LINES;
    case YK__SG_PRIMITIVETYPE_LINE_STRIP:
      return GL_LINE_STRIP;
    case YK__SG_PRIMITIVETYPE_TRIANGLES:
      return GL_TRIANGLES;
    case YK__SG_PRIMITIVETYPE_TRIANGLE_STRIP:
      return GL_TRIANGLE_STRIP;
    default:
      YK__SOKOL_UNREACHABLE;
      return 0;
  }
}
YK___SOKOL_PRIVATE GLenum yk___sg_gl_index_type(yk__sg_index_type t) {
  switch (t) {
    case YK__SG_INDEXTYPE_NONE:
      return 0;
    case YK__SG_INDEXTYPE_UINT16:
      return GL_UNSIGNED_SHORT;
    case YK__SG_INDEXTYPE_UINT32:
      return GL_UNSIGNED_INT;
    default:
      YK__SOKOL_UNREACHABLE;
      return 0;
  }
}
YK___SOKOL_PRIVATE GLenum yk___sg_gl_compare_func(yk__sg_compare_func cmp) {
  switch (cmp) {
    case YK__SG_COMPAREFUNC_NEVER:
      return GL_NEVER;
    case YK__SG_COMPAREFUNC_LESS:
      return GL_LESS;
    case YK__SG_COMPAREFUNC_EQUAL:
      return GL_EQUAL;
    case YK__SG_COMPAREFUNC_LESS_EQUAL:
      return GL_LEQUAL;
    case YK__SG_COMPAREFUNC_GREATER:
      return GL_GREATER;
    case YK__SG_COMPAREFUNC_NOT_EQUAL:
      return GL_NOTEQUAL;
    case YK__SG_COMPAREFUNC_GREATER_EQUAL:
      return GL_GEQUAL;
    case YK__SG_COMPAREFUNC_ALWAYS:
      return GL_ALWAYS;
    default:
      YK__SOKOL_UNREACHABLE;
      return 0;
  }
}
YK___SOKOL_PRIVATE GLenum yk___sg_gl_stencil_op(yk__sg_stencil_op op) {
  switch (op) {
    case YK__SG_STENCILOP_KEEP:
      return GL_KEEP;
    case YK__SG_STENCILOP_ZERO:
      return GL_ZERO;
    case YK__SG_STENCILOP_REPLACE:
      return GL_REPLACE;
    case YK__SG_STENCILOP_INCR_CLAMP:
      return GL_INCR;
    case YK__SG_STENCILOP_DECR_CLAMP:
      return GL_DECR;
    case YK__SG_STENCILOP_INVERT:
      return GL_INVERT;
    case YK__SG_STENCILOP_INCR_WRAP:
      return GL_INCR_WRAP;
    case YK__SG_STENCILOP_DECR_WRAP:
      return GL_DECR_WRAP;
    default:
      YK__SOKOL_UNREACHABLE;
      return 0;
  }
}
YK___SOKOL_PRIVATE GLenum yk___sg_gl_blend_factor(yk__sg_blend_factor f) {
  switch (f) {
    case YK__SG_BLENDFACTOR_ZERO:
      return GL_ZERO;
    case YK__SG_BLENDFACTOR_ONE:
      return GL_ONE;
    case YK__SG_BLENDFACTOR_SRC_COLOR:
      return GL_SRC_COLOR;
    case YK__SG_BLENDFACTOR_ONE_MINUS_SRC_COLOR:
      return GL_ONE_MINUS_SRC_COLOR;
    case YK__SG_BLENDFACTOR_SRC_ALPHA:
      return GL_SRC_ALPHA;
    case YK__SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA:
      return GL_ONE_MINUS_SRC_ALPHA;
    case YK__SG_BLENDFACTOR_DST_COLOR:
      return GL_DST_COLOR;
    case YK__SG_BLENDFACTOR_ONE_MINUS_DST_COLOR:
      return GL_ONE_MINUS_DST_COLOR;
    case YK__SG_BLENDFACTOR_DST_ALPHA:
      return GL_DST_ALPHA;
    case YK__SG_BLENDFACTOR_ONE_MINUS_DST_ALPHA:
      return GL_ONE_MINUS_DST_ALPHA;
    case YK__SG_BLENDFACTOR_SRC_ALPHA_SATURATED:
      return GL_SRC_ALPHA_SATURATE;
    case YK__SG_BLENDFACTOR_BLEND_COLOR:
      return GL_CONSTANT_COLOR;
    case YK__SG_BLENDFACTOR_ONE_MINUS_BLEND_COLOR:
      return GL_ONE_MINUS_CONSTANT_COLOR;
    case YK__SG_BLENDFACTOR_BLEND_ALPHA:
      return GL_CONSTANT_ALPHA;
    case YK__SG_BLENDFACTOR_ONE_MINUS_BLEND_ALPHA:
      return GL_ONE_MINUS_CONSTANT_ALPHA;
    default:
      YK__SOKOL_UNREACHABLE;
      return 0;
  }
}
YK___SOKOL_PRIVATE GLenum yk___sg_gl_blend_op(yk__sg_blend_op op) {
  switch (op) {
    case YK__SG_BLENDOP_ADD:
      return GL_FUNC_ADD;
    case YK__SG_BLENDOP_SUBTRACT:
      return GL_FUNC_SUBTRACT;
    case YK__SG_BLENDOP_REVERSE_SUBTRACT:
      return GL_FUNC_REVERSE_SUBTRACT;
    default:
      YK__SOKOL_UNREACHABLE;
      return 0;
  }
}
YK___SOKOL_PRIVATE GLenum yk___sg_gl_filter(yk__sg_filter f) {
  switch (f) {
    case YK__SG_FILTER_NEAREST:
      return GL_NEAREST;
    case YK__SG_FILTER_LINEAR:
      return GL_LINEAR;
    case YK__SG_FILTER_NEAREST_MIPMAP_NEAREST:
      return GL_NEAREST_MIPMAP_NEAREST;
    case YK__SG_FILTER_NEAREST_MIPMAP_LINEAR:
      return GL_NEAREST_MIPMAP_LINEAR;
    case YK__SG_FILTER_LINEAR_MIPMAP_NEAREST:
      return GL_LINEAR_MIPMAP_NEAREST;
    case YK__SG_FILTER_LINEAR_MIPMAP_LINEAR:
      return GL_LINEAR_MIPMAP_LINEAR;
    default:
      YK__SOKOL_UNREACHABLE;
      return 0;
  }
}
YK___SOKOL_PRIVATE GLenum yk___sg_gl_wrap(yk__sg_wrap w) {
  switch (w) {
    case YK__SG_WRAP_CLAMP_TO_EDGE:
      return GL_CLAMP_TO_EDGE;
#if defined(YK__SOKOL_GLCORE33)
    case YK__SG_WRAP_CLAMP_TO_BORDER:
      return GL_CLAMP_TO_BORDER;
#else
    case YK__SG_WRAP_CLAMP_TO_BORDER:
      return GL_CLAMP_TO_EDGE;
#endif
    case YK__SG_WRAP_REPEAT:
      return GL_REPEAT;
    case YK__SG_WRAP_MIRRORED_REPEAT:
      return GL_MIRRORED_REPEAT;
    default:
      YK__SOKOL_UNREACHABLE;
      return 0;
  }
}
YK___SOKOL_PRIVATE GLenum yk___sg_gl_teximage_type(yk__sg_pixel_format fmt) {
  switch (fmt) {
    case YK__SG_PIXELFORMAT_R8:
    case YK__SG_PIXELFORMAT_R8UI:
    case YK__SG_PIXELFORMAT_RG8:
    case YK__SG_PIXELFORMAT_RG8UI:
    case YK__SG_PIXELFORMAT_RGBA8:
    case YK__SG_PIXELFORMAT_RGBA8UI:
    case YK__SG_PIXELFORMAT_BGRA8:
      return GL_UNSIGNED_BYTE;
    case YK__SG_PIXELFORMAT_R8SN:
    case YK__SG_PIXELFORMAT_R8SI:
    case YK__SG_PIXELFORMAT_RG8SN:
    case YK__SG_PIXELFORMAT_RG8SI:
    case YK__SG_PIXELFORMAT_RGBA8SN:
    case YK__SG_PIXELFORMAT_RGBA8SI:
      return GL_BYTE;
    case YK__SG_PIXELFORMAT_R16:
    case YK__SG_PIXELFORMAT_R16UI:
    case YK__SG_PIXELFORMAT_RG16:
    case YK__SG_PIXELFORMAT_RG16UI:
    case YK__SG_PIXELFORMAT_RGBA16:
    case YK__SG_PIXELFORMAT_RGBA16UI:
      return GL_UNSIGNED_SHORT;
    case YK__SG_PIXELFORMAT_R16SN:
    case YK__SG_PIXELFORMAT_R16SI:
    case YK__SG_PIXELFORMAT_RG16SN:
    case YK__SG_PIXELFORMAT_RG16SI:
    case YK__SG_PIXELFORMAT_RGBA16SN:
    case YK__SG_PIXELFORMAT_RGBA16SI:
      return GL_SHORT;
    case YK__SG_PIXELFORMAT_R16F:
    case YK__SG_PIXELFORMAT_RG16F:
    case YK__SG_PIXELFORMAT_RGBA16F:
      return GL_HALF_FLOAT;
    case YK__SG_PIXELFORMAT_R32UI:
    case YK__SG_PIXELFORMAT_RG32UI:
    case YK__SG_PIXELFORMAT_RGBA32UI:
      return GL_UNSIGNED_INT;
    case YK__SG_PIXELFORMAT_R32SI:
    case YK__SG_PIXELFORMAT_RG32SI:
    case YK__SG_PIXELFORMAT_RGBA32SI:
      return GL_INT;
    case YK__SG_PIXELFORMAT_R32F:
    case YK__SG_PIXELFORMAT_RG32F:
    case YK__SG_PIXELFORMAT_RGBA32F:
      return GL_FLOAT;
#if !defined(YK__SOKOL_GLES2)
    case YK__SG_PIXELFORMAT_RGB10A2:
      return GL_UNSIGNED_INT_2_10_10_10_REV;
    case YK__SG_PIXELFORMAT_RG11B10F:
      return GL_UNSIGNED_INT_10F_11F_11F_REV;
#endif
    case YK__SG_PIXELFORMAT_DEPTH:
      return GL_UNSIGNED_SHORT;
    case YK__SG_PIXELFORMAT_DEPTH_STENCIL:
      return GL_UNSIGNED_INT_24_8;
    default:
      YK__SOKOL_UNREACHABLE;
      return 0;
  }
}
YK___SOKOL_PRIVATE GLenum yk___sg_gl_teximage_format(yk__sg_pixel_format fmt) {
  switch (fmt) {
    case YK__SG_PIXELFORMAT_R8:
    case YK__SG_PIXELFORMAT_R8SN:
    case YK__SG_PIXELFORMAT_R16:
    case YK__SG_PIXELFORMAT_R16SN:
    case YK__SG_PIXELFORMAT_R16F:
    case YK__SG_PIXELFORMAT_R32F:
#if defined(YK__SOKOL_GLES2)
      return GL_LUMINANCE;
#else
      if (yk___sg.gl.gles2) {
        return GL_LUMINANCE;
      } else {
        return GL_RED;
      }
#endif
#if !defined(YK__SOKOL_GLES2)
    case YK__SG_PIXELFORMAT_R8UI:
    case YK__SG_PIXELFORMAT_R8SI:
    case YK__SG_PIXELFORMAT_R16UI:
    case YK__SG_PIXELFORMAT_R16SI:
    case YK__SG_PIXELFORMAT_R32UI:
    case YK__SG_PIXELFORMAT_R32SI:
      return GL_RED_INTEGER;
    case YK__SG_PIXELFORMAT_RG8:
    case YK__SG_PIXELFORMAT_RG8SN:
    case YK__SG_PIXELFORMAT_RG16:
    case YK__SG_PIXELFORMAT_RG16SN:
    case YK__SG_PIXELFORMAT_RG16F:
    case YK__SG_PIXELFORMAT_RG32F:
      return GL_RG;
    case YK__SG_PIXELFORMAT_RG8UI:
    case YK__SG_PIXELFORMAT_RG8SI:
    case YK__SG_PIXELFORMAT_RG16UI:
    case YK__SG_PIXELFORMAT_RG16SI:
    case YK__SG_PIXELFORMAT_RG32UI:
    case YK__SG_PIXELFORMAT_RG32SI:
      return GL_RG_INTEGER;
#endif
    case YK__SG_PIXELFORMAT_RGBA8:
    case YK__SG_PIXELFORMAT_RGBA8SN:
    case YK__SG_PIXELFORMAT_RGBA16:
    case YK__SG_PIXELFORMAT_RGBA16SN:
    case YK__SG_PIXELFORMAT_RGBA16F:
    case YK__SG_PIXELFORMAT_RGBA32F:
    case YK__SG_PIXELFORMAT_RGB10A2:
      return GL_RGBA;
#if !defined(YK__SOKOL_GLES2)
    case YK__SG_PIXELFORMAT_RGBA8UI:
    case YK__SG_PIXELFORMAT_RGBA8SI:
    case YK__SG_PIXELFORMAT_RGBA16UI:
    case YK__SG_PIXELFORMAT_RGBA16SI:
    case YK__SG_PIXELFORMAT_RGBA32UI:
    case YK__SG_PIXELFORMAT_RGBA32SI:
      return GL_RGBA_INTEGER;
#endif
    case YK__SG_PIXELFORMAT_RG11B10F:
      return GL_RGB;
    case YK__SG_PIXELFORMAT_DEPTH:
      return GL_DEPTH_COMPONENT;
    case YK__SG_PIXELFORMAT_DEPTH_STENCIL:
      return GL_DEPTH_STENCIL;
    case YK__SG_PIXELFORMAT_BC1_RGBA:
      return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    case YK__SG_PIXELFORMAT_BC2_RGBA:
      return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    case YK__SG_PIXELFORMAT_BC3_RGBA:
      return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    case YK__SG_PIXELFORMAT_BC4_R:
      return GL_COMPRESSED_RED_RGTC1;
    case YK__SG_PIXELFORMAT_BC4_RSN:
      return GL_COMPRESSED_SIGNED_RED_RGTC1;
    case YK__SG_PIXELFORMAT_BC5_RG:
      return GL_COMPRESSED_RED_GREEN_RGTC2;
    case YK__SG_PIXELFORMAT_BC5_RGSN:
      return GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2;
    case YK__SG_PIXELFORMAT_BC6H_RGBF:
      return GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB;
    case YK__SG_PIXELFORMAT_BC6H_RGBUF:
      return GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB;
    case YK__SG_PIXELFORMAT_BC7_RGBA:
      return GL_COMPRESSED_RGBA_BPTC_UNORM_ARB;
    case YK__SG_PIXELFORMAT_PVRTC_RGB_2BPP:
      return GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
    case YK__SG_PIXELFORMAT_PVRTC_RGB_4BPP:
      return GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
    case YK__SG_PIXELFORMAT_PVRTC_RGBA_2BPP:
      return GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
    case YK__SG_PIXELFORMAT_PVRTC_RGBA_4BPP:
      return GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
    case YK__SG_PIXELFORMAT_ETC2_RGB8:
      return GL_COMPRESSED_RGB8_ETC2;
    case YK__SG_PIXELFORMAT_ETC2_RGB8A1:
      return GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
    case YK__SG_PIXELFORMAT_ETC2_RGBA8:
      return GL_COMPRESSED_RGBA8_ETC2_EAC;
    case YK__SG_PIXELFORMAT_ETC2_RG11:
      return GL_COMPRESSED_RG11_EAC;
    case YK__SG_PIXELFORMAT_ETC2_RG11SN:
      return GL_COMPRESSED_SIGNED_RG11_EAC;
    default:
      YK__SOKOL_UNREACHABLE;
      return 0;
  }
}
YK___SOKOL_PRIVATE GLenum
yk___sg_gl_teximage_internal_format(yk__sg_pixel_format fmt) {
#if defined(YK__SOKOL_GLES2)
  return yk___sg_gl_teximage_format(fmt);
#else
  if (yk___sg.gl.gles2) {
    return yk___sg_gl_teximage_format(fmt);
  } else {
    switch (fmt) {
      case YK__SG_PIXELFORMAT_R8:
        return GL_R8;
      case YK__SG_PIXELFORMAT_R8SN:
        return GL_R8_SNORM;
      case YK__SG_PIXELFORMAT_R8UI:
        return GL_R8UI;
      case YK__SG_PIXELFORMAT_R8SI:
        return GL_R8I;
#if !defined(YK__SOKOL_GLES3)
      case YK__SG_PIXELFORMAT_R16:
        return GL_R16;
      case YK__SG_PIXELFORMAT_R16SN:
        return GL_R16_SNORM;
#endif
      case YK__SG_PIXELFORMAT_R16UI:
        return GL_R16UI;
      case YK__SG_PIXELFORMAT_R16SI:
        return GL_R16I;
      case YK__SG_PIXELFORMAT_R16F:
        return GL_R16F;
      case YK__SG_PIXELFORMAT_RG8:
        return GL_RG8;
      case YK__SG_PIXELFORMAT_RG8SN:
        return GL_RG8_SNORM;
      case YK__SG_PIXELFORMAT_RG8UI:
        return GL_RG8UI;
      case YK__SG_PIXELFORMAT_RG8SI:
        return GL_RG8I;
      case YK__SG_PIXELFORMAT_R32UI:
        return GL_R32UI;
      case YK__SG_PIXELFORMAT_R32SI:
        return GL_R32I;
      case YK__SG_PIXELFORMAT_R32F:
        return GL_R32F;
#if !defined(YK__SOKOL_GLES3)
      case YK__SG_PIXELFORMAT_RG16:
        return GL_RG16;
      case YK__SG_PIXELFORMAT_RG16SN:
        return GL_RG16_SNORM;
#endif
      case YK__SG_PIXELFORMAT_RG16UI:
        return GL_RG16UI;
      case YK__SG_PIXELFORMAT_RG16SI:
        return GL_RG16I;
      case YK__SG_PIXELFORMAT_RG16F:
        return GL_RG16F;
      case YK__SG_PIXELFORMAT_RGBA8:
        return GL_RGBA8;
      case YK__SG_PIXELFORMAT_RGBA8SN:
        return GL_RGBA8_SNORM;
      case YK__SG_PIXELFORMAT_RGBA8UI:
        return GL_RGBA8UI;
      case YK__SG_PIXELFORMAT_RGBA8SI:
        return GL_RGBA8I;
      case YK__SG_PIXELFORMAT_RGB10A2:
        return GL_RGB10_A2;
      case YK__SG_PIXELFORMAT_RG11B10F:
        return GL_R11F_G11F_B10F;
      case YK__SG_PIXELFORMAT_RG32UI:
        return GL_RG32UI;
      case YK__SG_PIXELFORMAT_RG32SI:
        return GL_RG32I;
      case YK__SG_PIXELFORMAT_RG32F:
        return GL_RG32F;
#if !defined(YK__SOKOL_GLES3)
      case YK__SG_PIXELFORMAT_RGBA16:
        return GL_RGBA16;
      case YK__SG_PIXELFORMAT_RGBA16SN:
        return GL_RGBA16_SNORM;
#endif
      case YK__SG_PIXELFORMAT_RGBA16UI:
        return GL_RGBA16UI;
      case YK__SG_PIXELFORMAT_RGBA16SI:
        return GL_RGBA16I;
      case YK__SG_PIXELFORMAT_RGBA16F:
        return GL_RGBA16F;
      case YK__SG_PIXELFORMAT_RGBA32UI:
        return GL_RGBA32UI;
      case YK__SG_PIXELFORMAT_RGBA32SI:
        return GL_RGBA32I;
      case YK__SG_PIXELFORMAT_RGBA32F:
        return GL_RGBA32F;
      case YK__SG_PIXELFORMAT_DEPTH:
        return GL_DEPTH_COMPONENT16;
      case YK__SG_PIXELFORMAT_DEPTH_STENCIL:
        return GL_DEPTH24_STENCIL8;
      case YK__SG_PIXELFORMAT_BC1_RGBA:
        return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
      case YK__SG_PIXELFORMAT_BC2_RGBA:
        return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
      case YK__SG_PIXELFORMAT_BC3_RGBA:
        return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
      case YK__SG_PIXELFORMAT_BC4_R:
        return GL_COMPRESSED_RED_RGTC1;
      case YK__SG_PIXELFORMAT_BC4_RSN:
        return GL_COMPRESSED_SIGNED_RED_RGTC1;
      case YK__SG_PIXELFORMAT_BC5_RG:
        return GL_COMPRESSED_RED_GREEN_RGTC2;
      case YK__SG_PIXELFORMAT_BC5_RGSN:
        return GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2;
      case YK__SG_PIXELFORMAT_BC6H_RGBF:
        return GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB;
      case YK__SG_PIXELFORMAT_BC6H_RGBUF:
        return GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB;
      case YK__SG_PIXELFORMAT_BC7_RGBA:
        return GL_COMPRESSED_RGBA_BPTC_UNORM_ARB;
      case YK__SG_PIXELFORMAT_PVRTC_RGB_2BPP:
        return GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
      case YK__SG_PIXELFORMAT_PVRTC_RGB_4BPP:
        return GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
      case YK__SG_PIXELFORMAT_PVRTC_RGBA_2BPP:
        return GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
      case YK__SG_PIXELFORMAT_PVRTC_RGBA_4BPP:
        return GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
      case YK__SG_PIXELFORMAT_ETC2_RGB8:
        return GL_COMPRESSED_RGB8_ETC2;
      case YK__SG_PIXELFORMAT_ETC2_RGB8A1:
        return GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
      case YK__SG_PIXELFORMAT_ETC2_RGBA8:
        return GL_COMPRESSED_RGBA8_ETC2_EAC;
      case YK__SG_PIXELFORMAT_ETC2_RG11:
        return GL_COMPRESSED_RG11_EAC;
      case YK__SG_PIXELFORMAT_ETC2_RG11SN:
        return GL_COMPRESSED_SIGNED_RG11_EAC;
      default:
        YK__SOKOL_UNREACHABLE;
        return 0;
    }
  }
#endif
}
YK___SOKOL_PRIVATE GLenum yk___sg_gl_cubeface_target(int face_index) {
  switch (face_index) {
    case 0:
      return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    case 1:
      return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
    case 2:
      return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
    case 3:
      return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
    case 4:
      return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
    case 5:
      return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
    default:
      YK__SOKOL_UNREACHABLE;
      return 0;
  }
}
YK___SOKOL_PRIVATE GLenum
yk___sg_gl_depth_attachment_format(yk__sg_pixel_format fmt) {
  switch (fmt) {
    case YK__SG_PIXELFORMAT_DEPTH:
      return GL_DEPTH_COMPONENT16;
    case YK__SG_PIXELFORMAT_DEPTH_STENCIL:
      return GL_DEPTH24_STENCIL8;
    default:
      YK__SOKOL_UNREACHABLE;
      return 0;
  }
}
/* see: https://www.khronos.org/registry/OpenGL-Refpages/es3.0/html/glTexImage2D.xhtml */
YK___SOKOL_PRIVATE void yk___sg_gl_init_pixelformats(bool has_bgra) {
#if !defined(YK__SOKOL_GLES2)
  if (!yk___sg.gl.gles2) {
    yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_R8]);
  } else {
    yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_R8]);
  }
#else
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_R8]);
#endif
#if !defined(YK__SOKOL_GLES2)
  if (!yk___sg.gl.gles2) {
    yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_R8SN]);
    yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_R8UI]);
    yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_R8SI]);
#if !defined(YK__SOKOL_GLES3)
    yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_R16]);
    yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_R16SN]);
#endif
    yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_R16UI]);
    yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_R16SI]);
    yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RG8]);
    yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_RG8SN]);
    yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RG8UI]);
    yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RG8SI]);
    yk___sg_pixelformat_sr(&yk___sg.formats[YK__SG_PIXELFORMAT_R32UI]);
    yk___sg_pixelformat_sr(&yk___sg.formats[YK__SG_PIXELFORMAT_R32SI]);
#if !defined(YK__SOKOL_GLES3)
    yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RG16]);
    yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RG16SN]);
#endif
    yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RG16UI]);
    yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RG16SI]);
  }
#endif
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA8]);
#if !defined(YK__SOKOL_GLES2)
  if (!yk___sg.gl.gles2) {
    yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA8SN]);
    yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA8UI]);
    yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA8SI]);
  }
#endif
  if (has_bgra) {
    yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_BGRA8]);
  }
#if !defined(YK__SOKOL_GLES2)
  if (!yk___sg.gl.gles2) {
    yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RGB10A2]);
    yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_RG11B10F]);
    yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RG32UI]);
    yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RG32SI]);
#if !defined(YK__SOKOL_GLES3)
    yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA16]);
    yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA16SN]);
#endif
    yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA16UI]);
    yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA16SI]);
    yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA32UI]);
    yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA32SI]);
  }
#endif
  // FIXME: WEBGL_depth_texture extension?
  yk___sg_pixelformat_srmd(&yk___sg.formats[YK__SG_PIXELFORMAT_DEPTH]);
  yk___sg_pixelformat_srmd(&yk___sg.formats[YK__SG_PIXELFORMAT_DEPTH_STENCIL]);
}
/* FIXME: OES_half_float_blend */
YK___SOKOL_PRIVATE void
yk___sg_gl_init_pixelformats_half_float(bool has_colorbuffer_half_float,
                                        bool has_texture_half_float_linear) {
#if !defined(YK__SOKOL_GLES2)
  if (!yk___sg.gl.gles2) {
    if (has_texture_half_float_linear) {
      if (has_colorbuffer_half_float) {
        yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_R16F]);
        yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RG16F]);
        yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA16F]);
      } else {
        yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_R16F]);
        yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_RG16F]);
        yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA16F]);
      }
    } else {
      if (has_colorbuffer_half_float) {
        yk___sg_pixelformat_sbrm(&yk___sg.formats[YK__SG_PIXELFORMAT_R16F]);
        yk___sg_pixelformat_sbrm(&yk___sg.formats[YK__SG_PIXELFORMAT_RG16F]);
        yk___sg_pixelformat_sbrm(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA16F]);
      } else {
        yk___sg_pixelformat_s(&yk___sg.formats[YK__SG_PIXELFORMAT_R16F]);
        yk___sg_pixelformat_s(&yk___sg.formats[YK__SG_PIXELFORMAT_RG16F]);
        yk___sg_pixelformat_s(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA16F]);
      }
    }
  } else {
#endif
    /* GLES2 can only render to RGBA, and there's no RG format */
    if (has_texture_half_float_linear) {
      if (has_colorbuffer_half_float) {
        yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA16F]);
      } else {
        yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA16F]);
      }
      yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_R16F]);
    } else {
      if (has_colorbuffer_half_float) {
        yk___sg_pixelformat_sbrm(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA16F]);
      } else {
        yk___sg_pixelformat_s(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA16F]);
      }
      yk___sg_pixelformat_s(&yk___sg.formats[YK__SG_PIXELFORMAT_R16F]);
    }
#if !defined(YK__SOKOL_GLES2)
  }
#endif
}
YK___SOKOL_PRIVATE void
yk___sg_gl_init_pixelformats_float(bool has_colorbuffer_float,
                                   bool has_texture_float_linear,
                                   bool has_float_blend) {
#if !defined(YK__SOKOL_GLES2)
  if (!yk___sg.gl.gles2) {
    if (has_texture_float_linear) {
      if (has_colorbuffer_float) {
        if (has_float_blend) {
          yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_R32F]);
          yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RG32F]);
          yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA32F]);
        } else {
          yk___sg_pixelformat_sfrm(&yk___sg.formats[YK__SG_PIXELFORMAT_R32F]);
          yk___sg_pixelformat_sfrm(&yk___sg.formats[YK__SG_PIXELFORMAT_RG32F]);
          yk___sg_pixelformat_sfrm(
              &yk___sg.formats[YK__SG_PIXELFORMAT_RGBA32F]);
        }
      } else {
        yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_R32F]);
        yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_RG32F]);
        yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA32F]);
      }
    } else {
      if (has_colorbuffer_float) {
        yk___sg_pixelformat_sbrm(&yk___sg.formats[YK__SG_PIXELFORMAT_R32F]);
        yk___sg_pixelformat_sbrm(&yk___sg.formats[YK__SG_PIXELFORMAT_RG32F]);
        yk___sg_pixelformat_sbrm(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA32F]);
      } else {
        yk___sg_pixelformat_s(&yk___sg.formats[YK__SG_PIXELFORMAT_R32F]);
        yk___sg_pixelformat_s(&yk___sg.formats[YK__SG_PIXELFORMAT_RG32F]);
        yk___sg_pixelformat_s(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA32F]);
      }
    }
  } else {
#endif
    /* GLES2 can only render to RGBA, and there's no RG format */
    if (has_texture_float_linear) {
      if (has_colorbuffer_float) {
        if (has_float_blend) {
          yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA32F]);
        } else {
          yk___sg_pixelformat_sfrm(
              &yk___sg.formats[YK__SG_PIXELFORMAT_RGBA32F]);
        }
      } else {
        yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA32F]);
      }
      yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_R32F]);
    } else {
      if (has_colorbuffer_float) {
        yk___sg_pixelformat_sbrm(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA32F]);
      } else {
        yk___sg_pixelformat_s(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA32F]);
      }
      yk___sg_pixelformat_s(&yk___sg.formats[YK__SG_PIXELFORMAT_R32F]);
    }
#if !defined(YK__SOKOL_GLES2)
  }
#endif
}
YK___SOKOL_PRIVATE void yk___sg_gl_init_pixelformats_s3tc(void) {
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC1_RGBA]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC2_RGBA]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC3_RGBA]);
}
YK___SOKOL_PRIVATE void yk___sg_gl_init_pixelformats_rgtc(void) {
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC4_R]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC4_RSN]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC5_RG]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC5_RGSN]);
}
YK___SOKOL_PRIVATE void yk___sg_gl_init_pixelformats_bptc(void) {
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC6H_RGBF]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC6H_RGBUF]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC7_RGBA]);
}
YK___SOKOL_PRIVATE void yk___sg_gl_init_pixelformats_pvrtc(void) {
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_PVRTC_RGB_2BPP]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_PVRTC_RGB_4BPP]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_PVRTC_RGBA_2BPP]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_PVRTC_RGBA_4BPP]);
}
YK___SOKOL_PRIVATE void yk___sg_gl_init_pixelformats_etc2(void) {
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_ETC2_RGB8]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_ETC2_RGB8A1]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_ETC2_RGBA8]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_ETC2_RG11]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_ETC2_RG11SN]);
}
YK___SOKOL_PRIVATE void yk___sg_gl_init_limits(void) {
  YK___SG_GL_CHECK_ERROR();
  GLint gl_int;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &gl_int);
  YK___SG_GL_CHECK_ERROR();
  yk___sg.limits.max_image_size_2d = gl_int;
  yk___sg.limits.max_image_size_array = gl_int;
  glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &gl_int);
  YK___SG_GL_CHECK_ERROR();
  yk___sg.limits.max_image_size_cube = gl_int;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &gl_int);
  YK___SG_GL_CHECK_ERROR();
  if (gl_int > YK__SG_MAX_VERTEX_ATTRIBUTES) {
    gl_int = YK__SG_MAX_VERTEX_ATTRIBUTES;
  }
  yk___sg.limits.max_vertex_attrs = gl_int;
  glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &gl_int);
  YK___SG_GL_CHECK_ERROR();
  yk___sg.limits.gl_max_vertex_uniform_vectors = gl_int;
#if !defined(YK__SOKOL_GLES2)
  if (!yk___sg.gl.gles2) {
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &gl_int);
    YK___SG_GL_CHECK_ERROR();
    yk___sg.limits.max_image_size_3d = gl_int;
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &gl_int);
    YK___SG_GL_CHECK_ERROR();
    yk___sg.limits.max_image_array_layers = gl_int;
  }
#endif
  if (yk___sg.gl.ext_anisotropic) {
    glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &gl_int);
    YK___SG_GL_CHECK_ERROR();
    yk___sg.gl.max_anisotropy = gl_int;
  } else {
    yk___sg.gl.max_anisotropy = 1;
  }
  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &gl_int);
  YK___SG_GL_CHECK_ERROR();
  yk___sg.gl.max_combined_texture_image_units = gl_int;
}
#if defined(YK__SOKOL_GLCORE33)
YK___SOKOL_PRIVATE void yk___sg_gl_init_caps_glcore33(void) {
  yk___sg.backend = YK__SG_BACKEND_GLCORE33;
  yk___sg.features.origin_top_left = false;
  yk___sg.features.instancing = true;
  yk___sg.features.multiple_render_targets = true;
  yk___sg.features.msaa_render_targets = true;
  yk___sg.features.imagetype_3d = true;
  yk___sg.features.imagetype_array = true;
  yk___sg.features.image_clamp_to_border = true;
  yk___sg.features.mrt_independent_blend_state = false;
  yk___sg.features.mrt_independent_write_mask = true;
  /* scan extensions */
  bool has_s3tc = false; /* BC1..BC3 */
  bool has_rgtc = false; /* BC4 and BC5 */
  bool has_bptc = false; /* BC6H and BC7 */
  bool has_pvrtc = false;
  bool has_etc2 = false;
  GLint num_ext = 0;
  glGetIntegerv(GL_NUM_EXTENSIONS, &num_ext);
  for (int i = 0; i < num_ext; i++) {
    const char *ext = (const char *) glGetStringi(GL_EXTENSIONS, (GLuint) i);
    if (ext) {
      if (strstr(ext, "_texture_compression_s3tc")) {
        has_s3tc = true;
      } else if (strstr(ext, "_texture_compression_rgtc")) {
        has_rgtc = true;
      } else if (strstr(ext, "_texture_compression_bptc")) {
        has_bptc = true;
      } else if (strstr(ext, "_texture_compression_pvrtc")) {
        has_pvrtc = true;
      } else if (strstr(ext, "_ES3_compatibility")) {
        has_etc2 = true;
      } else if (strstr(ext, "_texture_filter_anisotropic")) {
        yk___sg.gl.ext_anisotropic = true;
      }
    }
  }
  /* limits */
  yk___sg_gl_init_limits();
  /* pixel formats */
  const bool has_bgra = false; /* not a bug */
  const bool has_colorbuffer_float = true;
  const bool has_colorbuffer_half_float = true;
  const bool has_texture_float_linear = true; /* FIXME??? */
  const bool has_texture_half_float_linear = true;
  const bool has_float_blend = true;
  yk___sg_gl_init_pixelformats(has_bgra);
  yk___sg_gl_init_pixelformats_float(has_colorbuffer_float,
                                     has_texture_float_linear, has_float_blend);
  yk___sg_gl_init_pixelformats_half_float(has_colorbuffer_half_float,
                                          has_texture_half_float_linear);
  if (has_s3tc) { yk___sg_gl_init_pixelformats_s3tc(); }
  if (has_rgtc) { yk___sg_gl_init_pixelformats_rgtc(); }
  if (has_bptc) { yk___sg_gl_init_pixelformats_bptc(); }
  if (has_pvrtc) { yk___sg_gl_init_pixelformats_pvrtc(); }
  if (has_etc2) { yk___sg_gl_init_pixelformats_etc2(); }
}
#endif
#if defined(YK__SOKOL_GLES3)
YK___SOKOL_PRIVATE void yk___sg_gl_init_caps_gles3(void) {
  yk___sg.backend = YK__SG_BACKEND_GLES3;
  yk___sg.features.origin_top_left = false;
  yk___sg.features.instancing = true;
  yk___sg.features.multiple_render_targets = true;
  yk___sg.features.msaa_render_targets = true;
  yk___sg.features.imagetype_3d = true;
  yk___sg.features.imagetype_array = true;
  yk___sg.features.image_clamp_to_border = false;
  yk___sg.features.mrt_independent_blend_state = false;
  yk___sg.features.mrt_independent_write_mask = false;
  bool has_s3tc = false; /* BC1..BC3 */
  bool has_rgtc = false; /* BC4 and BC5 */
  bool has_bptc = false; /* BC6H and BC7 */
  bool has_pvrtc = false;
#if defined(__EMSCRIPTEN__)
  bool has_etc2 = false;
#else
  bool has_etc2 = true;
#endif
  bool has_colorbuffer_float = false;
  bool has_colorbuffer_half_float = false;
  bool has_texture_float_linear = false;
  bool has_float_blend = false;
  GLint num_ext = 0;
  glGetIntegerv(GL_NUM_EXTENSIONS, &num_ext);
  for (int i = 0; i < num_ext; i++) {
    const char *ext = (const char *) glGetStringi(GL_EXTENSIONS, (GLuint) i);
    if (ext) {
      if (strstr(ext, "_texture_compression_s3tc")) {
        has_s3tc = true;
      } else if (strstr(ext, "_compressed_texture_s3tc")) {
        has_s3tc = true;
      } else if (strstr(ext, "_texture_compression_rgtc")) {
        has_rgtc = true;
      } else if (strstr(ext, "_texture_compression_bptc")) {
        has_bptc = true;
      } else if (strstr(ext, "_texture_compression_pvrtc")) {
        has_pvrtc = true;
      } else if (strstr(ext, "_compressed_texture_pvrtc")) {
        has_pvrtc = true;
      } else if (strstr(ext, "_compressed_texture_etc")) {
        has_etc2 = true;
      } else if (strstr(ext, "_color_buffer_float")) {
        has_colorbuffer_float = true;
      } else if (strstr(ext, "_color_buffer_half_float")) {
        has_colorbuffer_half_float = true;
      } else if (strstr(ext, "_texture_float_linear")) {
        has_texture_float_linear = true;
      } else if (strstr(ext, "_float_blend")) {
        has_float_blend = true;
      } else if (strstr(ext, "_texture_filter_anisotropic")) {
        yk___sg.gl.ext_anisotropic = true;
      }
    }
  }
/* on WebGL2, color_buffer_float also includes 16-bit formats
       see: https://developer.mozilla.org/en-US/docs/Web/API/EXT_color_buffer_float
    */
#if defined(__EMSCRIPTEN__)
  has_colorbuffer_half_float = has_colorbuffer_float;
#endif
  /* limits */
  yk___sg_gl_init_limits();
  /* pixel formats */
  const bool has_texture_half_float_linear = true;
  const bool has_bgra = false; /* not a bug */
  yk___sg_gl_init_pixelformats(has_bgra);
  yk___sg_gl_init_pixelformats_float(has_colorbuffer_float,
                                     has_texture_float_linear, has_float_blend);
  yk___sg_gl_init_pixelformats_half_float(has_colorbuffer_half_float,
                                          has_texture_half_float_linear);
  if (has_s3tc) { yk___sg_gl_init_pixelformats_s3tc(); }
  if (has_rgtc) { yk___sg_gl_init_pixelformats_rgtc(); }
  if (has_bptc) { yk___sg_gl_init_pixelformats_bptc(); }
  if (has_pvrtc) { yk___sg_gl_init_pixelformats_pvrtc(); }
  if (has_etc2) { yk___sg_gl_init_pixelformats_etc2(); }
}
#endif
#if defined(YK__SOKOL_GLES3) || defined(YK__SOKOL_GLES2)
YK___SOKOL_PRIVATE void yk___sg_gl_init_caps_gles2(void) {
  yk___sg.backend = YK__SG_BACKEND_GLES2;
  bool has_s3tc = false; /* BC1..BC3 */
  bool has_rgtc = false; /* BC4 and BC5 */
  bool has_bptc = false; /* BC6H and BC7 */
  bool has_pvrtc = false;
  bool has_etc2 = false;
  bool has_texture_float = false;
  bool has_texture_float_linear = false;
  bool has_colorbuffer_float = false;
  bool has_float_blend = false;
  bool has_instancing = false;
  const char *ext = (const char *) glGetString(GL_EXTENSIONS);
  if (ext) {
    has_s3tc = strstr(ext, "_texture_compression_s3tc") ||
               strstr(ext, "_compressed_texture_s3tc");
    has_rgtc = strstr(ext, "_texture_compression_rgtc");
    has_bptc = strstr(ext, "_texture_compression_bptc");
    has_pvrtc = strstr(ext, "_texture_compression_pvrtc") ||
                strstr(ext, "_compressed_texture_pvrtc");
    has_etc2 = strstr(ext, "_compressed_texture_etc");
    has_texture_float = strstr(ext, "_texture_float");
    has_texture_float_linear = strstr(ext, "_texture_float_linear");
    has_colorbuffer_float = strstr(ext, "_color_buffer_float");
    has_float_blend = strstr(ext, "_float_blend");
    /* don't bother with half_float support on WebGL1
            has_texture_half_float = strstr(ext, "_texture_half_float");
            has_texture_half_float_linear = strstr(ext, "_texture_half_float_linear");
            has_colorbuffer_half_float = strstr(ext, "_color_buffer_half_float");
        */
    has_instancing = strstr(ext, "_instanced_arrays");
    yk___sg.gl.ext_anisotropic = strstr(ext, "ext_anisotropic");
  }
  yk___sg.features.origin_top_left = false;
#if defined(YK___SOKOL_GL_INSTANCING_ENABLED)
  yk___sg.features.instancing = has_instancing;
#endif
  yk___sg.features.multiple_render_targets = false;
  yk___sg.features.msaa_render_targets = false;
  yk___sg.features.imagetype_3d = false;
  yk___sg.features.imagetype_array = false;
  yk___sg.features.image_clamp_to_border = false;
  yk___sg.features.mrt_independent_blend_state = false;
  yk___sg.features.mrt_independent_write_mask = false;
  /* limits */
  yk___sg_gl_init_limits();
  /* pixel formats */
  const bool has_bgra = false; /* not a bug */
  const bool has_texture_half_float = false;
  const bool has_texture_half_float_linear = false;
  const bool has_colorbuffer_half_float = false;
  yk___sg_gl_init_pixelformats(has_bgra);
  if (has_texture_float) {
    yk___sg_gl_init_pixelformats_float(
        has_colorbuffer_float, has_texture_float_linear, has_float_blend);
  }
  if (has_texture_half_float) {
    yk___sg_gl_init_pixelformats_half_float(has_colorbuffer_half_float,
                                            has_texture_half_float_linear);
  }
  if (has_s3tc) { yk___sg_gl_init_pixelformats_s3tc(); }
  if (has_rgtc) { yk___sg_gl_init_pixelformats_rgtc(); }
  if (has_bptc) { yk___sg_gl_init_pixelformats_bptc(); }
  if (has_pvrtc) { yk___sg_gl_init_pixelformats_pvrtc(); }
  if (has_etc2) { yk___sg_gl_init_pixelformats_etc2(); }
  /* GLES2 doesn't allow multi-sampled render targets at all */
  for (int i = 0; i < YK___SG_PIXELFORMAT_NUM; i++) {
    yk___sg.formats[i].msaa = false;
  }
}
#endif
/*-- state cache implementation ----------------------------------------------*/
YK___SOKOL_PRIVATE void yk___sg_gl_cache_clear_buffer_bindings(bool force) {
  if (force || (yk___sg.gl.cache.vertex_buffer != 0)) {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    yk___sg.gl.cache.vertex_buffer = 0;
  }
  if (force || (yk___sg.gl.cache.index_buffer != 0)) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    yk___sg.gl.cache.index_buffer = 0;
  }
}
YK___SOKOL_PRIVATE void yk___sg_gl_cache_bind_buffer(GLenum target,
                                                     GLuint buffer) {
  YK__SOKOL_ASSERT((GL_ARRAY_BUFFER == target) ||
                   (GL_ELEMENT_ARRAY_BUFFER == target));
  if (target == GL_ARRAY_BUFFER) {
    if (yk___sg.gl.cache.vertex_buffer != buffer) {
      yk___sg.gl.cache.vertex_buffer = buffer;
      glBindBuffer(target, buffer);
    }
  } else {
    if (yk___sg.gl.cache.index_buffer != buffer) {
      yk___sg.gl.cache.index_buffer = buffer;
      glBindBuffer(target, buffer);
    }
  }
}
YK___SOKOL_PRIVATE void yk___sg_gl_cache_store_buffer_binding(GLenum target) {
  if (target == GL_ARRAY_BUFFER) {
    yk___sg.gl.cache.stored_vertex_buffer = yk___sg.gl.cache.vertex_buffer;
  } else {
    yk___sg.gl.cache.stored_index_buffer = yk___sg.gl.cache.index_buffer;
  }
}
YK___SOKOL_PRIVATE void yk___sg_gl_cache_restore_buffer_binding(GLenum target) {
  if (target == GL_ARRAY_BUFFER) {
    if (yk___sg.gl.cache.stored_vertex_buffer != 0) {
      /* we only care restoring valid ids */
      yk___sg_gl_cache_bind_buffer(target,
                                   yk___sg.gl.cache.stored_vertex_buffer);
      yk___sg.gl.cache.stored_vertex_buffer = 0;
    }
  } else {
    if (yk___sg.gl.cache.stored_index_buffer != 0) {
      /* we only care restoring valid ids */
      yk___sg_gl_cache_bind_buffer(target,
                                   yk___sg.gl.cache.stored_index_buffer);
      yk___sg.gl.cache.stored_index_buffer = 0;
    }
  }
}
/* called when from yk___sg_gl_destroy_buffer() */
YK___SOKOL_PRIVATE void yk___sg_gl_cache_invalidate_buffer(GLuint buf) {
  if (buf == yk___sg.gl.cache.vertex_buffer) {
    yk___sg.gl.cache.vertex_buffer = 0;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
  if (buf == yk___sg.gl.cache.index_buffer) {
    yk___sg.gl.cache.index_buffer = 0;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }
  if (buf == yk___sg.gl.cache.stored_vertex_buffer) {
    yk___sg.gl.cache.stored_vertex_buffer = 0;
  }
  if (buf == yk___sg.gl.cache.stored_index_buffer) {
    yk___sg.gl.cache.stored_index_buffer = 0;
  }
  for (int i = 0; i < YK__SG_MAX_VERTEX_ATTRIBUTES; i++) {
    if (buf == yk___sg.gl.cache.attrs[i].gl_vbuf) {
      yk___sg.gl.cache.attrs[i].gl_vbuf = 0;
    }
  }
}
YK___SOKOL_PRIVATE void yk___sg_gl_cache_active_texture(GLenum texture) {
  if (yk___sg.gl.cache.cur_active_texture != texture) {
    yk___sg.gl.cache.cur_active_texture = texture;
    glActiveTexture(texture);
  }
}
YK___SOKOL_PRIVATE void yk___sg_gl_cache_clear_texture_bindings(bool force) {
  for (int i = 0; (i < YK__SG_MAX_SHADERSTAGE_IMAGES) &&
                  (i < yk___sg.gl.max_combined_texture_image_units);
       i++) {
    if (force || (yk___sg.gl.cache.textures[i].texture != 0)) {
      GLenum gl_texture_slot = (GLenum) (GL_TEXTURE0 + i);
      glActiveTexture(gl_texture_slot);
      glBindTexture(GL_TEXTURE_2D, 0);
      glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
#if !defined(YK__SOKOL_GLES2)
      if (!yk___sg.gl.gles2) {
        glBindTexture(GL_TEXTURE_3D, 0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
      }
#endif
      yk___sg.gl.cache.textures[i].target = 0;
      yk___sg.gl.cache.textures[i].texture = 0;
      yk___sg.gl.cache.cur_active_texture = gl_texture_slot;
    }
  }
}
YK___SOKOL_PRIVATE void
yk___sg_gl_cache_bind_texture(int slot_index, GLenum target, GLuint texture) {
  /* it's valid to call this function with target=0 and/or texture=0
       target=0 will unbind the previous binding, texture=0 will clear
       the new binding
    */
  YK__SOKOL_ASSERT(slot_index < YK__SG_MAX_SHADERSTAGE_IMAGES);
  if (slot_index >= yk___sg.gl.max_combined_texture_image_units) { return; }
  yk___sg_gl_texture_bind_slot *slot = &yk___sg.gl.cache.textures[slot_index];
  if ((slot->target != target) || (slot->texture != texture)) {
    yk___sg_gl_cache_active_texture((GLenum) (GL_TEXTURE0 + slot_index));
    /* if the target has changed, clear the previous binding on that target */
    if ((target != slot->target) && (slot->target != 0)) {
      glBindTexture(slot->target, 0);
    }
    /* apply new binding (texture can be 0 to unbind) */
    if (target != 0) { glBindTexture(target, texture); }
    slot->target = target;
    slot->texture = texture;
  }
}
YK___SOKOL_PRIVATE void yk___sg_gl_cache_store_texture_binding(int slot_index) {
  YK__SOKOL_ASSERT(slot_index < YK__SG_MAX_SHADERSTAGE_IMAGES);
  yk___sg.gl.cache.stored_texture = yk___sg.gl.cache.textures[slot_index];
}
YK___SOKOL_PRIVATE void
yk___sg_gl_cache_restore_texture_binding(int slot_index) {
  YK__SOKOL_ASSERT(slot_index < YK__SG_MAX_SHADERSTAGE_IMAGES);
  yk___sg_gl_texture_bind_slot *slot = &yk___sg.gl.cache.stored_texture;
  if (slot->texture != 0) {
    /* we only care restoring valid ids */
    YK__SOKOL_ASSERT(slot->target != 0);
    yk___sg_gl_cache_bind_texture(slot_index, slot->target, slot->texture);
    slot->target = 0;
    slot->texture = 0;
  }
}
/* called from _sg_gl_destroy_texture() */
YK___SOKOL_PRIVATE void yk___sg_gl_cache_invalidate_texture(GLuint tex) {
  for (int i = 0; i < YK__SG_MAX_SHADERSTAGE_IMAGES; i++) {
    yk___sg_gl_texture_bind_slot *slot = &yk___sg.gl.cache.textures[i];
    if (tex == slot->texture) {
      yk___sg_gl_cache_active_texture((GLenum) (GL_TEXTURE0 + i));
      glBindTexture(slot->target, 0);
      slot->target = 0;
      slot->texture = 0;
    }
  }
  if (tex == yk___sg.gl.cache.stored_texture.texture) {
    yk___sg.gl.cache.stored_texture.target = 0;
    yk___sg.gl.cache.stored_texture.texture = 0;
  }
}
/* called from yk___sg_gl_destroy_shader() */
YK___SOKOL_PRIVATE void yk___sg_gl_cache_invalidate_program(GLuint prog) {
  if (prog == yk___sg.gl.cache.prog) {
    yk___sg.gl.cache.prog = 0;
    glUseProgram(0);
  }
}
/* called from yk___sg_gl_destroy_pipeline() */
YK___SOKOL_PRIVATE void
yk___sg_gl_cache_invalidate_pipeline(yk___sg_pipeline_t *pip) {
  if (pip == yk___sg.gl.cache.cur_pipeline) {
    yk___sg.gl.cache.cur_pipeline = 0;
    yk___sg.gl.cache.cur_pipeline_id.id = YK__SG_INVALID_ID;
  }
}
YK___SOKOL_PRIVATE void yk___sg_gl_reset_state_cache(void) {
  if (yk___sg.gl.cur_context) {
    YK___SG_GL_CHECK_ERROR();
#if !defined(YK__SOKOL_GLES2)
    if (!yk___sg.gl.gles2) {
      glBindVertexArray(yk___sg.gl.cur_context->vao);
      YK___SG_GL_CHECK_ERROR();
    }
#endif
    memset(&yk___sg.gl.cache, 0, sizeof(yk___sg.gl.cache));
    yk___sg_gl_cache_clear_buffer_bindings(true);
    YK___SG_GL_CHECK_ERROR();
    yk___sg_gl_cache_clear_texture_bindings(true);
    YK___SG_GL_CHECK_ERROR();
    for (int i = 0; i < yk___sg.limits.max_vertex_attrs; i++) {
      yk___sg_gl_attr_t *attr = &yk___sg.gl.cache.attrs[i].gl_attr;
      attr->vb_index = -1;
      attr->divisor = -1;
      glDisableVertexAttribArray((GLuint) i);
      YK___SG_GL_CHECK_ERROR();
    }
    yk___sg.gl.cache.cur_primitive_type = GL_TRIANGLES;
    /* shader program */
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint *) &yk___sg.gl.cache.prog);
    YK___SG_GL_CHECK_ERROR();
    /* depth and stencil state */
    yk___sg.gl.cache.depth.compare = YK__SG_COMPAREFUNC_ALWAYS;
    yk___sg.gl.cache.stencil.front.compare = YK__SG_COMPAREFUNC_ALWAYS;
    yk___sg.gl.cache.stencil.front.fail_op = YK__SG_STENCILOP_KEEP;
    yk___sg.gl.cache.stencil.front.depth_fail_op = YK__SG_STENCILOP_KEEP;
    yk___sg.gl.cache.stencil.front.pass_op = YK__SG_STENCILOP_KEEP;
    yk___sg.gl.cache.stencil.back.compare = YK__SG_COMPAREFUNC_ALWAYS;
    yk___sg.gl.cache.stencil.back.fail_op = YK__SG_STENCILOP_KEEP;
    yk___sg.gl.cache.stencil.back.depth_fail_op = YK__SG_STENCILOP_KEEP;
    yk___sg.gl.cache.stencil.back.pass_op = YK__SG_STENCILOP_KEEP;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);
    glDepthMask(GL_FALSE);
    glDisable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilMask(0);
    /* blend state */
    yk___sg.gl.cache.blend.src_factor_rgb = YK__SG_BLENDFACTOR_ONE;
    yk___sg.gl.cache.blend.dst_factor_rgb = YK__SG_BLENDFACTOR_ZERO;
    yk___sg.gl.cache.blend.op_rgb = YK__SG_BLENDOP_ADD;
    yk___sg.gl.cache.blend.src_factor_alpha = YK__SG_BLENDFACTOR_ONE;
    yk___sg.gl.cache.blend.dst_factor_alpha = YK__SG_BLENDFACTOR_ZERO;
    yk___sg.gl.cache.blend.op_alpha = YK__SG_BLENDOP_ADD;
    glDisable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendColor(0.0f, 0.0f, 0.0f, 0.0f);
    /* standalone state */
    for (int i = 0; i < YK__SG_MAX_COLOR_ATTACHMENTS; i++) {
      yk___sg.gl.cache.color_write_mask[i] = YK__SG_COLORMASK_RGBA;
    }
    yk___sg.gl.cache.cull_mode = YK__SG_CULLMODE_NONE;
    yk___sg.gl.cache.face_winding = YK__SG_FACEWINDING_CW;
    yk___sg.gl.cache.sample_count = 1;
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glPolygonOffset(0.0f, 0.0f);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_SCISSOR_TEST);
    glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    glEnable(GL_DITHER);
    glDisable(GL_POLYGON_OFFSET_FILL);
#if defined(YK__SOKOL_GLCORE33)
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_PROGRAM_POINT_SIZE);
#endif
  }
}
YK___SOKOL_PRIVATE void yk___sg_gl_setup_backend(const yk__sg_desc *desc) {
  /* assumes that yk___sg.gl is already zero-initialized */
  yk___sg.gl.valid = true;
#if defined(YK__SOKOL_GLES2) || defined(YK__SOKOL_GLES3)
  yk___sg.gl.gles2 = desc->context.gl.force_gles2;
#else
  YK___SOKOL_UNUSED(desc);
  yk___sg.gl.gles2 = false;
#endif
#if defined(YK___SOKOL_USE_WIN32_GL_LOADER)
  yk___sg_gl_load_opengl();
#endif
/* clear initial GL error state */
#if defined(YK__SOKOL_DEBUG)
  while (glGetError() != GL_NO_ERROR)
    ;
#endif
#if defined(YK__SOKOL_GLCORE33)
  yk___sg_gl_init_caps_glcore33();
#elif defined(YK__SOKOL_GLES3)
  if (yk___sg.gl.gles2) {
    yk___sg_gl_init_caps_gles2();
  } else {
    yk___sg_gl_init_caps_gles3();
  }
#else
  yk___sg_gl_init_caps_gles2();
#endif
}
YK___SOKOL_PRIVATE void yk___sg_gl_discard_backend(void) {
  YK__SOKOL_ASSERT(yk___sg.gl.valid);
  yk___sg.gl.valid = false;
#if defined(YK___SOKOL_USE_WIN32_GL_LOADER)
  yk___sg_gl_unload_opengl();
#endif
}
YK___SOKOL_PRIVATE void yk___sg_gl_activate_context(yk___sg_context_t *ctx) {
  YK__SOKOL_ASSERT(yk___sg.gl.valid);
  /* NOTE: ctx can be 0 to unset the current context */
  yk___sg.gl.cur_context = ctx;
  yk___sg_gl_reset_state_cache();
}
/*-- GL backend resource creation and destruction ----------------------------*/
YK___SOKOL_PRIVATE yk__sg_resource_state
yk___sg_gl_create_context(yk___sg_context_t *ctx) {
  YK__SOKOL_ASSERT(ctx);
  YK__SOKOL_ASSERT(0 == ctx->default_framebuffer);
  YK___SG_GL_CHECK_ERROR();
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint *) &ctx->default_framebuffer);
  YK___SG_GL_CHECK_ERROR();
#if !defined(YK__SOKOL_GLES2)
  if (!yk___sg.gl.gles2) {
    YK__SOKOL_ASSERT(0 == ctx->vao);
    glGenVertexArrays(1, &ctx->vao);
    glBindVertexArray(ctx->vao);
    YK___SG_GL_CHECK_ERROR();
  }
#endif
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_gl_destroy_context(yk___sg_context_t *ctx) {
  YK__SOKOL_ASSERT(ctx);
#if !defined(YK__SOKOL_GLES2)
  if (!yk___sg.gl.gles2) {
    if (ctx->vao) { glDeleteVertexArrays(1, &ctx->vao); }
    YK___SG_GL_CHECK_ERROR();
  }
#else
  YK___SOKOL_UNUSED(ctx);
#endif
}
YK___SOKOL_PRIVATE yk__sg_resource_state yk___sg_gl_create_buffer(
    yk___sg_buffer_t *buf, const yk__sg_buffer_desc *desc) {
  YK__SOKOL_ASSERT(buf && desc);
  YK___SG_GL_CHECK_ERROR();
  yk___sg_buffer_common_init(&buf->cmn, desc);
  buf->gl.ext_buffers = (0 != desc->gl_buffers[0]);
  GLenum gl_target = yk___sg_gl_buffer_target(buf->cmn.type);
  GLenum gl_usage = yk___sg_gl_usage(buf->cmn.usage);
  for (int slot = 0; slot < buf->cmn.num_slots; slot++) {
    GLuint gl_buf = 0;
    if (buf->gl.ext_buffers) {
      YK__SOKOL_ASSERT(desc->gl_buffers[slot]);
      gl_buf = desc->gl_buffers[slot];
    } else {
      glGenBuffers(1, &gl_buf);
      YK__SOKOL_ASSERT(gl_buf);
      yk___sg_gl_cache_store_buffer_binding(gl_target);
      yk___sg_gl_cache_bind_buffer(gl_target, gl_buf);
      glBufferData(gl_target, buf->cmn.size, 0, gl_usage);
      if (buf->cmn.usage == YK__SG_USAGE_IMMUTABLE) {
        YK__SOKOL_ASSERT(desc->data.ptr);
        glBufferSubData(gl_target, 0, buf->cmn.size, desc->data.ptr);
      }
      yk___sg_gl_cache_restore_buffer_binding(gl_target);
    }
    buf->gl.buf[slot] = gl_buf;
  }
  YK___SG_GL_CHECK_ERROR();
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_gl_destroy_buffer(yk___sg_buffer_t *buf) {
  YK__SOKOL_ASSERT(buf);
  YK___SG_GL_CHECK_ERROR();
  for (int slot = 0; slot < buf->cmn.num_slots; slot++) {
    if (buf->gl.buf[slot]) {
      yk___sg_gl_cache_invalidate_buffer(buf->gl.buf[slot]);
      if (!buf->gl.ext_buffers) { glDeleteBuffers(1, &buf->gl.buf[slot]); }
    }
  }
  YK___SG_GL_CHECK_ERROR();
}
YK___SOKOL_PRIVATE bool
yk___sg_gl_supported_texture_format(yk__sg_pixel_format fmt) {
  const int fmt_index = (int) fmt;
  YK__SOKOL_ASSERT((fmt_index > YK__SG_PIXELFORMAT_NONE) &&
                   (fmt_index < YK___SG_PIXELFORMAT_NUM));
  return yk___sg.formats[fmt_index].sample;
}
YK___SOKOL_PRIVATE yk__sg_resource_state
yk___sg_gl_create_image(yk___sg_image_t *img, const yk__sg_image_desc *desc) {
  YK__SOKOL_ASSERT(img && desc);
  YK___SG_GL_CHECK_ERROR();
  yk___sg_image_common_init(&img->cmn, desc);
  img->gl.ext_textures = (0 != desc->gl_textures[0]);
  /* check if texture format is support */
  if (!yk___sg_gl_supported_texture_format(img->cmn.pixel_format)) {
    YK__SOKOL_LOG("texture format not supported by GL context\n");
    return YK__SG_RESOURCESTATE_FAILED;
  }
  /* check for optional texture types */
  if ((img->cmn.type == YK__SG_IMAGETYPE_3D) &&
      !yk___sg.features.imagetype_3d) {
    YK__SOKOL_LOG("3D textures not supported by GL context\n");
    return YK__SG_RESOURCESTATE_FAILED;
  }
  if ((img->cmn.type == YK__SG_IMAGETYPE_ARRAY) &&
      !yk___sg.features.imagetype_array) {
    YK__SOKOL_LOG("array textures not supported by GL context\n");
    return YK__SG_RESOURCESTATE_FAILED;
  }
#if !defined(YK__SOKOL_GLES2)
  bool msaa = false;
  if (!yk___sg.gl.gles2) {
    msaa =
        (img->cmn.sample_count > 1) && (yk___sg.features.msaa_render_targets);
  }
#endif
  if (yk___sg_is_valid_rendertarget_depth_format(img->cmn.pixel_format)) {
    /* special case depth-stencil-buffer? */
    YK__SOKOL_ASSERT((img->cmn.usage == YK__SG_USAGE_IMMUTABLE) &&
                     (img->cmn.num_slots == 1));
    YK__SOKOL_ASSERT(
        !img->gl
             .ext_textures); /* cannot provide external texture for depth images */
    glGenRenderbuffers(1, &img->gl.depth_render_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, img->gl.depth_render_buffer);
    GLenum gl_depth_format =
        yk___sg_gl_depth_attachment_format(img->cmn.pixel_format);
#if !defined(YK__SOKOL_GLES2)
    if (!yk___sg.gl.gles2 && msaa) {
      glRenderbufferStorageMultisample(GL_RENDERBUFFER, img->cmn.sample_count,
                                       gl_depth_format, img->cmn.width,
                                       img->cmn.height);
    } else
#endif
    {
      glRenderbufferStorage(GL_RENDERBUFFER, gl_depth_format, img->cmn.width,
                            img->cmn.height);
    }
  } else {
    /* regular color texture */
    img->gl.target = yk___sg_gl_texture_target(img->cmn.type);
    const GLenum gl_internal_format =
        yk___sg_gl_teximage_internal_format(img->cmn.pixel_format);
/* if this is a MSAA render target, need to create a separate render buffer */
#if !defined(YK__SOKOL_GLES2)
    if (!yk___sg.gl.gles2 && img->cmn.render_target && msaa) {
      glGenRenderbuffers(1, &img->gl.msaa_render_buffer);
      glBindRenderbuffer(GL_RENDERBUFFER, img->gl.msaa_render_buffer);
      glRenderbufferStorageMultisample(GL_RENDERBUFFER, img->cmn.sample_count,
                                       gl_internal_format, img->cmn.width,
                                       img->cmn.height);
    }
#endif
    if (img->gl.ext_textures) {
      /* inject externally GL textures */
      for (int slot = 0; slot < img->cmn.num_slots; slot++) {
        YK__SOKOL_ASSERT(desc->gl_textures[slot]);
        img->gl.tex[slot] = desc->gl_textures[slot];
      }
      if (desc->gl_texture_target) {
        img->gl.target = (GLenum) desc->gl_texture_target;
      }
    } else {
      /* create our own GL texture(s) */
      const GLenum gl_format =
          yk___sg_gl_teximage_format(img->cmn.pixel_format);
      const bool is_compressed =
          yk___sg_is_compressed_pixel_format(img->cmn.pixel_format);
      for (int slot = 0; slot < img->cmn.num_slots; slot++) {
        glGenTextures(1, &img->gl.tex[slot]);
        YK__SOKOL_ASSERT(img->gl.tex[slot]);
        yk___sg_gl_cache_store_texture_binding(0);
        yk___sg_gl_cache_bind_texture(0, img->gl.target, img->gl.tex[slot]);
        GLenum gl_min_filter = yk___sg_gl_filter(img->cmn.min_filter);
        GLenum gl_mag_filter = yk___sg_gl_filter(img->cmn.mag_filter);
        glTexParameteri(img->gl.target, GL_TEXTURE_MIN_FILTER,
                        (GLint) gl_min_filter);
        glTexParameteri(img->gl.target, GL_TEXTURE_MAG_FILTER,
                        (GLint) gl_mag_filter);
        if (yk___sg.gl.ext_anisotropic && (img->cmn.max_anisotropy > 1)) {
          GLint max_aniso = (GLint) img->cmn.max_anisotropy;
          if (max_aniso > yk___sg.gl.max_anisotropy) {
            max_aniso = yk___sg.gl.max_anisotropy;
          }
          glTexParameteri(img->gl.target, GL_TEXTURE_MAX_ANISOTROPY_EXT,
                          max_aniso);
        }
        if (img->cmn.type == YK__SG_IMAGETYPE_CUBE) {
          glTexParameteri(img->gl.target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
          glTexParameteri(img->gl.target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        } else {
          glTexParameteri(img->gl.target, GL_TEXTURE_WRAP_S,
                          (GLint) yk___sg_gl_wrap(img->cmn.wrap_u));
          glTexParameteri(img->gl.target, GL_TEXTURE_WRAP_T,
                          (GLint) yk___sg_gl_wrap(img->cmn.wrap_v));
#if !defined(YK__SOKOL_GLES2)
          if (!yk___sg.gl.gles2 && (img->cmn.type == YK__SG_IMAGETYPE_3D)) {
            glTexParameteri(img->gl.target, GL_TEXTURE_WRAP_R,
                            (GLint) yk___sg_gl_wrap(img->cmn.wrap_w));
          }
#endif
#if defined(YK__SOKOL_GLCORE33)
          float border[4];
          switch (img->cmn.border_color) {
            case YK__SG_BORDERCOLOR_TRANSPARENT_BLACK:
              border[0] = 0.0f;
              border[1] = 0.0f;
              border[2] = 0.0f;
              border[3] = 0.0f;
              break;
            case YK__SG_BORDERCOLOR_OPAQUE_WHITE:
              border[0] = 1.0f;
              border[1] = 1.0f;
              border[2] = 1.0f;
              border[3] = 1.0f;
              break;
            default:
              border[0] = 0.0f;
              border[1] = 0.0f;
              border[2] = 0.0f;
              border[3] = 1.0f;
              break;
          }
          glTexParameterfv(img->gl.target, GL_TEXTURE_BORDER_COLOR, border);
#endif
        }
#if !defined(YK__SOKOL_GLES2)
        if (!yk___sg.gl.gles2) {
          /* GL spec has strange defaults for mipmap min/max lod: -1000 to +1000 */
          const float min_lod = yk___sg_clamp(desc->min_lod, 0.0f, 1000.0f);
          const float max_lod = yk___sg_clamp(desc->max_lod, 0.0f, 1000.0f);
          glTexParameterf(img->gl.target, GL_TEXTURE_MIN_LOD, min_lod);
          glTexParameterf(img->gl.target, GL_TEXTURE_MAX_LOD, max_lod);
        }
#endif
        const int num_faces = img->cmn.type == YK__SG_IMAGETYPE_CUBE ? 6 : 1;
        int data_index = 0;
        for (int face_index = 0; face_index < num_faces; face_index++) {
          for (int mip_index = 0; mip_index < img->cmn.num_mipmaps;
               mip_index++, data_index++) {
            GLenum gl_img_target = img->gl.target;
            if (YK__SG_IMAGETYPE_CUBE == img->cmn.type) {
              gl_img_target = yk___sg_gl_cubeface_target(face_index);
            }
            const GLvoid *data_ptr =
                desc->data.subimage[face_index][mip_index].ptr;
            int mip_width = img->cmn.width >> mip_index;
            if (mip_width == 0) { mip_width = 1; }
            int mip_height = img->cmn.height >> mip_index;
            if (mip_height == 0) { mip_height = 1; }
            if ((YK__SG_IMAGETYPE_2D == img->cmn.type) ||
                (YK__SG_IMAGETYPE_CUBE == img->cmn.type)) {
              if (is_compressed) {
                const GLsizei data_size =
                    (GLsizei) desc->data.subimage[face_index][mip_index].size;
                glCompressedTexImage2D(gl_img_target, mip_index,
                                       gl_internal_format, mip_width,
                                       mip_height, 0, data_size, data_ptr);
              } else {
                const GLenum gl_type =
                    yk___sg_gl_teximage_type(img->cmn.pixel_format);
                glTexImage2D(gl_img_target, mip_index,
                             (GLint) gl_internal_format, mip_width, mip_height,
                             0, gl_format, gl_type, data_ptr);
              }
            }
#if !defined(YK__SOKOL_GLES2)
            else if (!yk___sg.gl.gles2 &&
                     ((YK__SG_IMAGETYPE_3D == img->cmn.type) ||
                      (YK__SG_IMAGETYPE_ARRAY == img->cmn.type))) {
              int mip_depth = img->cmn.num_slices;
              if (YK__SG_IMAGETYPE_3D == img->cmn.type) {
                mip_depth >>= mip_index;
              }
              if (mip_depth == 0) { mip_depth = 1; }
              if (is_compressed) {
                const GLsizei data_size =
                    (GLsizei) desc->data.subimage[face_index][mip_index].size;
                glCompressedTexImage3D(
                    gl_img_target, mip_index, gl_internal_format, mip_width,
                    mip_height, mip_depth, 0, data_size, data_ptr);
              } else {
                const GLenum gl_type =
                    yk___sg_gl_teximage_type(img->cmn.pixel_format);
                glTexImage3D(gl_img_target, mip_index,
                             (GLint) gl_internal_format, mip_width, mip_height,
                             mip_depth, 0, gl_format, gl_type, data_ptr);
              }
            }
#endif
          }
        }
        yk___sg_gl_cache_restore_texture_binding(0);
      }
    }
  }
  YK___SG_GL_CHECK_ERROR();
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_gl_destroy_image(yk___sg_image_t *img) {
  YK__SOKOL_ASSERT(img);
  YK___SG_GL_CHECK_ERROR();
  for (int slot = 0; slot < img->cmn.num_slots; slot++) {
    if (img->gl.tex[slot]) {
      yk___sg_gl_cache_invalidate_texture(img->gl.tex[slot]);
      if (!img->gl.ext_textures) { glDeleteTextures(1, &img->gl.tex[slot]); }
    }
  }
  if (img->gl.depth_render_buffer) {
    glDeleteRenderbuffers(1, &img->gl.depth_render_buffer);
  }
  if (img->gl.msaa_render_buffer) {
    glDeleteRenderbuffers(1, &img->gl.msaa_render_buffer);
  }
  YK___SG_GL_CHECK_ERROR();
}
YK___SOKOL_PRIVATE GLuint yk___sg_gl_compile_shader(yk__sg_shader_stage stage,
                                                    const char *src) {
  YK__SOKOL_ASSERT(src);
  YK___SG_GL_CHECK_ERROR();
  GLuint gl_shd = glCreateShader(yk___sg_gl_shader_stage(stage));
  glShaderSource(gl_shd, 1, &src, 0);
  glCompileShader(gl_shd);
  GLint compile_status = 0;
  glGetShaderiv(gl_shd, GL_COMPILE_STATUS, &compile_status);
  if (!compile_status) {
    /* compilation failed, log error and delete shader */
    GLint log_len = 0;
    glGetShaderiv(gl_shd, GL_INFO_LOG_LENGTH, &log_len);
    if (log_len > 0) {
      GLchar *log_buf = (GLchar *) YK__SOKOL_MALLOC((size_t) log_len);
      glGetShaderInfoLog(gl_shd, log_len, &log_len, log_buf);
      YK__SOKOL_LOG(log_buf);
      YK__SOKOL_FREE(log_buf);
    }
    glDeleteShader(gl_shd);
    gl_shd = 0;
  }
  YK___SG_GL_CHECK_ERROR();
  return gl_shd;
}
YK___SOKOL_PRIVATE yk__sg_resource_state yk___sg_gl_create_shader(
    yk___sg_shader_t *shd, const yk__sg_shader_desc *desc) {
  YK__SOKOL_ASSERT(shd && desc);
  YK__SOKOL_ASSERT(!shd->gl.prog);
  YK___SG_GL_CHECK_ERROR();
  yk___sg_shader_common_init(&shd->cmn, desc);
  /* copy vertex attribute names over, these are required for GLES2, and optional for GLES3 and GL3.x */
  for (int i = 0; i < YK__SG_MAX_VERTEX_ATTRIBUTES; i++) {
    yk___sg_strcpy(&shd->gl.attrs[i].name, desc->attrs[i].name);
  }
  GLuint gl_vs =
      yk___sg_gl_compile_shader(YK__SG_SHADERSTAGE_VS, desc->vs.source);
  GLuint gl_fs =
      yk___sg_gl_compile_shader(YK__SG_SHADERSTAGE_FS, desc->fs.source);
  if (!(gl_vs && gl_fs)) { return YK__SG_RESOURCESTATE_FAILED; }
  GLuint gl_prog = glCreateProgram();
  glAttachShader(gl_prog, gl_vs);
  glAttachShader(gl_prog, gl_fs);
  glLinkProgram(gl_prog);
  glDeleteShader(gl_vs);
  glDeleteShader(gl_fs);
  YK___SG_GL_CHECK_ERROR();
  GLint link_status;
  glGetProgramiv(gl_prog, GL_LINK_STATUS, &link_status);
  if (!link_status) {
    GLint log_len = 0;
    glGetProgramiv(gl_prog, GL_INFO_LOG_LENGTH, &log_len);
    if (log_len > 0) {
      GLchar *log_buf = (GLchar *) YK__SOKOL_MALLOC((size_t) log_len);
      glGetProgramInfoLog(gl_prog, log_len, &log_len, log_buf);
      YK__SOKOL_LOG(log_buf);
      YK__SOKOL_FREE(log_buf);
    }
    glDeleteProgram(gl_prog);
    return YK__SG_RESOURCESTATE_FAILED;
  }
  shd->gl.prog = gl_prog;
  /* resolve uniforms */
  YK___SG_GL_CHECK_ERROR();
  for (int stage_index = 0; stage_index < YK__SG_NUM_SHADER_STAGES;
       stage_index++) {
    const yk__sg_shader_stage_desc *stage_desc =
        (stage_index == YK__SG_SHADERSTAGE_VS) ? &desc->vs : &desc->fs;
    yk___sg_gl_shader_stage_t *gl_stage = &shd->gl.stage[stage_index];
    for (int ub_index = 0;
         ub_index < shd->cmn.stage[stage_index].num_uniform_blocks;
         ub_index++) {
      const yk__sg_shader_uniform_block_desc *ub_desc =
          &stage_desc->uniform_blocks[ub_index];
      YK__SOKOL_ASSERT(ub_desc->size > 0);
      yk___sg_gl_uniform_block_t *ub = &gl_stage->uniform_blocks[ub_index];
      YK__SOKOL_ASSERT(ub->num_uniforms == 0);
      int cur_uniform_offset = 0;
      for (int u_index = 0; u_index < YK__SG_MAX_UB_MEMBERS; u_index++) {
        const yk__sg_shader_uniform_desc *u_desc = &ub_desc->uniforms[u_index];
        if (u_desc->type == YK__SG_UNIFORMTYPE_INVALID) { break; }
        yk___sg_gl_uniform_t *u = &ub->uniforms[u_index];
        u->type = u_desc->type;
        u->count = (uint16_t) u_desc->array_count;
        u->offset = (uint16_t) cur_uniform_offset;
        cur_uniform_offset += yk___sg_uniform_size(u->type, u->count);
        if (u_desc->name) {
          u->gl_loc = glGetUniformLocation(gl_prog, u_desc->name);
        } else {
          u->gl_loc = u_index;
        }
        ub->num_uniforms++;
      }
      YK__SOKOL_ASSERT(ub_desc->size == (size_t) cur_uniform_offset);
    }
  }
  /* resolve image locations */
  YK___SG_GL_CHECK_ERROR();
  GLuint cur_prog = 0;
  glGetIntegerv(GL_CURRENT_PROGRAM, (GLint *) &cur_prog);
  glUseProgram(gl_prog);
  int gl_tex_slot = 0;
  for (int stage_index = 0; stage_index < YK__SG_NUM_SHADER_STAGES;
       stage_index++) {
    const yk__sg_shader_stage_desc *stage_desc =
        (stage_index == YK__SG_SHADERSTAGE_VS) ? &desc->vs : &desc->fs;
    yk___sg_gl_shader_stage_t *gl_stage = &shd->gl.stage[stage_index];
    for (int img_index = 0; img_index < shd->cmn.stage[stage_index].num_images;
         img_index++) {
      const yk__sg_shader_image_desc *img_desc = &stage_desc->images[img_index];
      YK__SOKOL_ASSERT(img_desc->image_type != YK___SG_IMAGETYPE_DEFAULT);
      yk___sg_gl_shader_image_t *gl_img = &gl_stage->images[img_index];
      GLint gl_loc = img_index;
      if (img_desc->name) {
        gl_loc = glGetUniformLocation(gl_prog, img_desc->name);
      }
      if (gl_loc != -1) {
        gl_img->gl_tex_slot = gl_tex_slot++;
        glUniform1i(gl_loc, gl_img->gl_tex_slot);
      } else {
        gl_img->gl_tex_slot = -1;
      }
    }
  }
  /* it's legal to call glUseProgram with 0 */
  glUseProgram(cur_prog);
  YK___SG_GL_CHECK_ERROR();
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_gl_destroy_shader(yk___sg_shader_t *shd) {
  YK__SOKOL_ASSERT(shd);
  YK___SG_GL_CHECK_ERROR();
  if (shd->gl.prog) {
    yk___sg_gl_cache_invalidate_program(shd->gl.prog);
    glDeleteProgram(shd->gl.prog);
  }
  YK___SG_GL_CHECK_ERROR();
}
YK___SOKOL_PRIVATE yk__sg_resource_state
yk___sg_gl_create_pipeline(yk___sg_pipeline_t *pip, yk___sg_shader_t *shd,
                           const yk__sg_pipeline_desc *desc) {
  YK__SOKOL_ASSERT(pip && shd && desc);
  YK__SOKOL_ASSERT(!pip->shader && pip->cmn.shader_id.id == YK__SG_INVALID_ID);
  YK__SOKOL_ASSERT(desc->shader.id == shd->slot.id);
  YK__SOKOL_ASSERT(shd->gl.prog);
  pip->shader = shd;
  yk___sg_pipeline_common_init(&pip->cmn, desc);
  pip->gl.primitive_type = desc->primitive_type;
  pip->gl.depth = desc->depth;
  pip->gl.stencil = desc->stencil;
  // FIXME: blend color and write mask per draw-buffer-attachment (requires GL4)
  pip->gl.blend = desc->colors[0].blend;
  for (int i = 0; i < YK__SG_MAX_COLOR_ATTACHMENTS; i++) {
    pip->gl.color_write_mask[i] = desc->colors[i].write_mask;
  }
  pip->gl.cull_mode = desc->cull_mode;
  pip->gl.face_winding = desc->face_winding;
  pip->gl.sample_count = desc->sample_count;
  pip->gl.alpha_to_coverage_enabled = desc->alpha_to_coverage_enabled;
  /* resolve vertex attributes */
  for (int attr_index = 0; attr_index < YK__SG_MAX_VERTEX_ATTRIBUTES;
       attr_index++) {
    pip->gl.attrs[attr_index].vb_index = -1;
  }
  for (int attr_index = 0; attr_index < yk___sg.limits.max_vertex_attrs;
       attr_index++) {
    const yk__sg_vertex_attr_desc *a_desc = &desc->layout.attrs[attr_index];
    if (a_desc->format == YK__SG_VERTEXFORMAT_INVALID) { break; }
    YK__SOKOL_ASSERT(a_desc->buffer_index < YK__SG_MAX_SHADERSTAGE_BUFFERS);
    const yk__sg_buffer_layout_desc *l_desc =
        &desc->layout.buffers[a_desc->buffer_index];
    const yk__sg_vertex_step step_func = l_desc->step_func;
    const int step_rate = l_desc->step_rate;
    GLint attr_loc = attr_index;
    if (!yk___sg_strempty(&shd->gl.attrs[attr_index].name)) {
      attr_loc =
          glGetAttribLocation(pip->shader->gl.prog,
                              yk___sg_strptr(&shd->gl.attrs[attr_index].name));
    }
    YK__SOKOL_ASSERT(attr_loc < (GLint) yk___sg.limits.max_vertex_attrs);
    if (attr_loc != -1) {
      yk___sg_gl_attr_t *gl_attr = &pip->gl.attrs[attr_loc];
      YK__SOKOL_ASSERT(gl_attr->vb_index == -1);
      gl_attr->vb_index = (int8_t) a_desc->buffer_index;
      if (step_func == YK__SG_VERTEXSTEP_PER_VERTEX) {
        gl_attr->divisor = 0;
      } else {
        gl_attr->divisor = (int8_t) step_rate;
        pip->cmn.use_instanced_draw = true;
      }
      YK__SOKOL_ASSERT(l_desc->stride > 0);
      gl_attr->stride = (uint8_t) l_desc->stride;
      gl_attr->offset = a_desc->offset;
      gl_attr->size = (uint8_t) yk___sg_gl_vertexformat_size(a_desc->format);
      gl_attr->type = yk___sg_gl_vertexformat_type(a_desc->format);
      gl_attr->normalized = yk___sg_gl_vertexformat_normalized(a_desc->format);
      pip->cmn.vertex_layout_valid[a_desc->buffer_index] = true;
    } else {
      YK__SOKOL_LOG("Vertex attribute not found in shader: ");
      YK__SOKOL_LOG(yk___sg_strptr(&shd->gl.attrs[attr_index].name));
    }
  }
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_gl_destroy_pipeline(yk___sg_pipeline_t *pip) {
  YK__SOKOL_ASSERT(pip);
  yk___sg_gl_cache_invalidate_pipeline(pip);
}
/*
    yk___sg_create_pass

    att_imgs must point to a _sg_image* att_imgs[YK__SG_MAX_COLOR_ATTACHMENTS+1] array,
    first entries are the color attachment images (or nullptr), last entry
    is the depth-stencil image (or nullptr).
*/
YK___SOKOL_PRIVATE yk__sg_resource_state
yk___sg_gl_create_pass(yk___sg_pass_t *pass, yk___sg_image_t **att_images,
                       const yk__sg_pass_desc *desc) {
  YK__SOKOL_ASSERT(pass && att_images && desc);
  YK__SOKOL_ASSERT(att_images && att_images[0]);
  YK___SG_GL_CHECK_ERROR();
  yk___sg_pass_common_init(&pass->cmn, desc);
  /* copy image pointers */
  const yk__sg_pass_attachment_desc *att_desc;
  for (int i = 0; i < pass->cmn.num_color_atts; i++) {
    att_desc = &desc->color_attachments[i];
    YK__SOKOL_ASSERT(att_desc->image.id != YK__SG_INVALID_ID);
    YK__SOKOL_ASSERT(0 == pass->gl.color_atts[i].image);
    YK__SOKOL_ASSERT(att_images[i] &&
                     (att_images[i]->slot.id == att_desc->image.id));
    YK__SOKOL_ASSERT(yk___sg_is_valid_rendertarget_color_format(
        att_images[i]->cmn.pixel_format));
    pass->gl.color_atts[i].image = att_images[i];
  }
  YK__SOKOL_ASSERT(0 == pass->gl.ds_att.image);
  att_desc = &desc->depth_stencil_attachment;
  if (att_desc->image.id != YK__SG_INVALID_ID) {
    const int ds_img_index = YK__SG_MAX_COLOR_ATTACHMENTS;
    YK__SOKOL_ASSERT(att_images[ds_img_index] &&
                     (att_images[ds_img_index]->slot.id == att_desc->image.id));
    YK__SOKOL_ASSERT(yk___sg_is_valid_rendertarget_depth_format(
        att_images[ds_img_index]->cmn.pixel_format));
    pass->gl.ds_att.image = att_images[ds_img_index];
  }
  /* store current framebuffer binding (restored at end of function) */
  GLuint gl_orig_fb;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint *) &gl_orig_fb);
  /* create a framebuffer object */
  glGenFramebuffers(1, &pass->gl.fb);
  glBindFramebuffer(GL_FRAMEBUFFER, pass->gl.fb);
  /* attach msaa render buffer or textures */
  const bool is_msaa = (0 != att_images[0]->gl.msaa_render_buffer);
  if (is_msaa) {
    for (int i = 0; i < YK__SG_MAX_COLOR_ATTACHMENTS; i++) {
      const yk___sg_image_t *att_img = pass->gl.color_atts[i].image;
      if (att_img) {
        const GLuint gl_render_buffer = att_img->gl.msaa_render_buffer;
        YK__SOKOL_ASSERT(gl_render_buffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                  (GLenum) (GL_COLOR_ATTACHMENT0 + i),
                                  GL_RENDERBUFFER, gl_render_buffer);
      }
    }
  } else {
    for (int i = 0; i < YK__SG_MAX_COLOR_ATTACHMENTS; i++) {
      const yk___sg_image_t *att_img = pass->gl.color_atts[i].image;
      const int mip_level = pass->cmn.color_atts[i].mip_level;
      const int slice = pass->cmn.color_atts[i].slice;
      if (att_img) {
        const GLuint gl_tex = att_img->gl.tex[0];
        YK__SOKOL_ASSERT(gl_tex);
        const GLenum gl_att = (GLenum) (GL_COLOR_ATTACHMENT0 + i);
        switch (att_img->cmn.type) {
          case YK__SG_IMAGETYPE_2D:
            glFramebufferTexture2D(GL_FRAMEBUFFER, gl_att, GL_TEXTURE_2D,
                                   gl_tex, mip_level);
            break;
          case YK__SG_IMAGETYPE_CUBE:
            glFramebufferTexture2D(GL_FRAMEBUFFER, gl_att,
                                   yk___sg_gl_cubeface_target(slice), gl_tex,
                                   mip_level);
            break;
          default:
/* 3D- or array-texture */
#if !defined(YK__SOKOL_GLES2)
            if (!yk___sg.gl.gles2) {
              glFramebufferTextureLayer(GL_FRAMEBUFFER, gl_att, gl_tex,
                                        mip_level, slice);
            }
#endif
            break;
        }
      }
    }
  }
  /* attach depth-stencil buffer to framebuffer */
  if (pass->gl.ds_att.image) {
    const GLuint gl_render_buffer =
        pass->gl.ds_att.image->gl.depth_render_buffer;
    YK__SOKOL_ASSERT(gl_render_buffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, gl_render_buffer);
    if (yk___sg_is_depth_stencil_format(
            pass->gl.ds_att.image->cmn.pixel_format)) {
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                                GL_RENDERBUFFER, gl_render_buffer);
    }
  }
  /* check if framebuffer is complete */
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    YK__SOKOL_LOG("Framebuffer completeness check failed!\n");
    return YK__SG_RESOURCESTATE_FAILED;
  }
/* setup color attachments for the framebuffer */
#if !defined(YK__SOKOL_GLES2)
  if (!yk___sg.gl.gles2) {
    GLenum att[YK__SG_MAX_COLOR_ATTACHMENTS] = {
        GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3};
    glDrawBuffers(pass->cmn.num_color_atts, att);
  }
#endif
  /* create MSAA resolve framebuffers if necessary */
  if (is_msaa) {
    for (int i = 0; i < YK__SG_MAX_COLOR_ATTACHMENTS; i++) {
      yk___sg_gl_attachment_t *gl_att = &pass->gl.color_atts[i];
      yk___sg_pass_attachment_t *cmn_att = &pass->cmn.color_atts[i];
      if (gl_att->image) {
        YK__SOKOL_ASSERT(0 == gl_att->gl_msaa_resolve_buffer);
        glGenFramebuffers(1, &gl_att->gl_msaa_resolve_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, gl_att->gl_msaa_resolve_buffer);
        const GLuint gl_tex = gl_att->image->gl.tex[0];
        YK__SOKOL_ASSERT(gl_tex);
        switch (gl_att->image->cmn.type) {
          case YK__SG_IMAGETYPE_2D:
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_2D, gl_tex, cmn_att->mip_level);
            break;
          case YK__SG_IMAGETYPE_CUBE:
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   yk___sg_gl_cubeface_target(cmn_att->slice),
                                   gl_tex, cmn_att->mip_level);
            break;
          default:
#if !defined(YK__SOKOL_GLES2)
            if (!yk___sg.gl.gles2) {
              glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                        gl_tex, cmn_att->mip_level,
                                        cmn_att->slice);
            }
#endif
            break;
        }
        /* check if framebuffer is complete */
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
            GL_FRAMEBUFFER_COMPLETE) {
          YK__SOKOL_LOG(
              "Framebuffer completeness check failed (msaa resolve buffer)!\n");
          return YK__SG_RESOURCESTATE_FAILED;
        }
/* setup color attachments for the framebuffer */
#if !defined(YK__SOKOL_GLES2)
        if (!yk___sg.gl.gles2) {
          const GLenum gl_draw_bufs = GL_COLOR_ATTACHMENT0;
          glDrawBuffers(1, &gl_draw_bufs);
        }
#endif
      }
    }
  }
  /* restore original framebuffer binding */
  glBindFramebuffer(GL_FRAMEBUFFER, gl_orig_fb);
  YK___SG_GL_CHECK_ERROR();
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_gl_destroy_pass(yk___sg_pass_t *pass) {
  YK__SOKOL_ASSERT(pass);
  YK__SOKOL_ASSERT(pass != yk___sg.gl.cur_pass);
  YK___SG_GL_CHECK_ERROR();
  if (0 != pass->gl.fb) { glDeleteFramebuffers(1, &pass->gl.fb); }
  for (int i = 0; i < YK__SG_MAX_COLOR_ATTACHMENTS; i++) {
    if (pass->gl.color_atts[i].gl_msaa_resolve_buffer) {
      glDeleteFramebuffers(1, &pass->gl.color_atts[i].gl_msaa_resolve_buffer);
    }
  }
  if (pass->gl.ds_att.gl_msaa_resolve_buffer) {
    glDeleteFramebuffers(1, &pass->gl.ds_att.gl_msaa_resolve_buffer);
  }
  YK___SG_GL_CHECK_ERROR();
}
YK___SOKOL_PRIVATE yk___sg_image_t *
yk___sg_gl_pass_color_image(const yk___sg_pass_t *pass, int index) {
  YK__SOKOL_ASSERT(pass && (index >= 0) &&
                   (index < YK__SG_MAX_COLOR_ATTACHMENTS));
  /* NOTE: may return null */
  return pass->gl.color_atts[index].image;
}
YK___SOKOL_PRIVATE yk___sg_image_t *
yk___sg_gl_pass_ds_image(const yk___sg_pass_t *pass) {
  /* NOTE: may return null */
  YK__SOKOL_ASSERT(pass);
  return pass->gl.ds_att.image;
}
YK___SOKOL_PRIVATE void yk___sg_gl_begin_pass(yk___sg_pass_t *pass,
                                              const yk__sg_pass_action *action,
                                              int w, int h) {
  /* FIXME: what if a texture used as render target is still bound, should we
       unbind all currently bound textures in begin pass? */
  YK__SOKOL_ASSERT(action);
  YK__SOKOL_ASSERT(!yk___sg.gl.in_pass);
  YK___SG_GL_CHECK_ERROR();
  yk___sg.gl.in_pass = true;
  yk___sg.gl.cur_pass = pass; /* can be 0 */
  if (pass) {
    yk___sg.gl.cur_pass_id.id = pass->slot.id;
  } else {
    yk___sg.gl.cur_pass_id.id = YK__SG_INVALID_ID;
  }
  yk___sg.gl.cur_pass_width = w;
  yk___sg.gl.cur_pass_height = h;
  /* number of color attachments */
  const int num_color_atts = pass ? pass->cmn.num_color_atts : 1;
  /* bind the render pass framebuffer */
  if (pass) {
    /* offscreen pass */
    YK__SOKOL_ASSERT(pass->gl.fb);
    glBindFramebuffer(GL_FRAMEBUFFER, pass->gl.fb);
  } else {
    /* default pass */
    YK__SOKOL_ASSERT(yk___sg.gl.cur_context);
    glBindFramebuffer(GL_FRAMEBUFFER,
                      yk___sg.gl.cur_context->default_framebuffer);
  }
  glViewport(0, 0, w, h);
  glScissor(0, 0, w, h);
  /* clear color and depth-stencil attachments if needed */
  bool clear_color = false;
  for (int i = 0; i < num_color_atts; i++) {
    if (YK__SG_ACTION_CLEAR == action->colors[i].action) {
      clear_color = true;
      break;
    }
  }
  const bool clear_depth = (action->depth.action == YK__SG_ACTION_CLEAR);
  const bool clear_stencil = (action->stencil.action == YK__SG_ACTION_CLEAR);
  bool need_pip_cache_flush = false;
  if (clear_color) {
    bool need_color_mask_flush = false;
    // NOTE: not a bug to iterate over all possible color attachments
    for (int i = 0; i < YK__SG_MAX_COLOR_ATTACHMENTS; i++) {
      if (YK__SG_COLORMASK_RGBA != yk___sg.gl.cache.color_write_mask[i]) {
        need_pip_cache_flush = true;
        need_color_mask_flush = true;
        yk___sg.gl.cache.color_write_mask[i] = YK__SG_COLORMASK_RGBA;
      }
    }
    if (need_color_mask_flush) {
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
  }
  if (clear_depth) {
    if (!yk___sg.gl.cache.depth.write_enabled) {
      need_pip_cache_flush = true;
      yk___sg.gl.cache.depth.write_enabled = true;
      glDepthMask(GL_TRUE);
    }
    if (yk___sg.gl.cache.depth.compare != YK__SG_COMPAREFUNC_ALWAYS) {
      need_pip_cache_flush = true;
      yk___sg.gl.cache.depth.compare = YK__SG_COMPAREFUNC_ALWAYS;
      glDepthFunc(GL_ALWAYS);
    }
  }
  if (clear_stencil) {
    if (yk___sg.gl.cache.stencil.write_mask != 0xFF) {
      need_pip_cache_flush = true;
      yk___sg.gl.cache.stencil.write_mask = 0xFF;
      glStencilMask(0xFF);
    }
  }
  if (need_pip_cache_flush) {
    /* we messed with the state cache directly, need to clear cached
           pipeline to force re-evaluation in next yk__sg_apply_pipeline() */
    yk___sg.gl.cache.cur_pipeline = 0;
    yk___sg.gl.cache.cur_pipeline_id.id = YK__SG_INVALID_ID;
  }
  bool use_mrt_clear = (0 != pass);
#if defined(YK__SOKOL_GLES2)
  use_mrt_clear = false;
#else
  if (yk___sg.gl.gles2) { use_mrt_clear = false; }
#endif
  if (!use_mrt_clear) {
    GLbitfield clear_mask = 0;
    if (clear_color) {
      clear_mask |= GL_COLOR_BUFFER_BIT;
      const yk__sg_color c = action->colors[0].value;
      glClearColor(c.r, c.g, c.b, c.a);
    }
    if (clear_depth) {
      clear_mask |= GL_DEPTH_BUFFER_BIT;
#ifdef YK__SOKOL_GLCORE33
      glClearDepth(action->depth.value);
#else
      glClearDepthf(action->depth.value);
#endif
    }
    if (clear_stencil) {
      clear_mask |= GL_STENCIL_BUFFER_BIT;
      glClearStencil(action->stencil.value);
    }
    if (0 != clear_mask) { glClear(clear_mask); }
  }
#if !defined YK__SOKOL_GLES2
  else {
    YK__SOKOL_ASSERT(pass);
    for (int i = 0; i < num_color_atts; i++) {
      if (action->colors[i].action == YK__SG_ACTION_CLEAR) {
        glClearBufferfv(GL_COLOR, i, &action->colors[i].value.r);
      }
    }
    if (pass->gl.ds_att.image) {
      if (clear_depth && clear_stencil) {
        glClearBufferfi(GL_DEPTH_STENCIL, 0, action->depth.value,
                        action->stencil.value);
      } else if (clear_depth) {
        glClearBufferfv(GL_DEPTH, 0, &action->depth.value);
      } else if (clear_stencil) {
        GLint val = (GLint) action->stencil.value;
        glClearBufferiv(GL_STENCIL, 0, &val);
      }
    }
  }
#endif
  YK___SG_GL_CHECK_ERROR();
}
YK___SOKOL_PRIVATE void yk___sg_gl_end_pass(void) {
  YK__SOKOL_ASSERT(yk___sg.gl.in_pass);
  YK___SG_GL_CHECK_ERROR();
/* if this was an offscreen pass, and MSAA rendering was used, need
       to resolve into the pass images */
#if !defined(YK__SOKOL_GLES2)
  if (!yk___sg.gl.gles2 && yk___sg.gl.cur_pass) {
    /* check if the pass object is still valid */
    const yk___sg_pass_t *pass = yk___sg.gl.cur_pass;
    YK__SOKOL_ASSERT(pass->slot.id == yk___sg.gl.cur_pass_id.id);
    bool is_msaa =
        (0 != yk___sg.gl.cur_pass->gl.color_atts[0].gl_msaa_resolve_buffer);
    if (is_msaa) {
      YK__SOKOL_ASSERT(pass->gl.fb);
      glBindFramebuffer(GL_READ_FRAMEBUFFER, pass->gl.fb);
      YK__SOKOL_ASSERT(pass->gl.color_atts[0].image);
      const int w = pass->gl.color_atts[0].image->cmn.width;
      const int h = pass->gl.color_atts[0].image->cmn.height;
      for (int att_index = 0; att_index < YK__SG_MAX_COLOR_ATTACHMENTS;
           att_index++) {
        const yk___sg_gl_attachment_t *gl_att = &pass->gl.color_atts[att_index];
        if (gl_att->image) {
          YK__SOKOL_ASSERT(gl_att->gl_msaa_resolve_buffer);
          glBindFramebuffer(GL_DRAW_FRAMEBUFFER,
                            gl_att->gl_msaa_resolve_buffer);
          glReadBuffer((GLenum) (GL_COLOR_ATTACHMENT0 + att_index));
          glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT,
                            GL_NEAREST);
        } else {
          break;
        }
      }
    }
  }
#endif
  yk___sg.gl.cur_pass = 0;
  yk___sg.gl.cur_pass_id.id = YK__SG_INVALID_ID;
  yk___sg.gl.cur_pass_width = 0;
  yk___sg.gl.cur_pass_height = 0;
  YK__SOKOL_ASSERT(yk___sg.gl.cur_context);
  glBindFramebuffer(GL_FRAMEBUFFER,
                    yk___sg.gl.cur_context->default_framebuffer);
  yk___sg.gl.in_pass = false;
  YK___SG_GL_CHECK_ERROR();
}
YK___SOKOL_PRIVATE void yk___sg_gl_apply_viewport(int x, int y, int w, int h,
                                                  bool origin_top_left) {
  YK__SOKOL_ASSERT(yk___sg.gl.in_pass);
  y = origin_top_left ? (yk___sg.gl.cur_pass_height - (y + h)) : y;
  glViewport(x, y, w, h);
}
YK___SOKOL_PRIVATE void yk___sg_gl_apply_scissor_rect(int x, int y, int w,
                                                      int h,
                                                      bool origin_top_left) {
  YK__SOKOL_ASSERT(yk___sg.gl.in_pass);
  y = origin_top_left ? (yk___sg.gl.cur_pass_height - (y + h)) : y;
  glScissor(x, y, w, h);
}
YK___SOKOL_PRIVATE void yk___sg_gl_apply_pipeline(yk___sg_pipeline_t *pip) {
  YK__SOKOL_ASSERT(pip);
  YK__SOKOL_ASSERT(pip->shader &&
                   (pip->cmn.shader_id.id == pip->shader->slot.id));
  YK___SG_GL_CHECK_ERROR();
  if ((yk___sg.gl.cache.cur_pipeline != pip) ||
      (yk___sg.gl.cache.cur_pipeline_id.id != pip->slot.id)) {
    yk___sg.gl.cache.cur_pipeline = pip;
    yk___sg.gl.cache.cur_pipeline_id.id = pip->slot.id;
    yk___sg.gl.cache.cur_primitive_type =
        yk___sg_gl_primitive_type(pip->gl.primitive_type);
    yk___sg.gl.cache.cur_index_type =
        yk___sg_gl_index_type(pip->cmn.index_type);
    /* update depth state */
    {
      const yk__sg_depth_state *state_ds = &pip->gl.depth;
      yk__sg_depth_state *cache_ds = &yk___sg.gl.cache.depth;
      if (state_ds->compare != cache_ds->compare) {
        cache_ds->compare = state_ds->compare;
        glDepthFunc(yk___sg_gl_compare_func(state_ds->compare));
      }
      if (state_ds->write_enabled != cache_ds->write_enabled) {
        cache_ds->write_enabled = state_ds->write_enabled;
        glDepthMask(state_ds->write_enabled);
      }
      if (!yk___sg_fequal(state_ds->bias, cache_ds->bias, 0.000001f) ||
          !yk___sg_fequal(state_ds->bias_slope_scale,
                          cache_ds->bias_slope_scale, 0.000001f)) {
        /* according to ANGLE's D3D11 backend:
                    D3D11 SlopeScaledDepthBias ==> GL polygonOffsetFactor
                    D3D11 DepthBias ==> GL polygonOffsetUnits
                    DepthBiasClamp has no meaning on GL
                */
        cache_ds->bias = state_ds->bias;
        cache_ds->bias_slope_scale = state_ds->bias_slope_scale;
        glPolygonOffset(state_ds->bias_slope_scale, state_ds->bias);
        bool po_enabled = true;
        if (yk___sg_fequal(state_ds->bias, 0.0f, 0.000001f) &&
            yk___sg_fequal(state_ds->bias_slope_scale, 0.0f, 0.000001f)) {
          po_enabled = false;
        }
        if (po_enabled != yk___sg.gl.cache.polygon_offset_enabled) {
          yk___sg.gl.cache.polygon_offset_enabled = po_enabled;
          if (po_enabled) {
            glEnable(GL_POLYGON_OFFSET_FILL);
          } else {
            glDisable(GL_POLYGON_OFFSET_FILL);
          }
        }
      }
    }
    /* update stencil state */
    {
      const yk__sg_stencil_state *state_ss = &pip->gl.stencil;
      yk__sg_stencil_state *cache_ss = &yk___sg.gl.cache.stencil;
      if (state_ss->enabled != cache_ss->enabled) {
        cache_ss->enabled = state_ss->enabled;
        if (state_ss->enabled) {
          glEnable(GL_STENCIL_TEST);
        } else {
          glDisable(GL_STENCIL_TEST);
        }
      }
      if (state_ss->write_mask != cache_ss->write_mask) {
        cache_ss->write_mask = state_ss->write_mask;
        glStencilMask(state_ss->write_mask);
      }
      for (int i = 0; i < 2; i++) {
        const yk__sg_stencil_face_state *state_sfs =
            (i == 0) ? &state_ss->front : &state_ss->back;
        yk__sg_stencil_face_state *cache_sfs =
            (i == 0) ? &cache_ss->front : &cache_ss->back;
        GLenum gl_face = (i == 0) ? GL_FRONT : GL_BACK;
        if ((state_sfs->compare != cache_sfs->compare) ||
            (state_ss->read_mask != cache_ss->read_mask) ||
            (state_ss->ref != cache_ss->ref)) {
          cache_sfs->compare = state_sfs->compare;
          glStencilFuncSeparate(gl_face,
                                yk___sg_gl_compare_func(state_sfs->compare),
                                state_ss->ref, state_ss->read_mask);
        }
        if ((state_sfs->fail_op != cache_sfs->fail_op) ||
            (state_sfs->depth_fail_op != cache_sfs->depth_fail_op) ||
            (state_sfs->pass_op != cache_sfs->pass_op)) {
          cache_sfs->fail_op = state_sfs->fail_op;
          cache_sfs->depth_fail_op = state_sfs->depth_fail_op;
          cache_sfs->pass_op = state_sfs->pass_op;
          glStencilOpSeparate(gl_face,
                              yk___sg_gl_stencil_op(state_sfs->fail_op),
                              yk___sg_gl_stencil_op(state_sfs->depth_fail_op),
                              yk___sg_gl_stencil_op(state_sfs->pass_op));
        }
      }
      cache_ss->read_mask = state_ss->read_mask;
      cache_ss->ref = state_ss->ref;
    }
    /* update blend state
            FIXME: separate blend state per color attachment not support, needs GL4
        */
    {
      const yk__sg_blend_state *state_bs = &pip->gl.blend;
      yk__sg_blend_state *cache_bs = &yk___sg.gl.cache.blend;
      if (state_bs->enabled != cache_bs->enabled) {
        cache_bs->enabled = state_bs->enabled;
        if (state_bs->enabled) {
          glEnable(GL_BLEND);
        } else {
          glDisable(GL_BLEND);
        }
      }
      if ((state_bs->src_factor_rgb != cache_bs->src_factor_rgb) ||
          (state_bs->dst_factor_rgb != cache_bs->dst_factor_rgb) ||
          (state_bs->src_factor_alpha != cache_bs->src_factor_alpha) ||
          (state_bs->dst_factor_alpha != cache_bs->dst_factor_alpha)) {
        cache_bs->src_factor_rgb = state_bs->src_factor_rgb;
        cache_bs->dst_factor_rgb = state_bs->dst_factor_rgb;
        cache_bs->src_factor_alpha = state_bs->src_factor_alpha;
        cache_bs->dst_factor_alpha = state_bs->dst_factor_alpha;
        glBlendFuncSeparate(
            yk___sg_gl_blend_factor(state_bs->src_factor_rgb),
            yk___sg_gl_blend_factor(state_bs->dst_factor_rgb),
            yk___sg_gl_blend_factor(state_bs->src_factor_alpha),
            yk___sg_gl_blend_factor(state_bs->dst_factor_alpha));
      }
      if ((state_bs->op_rgb != cache_bs->op_rgb) ||
          (state_bs->op_alpha != cache_bs->op_alpha)) {
        cache_bs->op_rgb = state_bs->op_rgb;
        cache_bs->op_alpha = state_bs->op_alpha;
        glBlendEquationSeparate(yk___sg_gl_blend_op(state_bs->op_rgb),
                                yk___sg_gl_blend_op(state_bs->op_alpha));
      }
    }
    /* standalone state */
    for (GLuint i = 0; i < (GLuint) pip->cmn.color_attachment_count; i++) {
      if (pip->gl.color_write_mask[i] != yk___sg.gl.cache.color_write_mask[i]) {
        const yk__sg_color_mask cm = pip->gl.color_write_mask[i];
        yk___sg.gl.cache.color_write_mask[i] = cm;
#ifdef YK__SOKOL_GLCORE33
        glColorMaski(
            i, (cm & YK__SG_COLORMASK_R) != 0, (cm & YK__SG_COLORMASK_G) != 0,
            (cm & YK__SG_COLORMASK_B) != 0, (cm & YK__SG_COLORMASK_A) != 0);
#else
        if (0 == i) {
          glColorMask(
              (cm & YK__SG_COLORMASK_R) != 0, (cm & YK__SG_COLORMASK_G) != 0,
              (cm & YK__SG_COLORMASK_B) != 0, (cm & YK__SG_COLORMASK_A) != 0);
        }
#endif
      }
    }
    if (!yk___sg_fequal(pip->cmn.blend_color.r, yk___sg.gl.cache.blend_color.r,
                        0.0001f) ||
        !yk___sg_fequal(pip->cmn.blend_color.g, yk___sg.gl.cache.blend_color.g,
                        0.0001f) ||
        !yk___sg_fequal(pip->cmn.blend_color.b, yk___sg.gl.cache.blend_color.b,
                        0.0001f) ||
        !yk___sg_fequal(pip->cmn.blend_color.a, yk___sg.gl.cache.blend_color.a,
                        0.0001f)) {
      yk__sg_color c = pip->cmn.blend_color;
      yk___sg.gl.cache.blend_color = c;
      glBlendColor(c.r, c.g, c.b, c.a);
    }
    if (pip->gl.cull_mode != yk___sg.gl.cache.cull_mode) {
      yk___sg.gl.cache.cull_mode = pip->gl.cull_mode;
      if (YK__SG_CULLMODE_NONE == pip->gl.cull_mode) {
        glDisable(GL_CULL_FACE);
      } else {
        glEnable(GL_CULL_FACE);
        GLenum gl_mode =
            (YK__SG_CULLMODE_FRONT == pip->gl.cull_mode) ? GL_FRONT : GL_BACK;
        glCullFace(gl_mode);
      }
    }
    if (pip->gl.face_winding != yk___sg.gl.cache.face_winding) {
      yk___sg.gl.cache.face_winding = pip->gl.face_winding;
      GLenum gl_winding =
          (YK__SG_FACEWINDING_CW == pip->gl.face_winding) ? GL_CW : GL_CCW;
      glFrontFace(gl_winding);
    }
    if (pip->gl.alpha_to_coverage_enabled !=
        yk___sg.gl.cache.alpha_to_coverage_enabled) {
      yk___sg.gl.cache.alpha_to_coverage_enabled =
          pip->gl.alpha_to_coverage_enabled;
      if (pip->gl.alpha_to_coverage_enabled) {
        glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
      } else {
        glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
      }
    }
#ifdef YK__SOKOL_GLCORE33
    if (pip->gl.sample_count != yk___sg.gl.cache.sample_count) {
      yk___sg.gl.cache.sample_count = pip->gl.sample_count;
      if (pip->gl.sample_count > 1) {
        glEnable(GL_MULTISAMPLE);
      } else {
        glDisable(GL_MULTISAMPLE);
      }
    }
#endif
    /* bind shader program */
    if (pip->shader->gl.prog != yk___sg.gl.cache.prog) {
      yk___sg.gl.cache.prog = pip->shader->gl.prog;
      glUseProgram(pip->shader->gl.prog);
    }
  }
  YK___SG_GL_CHECK_ERROR();
}
YK___SOKOL_PRIVATE void yk___sg_gl_apply_bindings(
    yk___sg_pipeline_t *pip, yk___sg_buffer_t **vbs, const int *vb_offsets,
    int num_vbs, yk___sg_buffer_t *ib, int ib_offset, yk___sg_image_t **vs_imgs,
    int num_vs_imgs, yk___sg_image_t **fs_imgs, int num_fs_imgs) {
  YK__SOKOL_ASSERT(pip);
  YK___SOKOL_UNUSED(num_fs_imgs);
  YK___SOKOL_UNUSED(num_vs_imgs);
  YK___SOKOL_UNUSED(num_vbs);
  YK___SG_GL_CHECK_ERROR();
  /* bind textures */
  YK___SG_GL_CHECK_ERROR();
  for (int stage_index = 0; stage_index < YK__SG_NUM_SHADER_STAGES;
       stage_index++) {
    const yk___sg_shader_stage_t *stage = &pip->shader->cmn.stage[stage_index];
    const yk___sg_gl_shader_stage_t *gl_stage =
        &pip->shader->gl.stage[stage_index];
    yk___sg_image_t **imgs =
        (stage_index == YK__SG_SHADERSTAGE_VS) ? vs_imgs : fs_imgs;
    YK__SOKOL_ASSERT(
        ((stage_index == YK__SG_SHADERSTAGE_VS) ? num_vs_imgs : num_fs_imgs) ==
        stage->num_images);
    for (int img_index = 0; img_index < stage->num_images; img_index++) {
      const yk___sg_gl_shader_image_t *gl_shd_img =
          &gl_stage->images[img_index];
      if (gl_shd_img->gl_tex_slot != -1) {
        yk___sg_image_t *img = imgs[img_index];
        const GLuint gl_tex = img->gl.tex[img->cmn.active_slot];
        YK__SOKOL_ASSERT(img && img->gl.target);
        YK__SOKOL_ASSERT((gl_shd_img->gl_tex_slot != -1) && gl_tex);
        yk___sg_gl_cache_bind_texture(gl_shd_img->gl_tex_slot, img->gl.target,
                                      gl_tex);
      }
    }
  }
  YK___SG_GL_CHECK_ERROR();
  /* index buffer (can be 0) */
  const GLuint gl_ib = ib ? ib->gl.buf[ib->cmn.active_slot] : 0;
  yk___sg_gl_cache_bind_buffer(GL_ELEMENT_ARRAY_BUFFER, gl_ib);
  yk___sg.gl.cache.cur_ib_offset = ib_offset;
  /* vertex attributes */
  for (GLuint attr_index = 0;
       attr_index < (GLuint) yk___sg.limits.max_vertex_attrs; attr_index++) {
    yk___sg_gl_attr_t *attr = &pip->gl.attrs[attr_index];
    yk___sg_gl_cache_attr_t *cache_attr = &yk___sg.gl.cache.attrs[attr_index];
    bool cache_attr_dirty = false;
    int vb_offset = 0;
    GLuint gl_vb = 0;
    if (attr->vb_index >= 0) {
      /* attribute is enabled */
      YK__SOKOL_ASSERT(attr->vb_index < num_vbs);
      yk___sg_buffer_t *vb = vbs[attr->vb_index];
      YK__SOKOL_ASSERT(vb);
      gl_vb = vb->gl.buf[vb->cmn.active_slot];
      vb_offset = vb_offsets[attr->vb_index] + attr->offset;
      if ((gl_vb != cache_attr->gl_vbuf) ||
          (attr->size != cache_attr->gl_attr.size) ||
          (attr->type != cache_attr->gl_attr.type) ||
          (attr->normalized != cache_attr->gl_attr.normalized) ||
          (attr->stride != cache_attr->gl_attr.stride) ||
          (vb_offset != cache_attr->gl_attr.offset) ||
          (cache_attr->gl_attr.divisor != attr->divisor)) {
        yk___sg_gl_cache_bind_buffer(GL_ARRAY_BUFFER, gl_vb);
        glVertexAttribPointer(attr_index, attr->size, attr->type,
                              attr->normalized, attr->stride,
                              (const GLvoid *) (GLintptr) vb_offset);
#if defined(YK___SOKOL_GL_INSTANCING_ENABLED)
        if (yk___sg.features.instancing) {
          glVertexAttribDivisor(attr_index, (GLuint) attr->divisor);
        }
#endif
        cache_attr_dirty = true;
      }
      if (cache_attr->gl_attr.vb_index == -1) {
        glEnableVertexAttribArray(attr_index);
        cache_attr_dirty = true;
      }
    } else {
      /* attribute is disabled */
      if (cache_attr->gl_attr.vb_index != -1) {
        glDisableVertexAttribArray(attr_index);
        cache_attr_dirty = true;
      }
    }
    if (cache_attr_dirty) {
      cache_attr->gl_attr = *attr;
      cache_attr->gl_attr.offset = vb_offset;
      cache_attr->gl_vbuf = gl_vb;
    }
  }
  YK___SG_GL_CHECK_ERROR();
}
YK___SOKOL_PRIVATE void
yk___sg_gl_apply_uniforms(yk__sg_shader_stage stage_index, int ub_index,
                          const yk__sg_range *data) {
  YK__SOKOL_ASSERT(yk___sg.gl.cache.cur_pipeline);
  YK__SOKOL_ASSERT(yk___sg.gl.cache.cur_pipeline->slot.id ==
                   yk___sg.gl.cache.cur_pipeline_id.id);
  YK__SOKOL_ASSERT(yk___sg.gl.cache.cur_pipeline->shader->slot.id ==
                   yk___sg.gl.cache.cur_pipeline->cmn.shader_id.id);
  YK__SOKOL_ASSERT(yk___sg.gl.cache.cur_pipeline->shader->cmn.stage[stage_index]
                       .num_uniform_blocks > ub_index);
  YK__SOKOL_ASSERT(yk___sg.gl.cache.cur_pipeline->shader->cmn.stage[stage_index]
                       .uniform_blocks[ub_index]
                       .size == data->size);
  const yk___sg_gl_shader_stage_t *gl_stage =
      &yk___sg.gl.cache.cur_pipeline->shader->gl.stage[stage_index];
  const yk___sg_gl_uniform_block_t *gl_ub = &gl_stage->uniform_blocks[ub_index];
  for (int u_index = 0; u_index < gl_ub->num_uniforms; u_index++) {
    const yk___sg_gl_uniform_t *u = &gl_ub->uniforms[u_index];
    YK__SOKOL_ASSERT(u->type != YK__SG_UNIFORMTYPE_INVALID);
    if (u->gl_loc == -1) { continue; }
    GLfloat *ptr = (GLfloat *) (((uint8_t *) data->ptr) + u->offset);
    switch (u->type) {
      case YK__SG_UNIFORMTYPE_INVALID:
        break;
      case YK__SG_UNIFORMTYPE_FLOAT:
        glUniform1fv(u->gl_loc, u->count, ptr);
        break;
      case YK__SG_UNIFORMTYPE_FLOAT2:
        glUniform2fv(u->gl_loc, u->count, ptr);
        break;
      case YK__SG_UNIFORMTYPE_FLOAT3:
        glUniform3fv(u->gl_loc, u->count, ptr);
        break;
      case YK__SG_UNIFORMTYPE_FLOAT4:
        glUniform4fv(u->gl_loc, u->count, ptr);
        break;
      case YK__SG_UNIFORMTYPE_MAT4:
        glUniformMatrix4fv(u->gl_loc, u->count, GL_FALSE, ptr);
        break;
      default:
        YK__SOKOL_UNREACHABLE;
        break;
    }
  }
}
YK___SOKOL_PRIVATE void yk___sg_gl_draw(int base_element, int num_elements,
                                        int num_instances) {
  YK__SOKOL_ASSERT(yk___sg.gl.cache.cur_pipeline);
  const GLenum i_type = yk___sg.gl.cache.cur_index_type;
  const GLenum p_type = yk___sg.gl.cache.cur_primitive_type;
  if (0 != i_type) {
    /* indexed rendering */
    const int i_size = (i_type == GL_UNSIGNED_SHORT) ? 2 : 4;
    const int ib_offset = yk___sg.gl.cache.cur_ib_offset;
    const GLvoid *indices =
        (const GLvoid *) (GLintptr) (base_element * i_size + ib_offset);
    if (yk___sg.gl.cache.cur_pipeline->cmn.use_instanced_draw) {
      if (yk___sg.features.instancing) {
        glDrawElementsInstanced(p_type, num_elements, i_type, indices,
                                num_instances);
      }
    } else {
      glDrawElements(p_type, num_elements, i_type, indices);
    }
  } else {
    /* non-indexed rendering */
    if (yk___sg.gl.cache.cur_pipeline->cmn.use_instanced_draw) {
      if (yk___sg.features.instancing) {
        glDrawArraysInstanced(p_type, base_element, num_elements,
                              num_instances);
      }
    } else {
      glDrawArrays(p_type, base_element, num_elements);
    }
  }
}
YK___SOKOL_PRIVATE void yk___sg_gl_commit(void) {
  YK__SOKOL_ASSERT(!yk___sg.gl.in_pass);
  /* "soft" clear bindings (only those that are actually bound) */
  yk___sg_gl_cache_clear_buffer_bindings(false);
  yk___sg_gl_cache_clear_texture_bindings(false);
}
YK___SOKOL_PRIVATE void yk___sg_gl_update_buffer(yk___sg_buffer_t *buf,
                                                 const yk__sg_range *data) {
  YK__SOKOL_ASSERT(buf && data && data->ptr && (data->size > 0));
  /* only one update per buffer per frame allowed */
  if (++buf->cmn.active_slot >= buf->cmn.num_slots) {
    buf->cmn.active_slot = 0;
  }
  GLenum gl_tgt = yk___sg_gl_buffer_target(buf->cmn.type);
  YK__SOKOL_ASSERT(buf->cmn.active_slot < YK__SG_NUM_INFLIGHT_FRAMES);
  GLuint gl_buf = buf->gl.buf[buf->cmn.active_slot];
  YK__SOKOL_ASSERT(gl_buf);
  YK___SG_GL_CHECK_ERROR();
  yk___sg_gl_cache_store_buffer_binding(gl_tgt);
  yk___sg_gl_cache_bind_buffer(gl_tgt, gl_buf);
  glBufferSubData(gl_tgt, 0, (GLsizeiptr) data->size, data->ptr);
  yk___sg_gl_cache_restore_buffer_binding(gl_tgt);
  YK___SG_GL_CHECK_ERROR();
}
YK___SOKOL_PRIVATE int yk___sg_gl_append_buffer(yk___sg_buffer_t *buf,
                                                const yk__sg_range *data,
                                                bool new_frame) {
  YK__SOKOL_ASSERT(buf && data && data->ptr && (data->size > 0));
  if (new_frame) {
    if (++buf->cmn.active_slot >= buf->cmn.num_slots) {
      buf->cmn.active_slot = 0;
    }
  }
  GLenum gl_tgt = yk___sg_gl_buffer_target(buf->cmn.type);
  YK__SOKOL_ASSERT(buf->cmn.active_slot < YK__SG_NUM_INFLIGHT_FRAMES);
  GLuint gl_buf = buf->gl.buf[buf->cmn.active_slot];
  YK__SOKOL_ASSERT(gl_buf);
  YK___SG_GL_CHECK_ERROR();
  yk___sg_gl_cache_store_buffer_binding(gl_tgt);
  yk___sg_gl_cache_bind_buffer(gl_tgt, gl_buf);
  glBufferSubData(gl_tgt, buf->cmn.append_pos, (GLsizeiptr) data->size,
                  data->ptr);
  yk___sg_gl_cache_restore_buffer_binding(gl_tgt);
  YK___SG_GL_CHECK_ERROR();
  /* NOTE: this is a requirement from WebGPU, but we want identical behaviour across all backend */
  return yk___sg_roundup((int) data->size, 4);
}
YK___SOKOL_PRIVATE void yk___sg_gl_update_image(yk___sg_image_t *img,
                                                const yk__sg_image_data *data) {
  YK__SOKOL_ASSERT(img && data);
  /* only one update per image per frame allowed */
  if (++img->cmn.active_slot >= img->cmn.num_slots) {
    img->cmn.active_slot = 0;
  }
  YK__SOKOL_ASSERT(img->cmn.active_slot < YK__SG_NUM_INFLIGHT_FRAMES);
  YK__SOKOL_ASSERT(0 != img->gl.tex[img->cmn.active_slot]);
  yk___sg_gl_cache_store_texture_binding(0);
  yk___sg_gl_cache_bind_texture(0, img->gl.target,
                                img->gl.tex[img->cmn.active_slot]);
  const GLenum gl_img_format =
      yk___sg_gl_teximage_format(img->cmn.pixel_format);
  const GLenum gl_img_type = yk___sg_gl_teximage_type(img->cmn.pixel_format);
  const int num_faces = img->cmn.type == YK__SG_IMAGETYPE_CUBE ? 6 : 1;
  const int num_mips = img->cmn.num_mipmaps;
  for (int face_index = 0; face_index < num_faces; face_index++) {
    for (int mip_index = 0; mip_index < num_mips; mip_index++) {
      GLenum gl_img_target = img->gl.target;
      if (YK__SG_IMAGETYPE_CUBE == img->cmn.type) {
        gl_img_target = yk___sg_gl_cubeface_target(face_index);
      }
      const GLvoid *data_ptr = data->subimage[face_index][mip_index].ptr;
      int mip_width = img->cmn.width >> mip_index;
      if (mip_width == 0) { mip_width = 1; }
      int mip_height = img->cmn.height >> mip_index;
      if (mip_height == 0) { mip_height = 1; }
      if ((YK__SG_IMAGETYPE_2D == img->cmn.type) ||
          (YK__SG_IMAGETYPE_CUBE == img->cmn.type)) {
        glTexSubImage2D(gl_img_target, mip_index, 0, 0, mip_width, mip_height,
                        gl_img_format, gl_img_type, data_ptr);
      }
#if !defined(YK__SOKOL_GLES2)
      else if (!yk___sg.gl.gles2 &&
               ((YK__SG_IMAGETYPE_3D == img->cmn.type) ||
                (YK__SG_IMAGETYPE_ARRAY == img->cmn.type))) {
        int mip_depth = img->cmn.num_slices >> mip_index;
        if (mip_depth == 0) { mip_depth = 1; }
        glTexSubImage3D(gl_img_target, mip_index, 0, 0, 0, mip_width,
                        mip_height, mip_depth, gl_img_format, gl_img_type,
                        data_ptr);
      }
#endif
    }
  }
  yk___sg_gl_cache_restore_texture_binding(0);
}
/*== D3D11 BACKEND IMPLEMENTATION ============================================*/
#elif defined(YK__SOKOL_D3D11)
#if defined(__cplusplus)
#define yk___sg_d3d11_AddRef(self) (self)->AddRef()
#else
#define yk___sg_d3d11_AddRef(self) (self)->lpVtbl->AddRef(self)
#endif
#if defined(__cplusplus)
#define yk___sg_d3d11_Release(self) (self)->Release()
#else
#define yk___sg_d3d11_Release(self) (self)->lpVtbl->Release(self)
#endif
/*-- D3D11 C/C++ wrappers ----------------------------------------------------*/
static inline HRESULT yk___sg_d3d11_CheckFormatSupport(ID3D11Device *self,
                                                       DXGI_FORMAT Format,
                                                       UINT *pFormatSupport) {
#if defined(__cplusplus)
  return self->CheckFormatSupport(Format, pFormatSupport);
#else
  return self->lpVtbl->CheckFormatSupport(self, Format, pFormatSupport);
#endif
}
static inline void yk___sg_d3d11_OMSetRenderTargets(
    ID3D11DeviceContext *self, UINT NumViews,
    ID3D11RenderTargetView *const *ppRenderTargetViews,
    ID3D11DepthStencilView *pDepthStencilView) {
#if defined(__cplusplus)
  self->OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
#else
  self->lpVtbl->OMSetRenderTargets(self, NumViews, ppRenderTargetViews,
                                   pDepthStencilView);
#endif
}
static inline void
yk___sg_d3d11_RSSetState(ID3D11DeviceContext *self,
                         ID3D11RasterizerState *pRasterizerState) {
#if defined(__cplusplus)
  self->RSSetState(pRasterizerState);
#else
  self->lpVtbl->RSSetState(self, pRasterizerState);
#endif
}
static inline void yk___sg_d3d11_OMSetDepthStencilState(
    ID3D11DeviceContext *self, ID3D11DepthStencilState *pDepthStencilState,
    UINT StencilRef) {
#if defined(__cplusplus)
  self->OMSetDepthStencilState(pDepthStencilState, StencilRef);
#else
  self->lpVtbl->OMSetDepthStencilState(self, pDepthStencilState, StencilRef);
#endif
}
static inline void yk___sg_d3d11_OMSetBlendState(ID3D11DeviceContext *self,
                                                 ID3D11BlendState *pBlendState,
                                                 const FLOAT BlendFactor[4],
                                                 UINT SampleMask) {
#if defined(__cplusplus)
  self->OMSetBlendState(pBlendState, BlendFactor, SampleMask);
#else
  self->lpVtbl->OMSetBlendState(self, pBlendState, BlendFactor, SampleMask);
#endif
}
static inline void
yk___sg_d3d11_IASetVertexBuffers(ID3D11DeviceContext *self, UINT StartSlot,
                                 UINT NumBuffers,
                                 ID3D11Buffer *const *ppVertexBuffers,
                                 const UINT *pStrides, const UINT *pOffsets) {
#if defined(__cplusplus)
  self->IASetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides,
                           pOffsets);
#else
  self->lpVtbl->IASetVertexBuffers(self, StartSlot, NumBuffers, ppVertexBuffers,
                                   pStrides, pOffsets);
#endif
}
static inline void yk___sg_d3d11_IASetIndexBuffer(ID3D11DeviceContext *self,
                                                  ID3D11Buffer *pIndexBuffer,
                                                  DXGI_FORMAT Format,
                                                  UINT Offset) {
#if defined(__cplusplus)
  self->IASetIndexBuffer(pIndexBuffer, Format, Offset);
#else
  self->lpVtbl->IASetIndexBuffer(self, pIndexBuffer, Format, Offset);
#endif
}
static inline void
yk___sg_d3d11_IASetInputLayout(ID3D11DeviceContext *self,
                               ID3D11InputLayout *pInputLayout) {
#if defined(__cplusplus)
  self->IASetInputLayout(pInputLayout);
#else
  self->lpVtbl->IASetInputLayout(self, pInputLayout);
#endif
}
static inline void yk___sg_d3d11_VSSetShader(
    ID3D11DeviceContext *self, ID3D11VertexShader *pVertexShader,
    ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances) {
#if defined(__cplusplus)
  self->VSSetShader(pVertexShader, ppClassInstances, NumClassInstances);
#else
  self->lpVtbl->VSSetShader(self, pVertexShader, ppClassInstances,
                            NumClassInstances);
#endif
}
static inline void yk___sg_d3d11_PSSetShader(
    ID3D11DeviceContext *self, ID3D11PixelShader *pPixelShader,
    ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances) {
#if defined(__cplusplus)
  self->PSSetShader(pPixelShader, ppClassInstances, NumClassInstances);
#else
  self->lpVtbl->PSSetShader(self, pPixelShader, ppClassInstances,
                            NumClassInstances);
#endif
}
static inline void
yk___sg_d3d11_VSSetConstantBuffers(ID3D11DeviceContext *self, UINT StartSlot,
                                   UINT NumBuffers,
                                   ID3D11Buffer *const *ppConstantBuffers) {
#if defined(__cplusplus)
  self->VSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
#else
  self->lpVtbl->VSSetConstantBuffers(self, StartSlot, NumBuffers,
                                     ppConstantBuffers);
#endif
}
static inline void
yk___sg_d3d11_PSSetConstantBuffers(ID3D11DeviceContext *self, UINT StartSlot,
                                   UINT NumBuffers,
                                   ID3D11Buffer *const *ppConstantBuffers) {
#if defined(__cplusplus)
  self->PSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
#else
  self->lpVtbl->PSSetConstantBuffers(self, StartSlot, NumBuffers,
                                     ppConstantBuffers);
#endif
}
static inline void yk___sg_d3d11_VSSetShaderResources(
    ID3D11DeviceContext *self, UINT StartSlot, UINT NumViews,
    ID3D11ShaderResourceView *const *ppShaderResourceViews) {
#if defined(__cplusplus)
  self->VSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
#else
  self->lpVtbl->VSSetShaderResources(self, StartSlot, NumViews,
                                     ppShaderResourceViews);
#endif
}
static inline void yk___sg_d3d11_PSSetShaderResources(
    ID3D11DeviceContext *self, UINT StartSlot, UINT NumViews,
    ID3D11ShaderResourceView *const *ppShaderResourceViews) {
#if defined(__cplusplus)
  self->PSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
#else
  self->lpVtbl->PSSetShaderResources(self, StartSlot, NumViews,
                                     ppShaderResourceViews);
#endif
}
static inline void
yk___sg_d3d11_VSSetSamplers(ID3D11DeviceContext *self, UINT StartSlot,
                            UINT NumSamplers,
                            ID3D11SamplerState *const *ppSamplers) {
#if defined(__cplusplus)
  self->VSSetSamplers(StartSlot, NumSamplers, ppSamplers);
#else
  self->lpVtbl->VSSetSamplers(self, StartSlot, NumSamplers, ppSamplers);
#endif
}
static inline void
yk___sg_d3d11_PSSetSamplers(ID3D11DeviceContext *self, UINT StartSlot,
                            UINT NumSamplers,
                            ID3D11SamplerState *const *ppSamplers) {
#if defined(__cplusplus)
  self->PSSetSamplers(StartSlot, NumSamplers, ppSamplers);
#else
  self->lpVtbl->PSSetSamplers(self, StartSlot, NumSamplers, ppSamplers);
#endif
}
static inline HRESULT
yk___sg_d3d11_CreateBuffer(ID3D11Device *self, const D3D11_BUFFER_DESC *pDesc,
                           const D3D11_SUBRESOURCE_DATA *pInitialData,
                           ID3D11Buffer **ppBuffer) {
#if defined(__cplusplus)
  return self->CreateBuffer(pDesc, pInitialData, ppBuffer);
#else
  return self->lpVtbl->CreateBuffer(self, pDesc, pInitialData, ppBuffer);
#endif
}
static inline HRESULT yk___sg_d3d11_CreateTexture2D(
    ID3D11Device *self, const D3D11_TEXTURE2D_DESC *pDesc,
    const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Texture2D **ppTexture2D) {
#if defined(__cplusplus)
  return self->CreateTexture2D(pDesc, pInitialData, ppTexture2D);
#else
  return self->lpVtbl->CreateTexture2D(self, pDesc, pInitialData, ppTexture2D);
#endif
}
static inline HRESULT yk___sg_d3d11_CreateShaderResourceView(
    ID3D11Device *self, ID3D11Resource *pResource,
    const D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc,
    ID3D11ShaderResourceView **ppSRView) {
#if defined(__cplusplus)
  return self->CreateShaderResourceView(pResource, pDesc, ppSRView);
#else
  return self->lpVtbl->CreateShaderResourceView(self, pResource, pDesc,
                                                ppSRView);
#endif
}
static inline void yk___sg_d3d11_GetResource(ID3D11View *self,
                                             ID3D11Resource **ppResource) {
#if defined(__cplusplus)
  self->GetResource(ppResource);
#else
  self->lpVtbl->GetResource(self, ppResource);
#endif
}
static inline HRESULT yk___sg_d3d11_CreateTexture3D(
    ID3D11Device *self, const D3D11_TEXTURE3D_DESC *pDesc,
    const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Texture3D **ppTexture3D) {
#if defined(__cplusplus)
  return self->CreateTexture3D(pDesc, pInitialData, ppTexture3D);
#else
  return self->lpVtbl->CreateTexture3D(self, pDesc, pInitialData, ppTexture3D);
#endif
}
static inline HRESULT
yk___sg_d3d11_CreateSamplerState(ID3D11Device *self,
                                 const D3D11_SAMPLER_DESC *pSamplerDesc,
                                 ID3D11SamplerState **ppSamplerState) {
#if defined(__cplusplus)
  return self->CreateSamplerState(pSamplerDesc, ppSamplerState);
#else
  return self->lpVtbl->CreateSamplerState(self, pSamplerDesc, ppSamplerState);
#endif
}
static inline LPVOID yk___sg_d3d11_GetBufferPointer(ID3D10Blob *self) {
#if defined(__cplusplus)
  return self->GetBufferPointer();
#else
  return self->lpVtbl->GetBufferPointer(self);
#endif
}
static inline SIZE_T yk___sg_d3d11_GetBufferSize(ID3D10Blob *self) {
#if defined(__cplusplus)
  return self->GetBufferSize();
#else
  return self->lpVtbl->GetBufferSize(self);
#endif
}
static inline HRESULT yk___sg_d3d11_CreateVertexShader(
    ID3D11Device *self, const void *pShaderBytecode, SIZE_T BytecodeLength,
    ID3D11ClassLinkage *pClassLinkage, ID3D11VertexShader **ppVertexShader) {
#if defined(__cplusplus)
  return self->CreateVertexShader(pShaderBytecode, BytecodeLength,
                                  pClassLinkage, ppVertexShader);
#else
  return self->lpVtbl->CreateVertexShader(self, pShaderBytecode, BytecodeLength,
                                          pClassLinkage, ppVertexShader);
#endif
}
static inline HRESULT yk___sg_d3d11_CreatePixelShader(
    ID3D11Device *self, const void *pShaderBytecode, SIZE_T BytecodeLength,
    ID3D11ClassLinkage *pClassLinkage, ID3D11PixelShader **ppPixelShader) {
#if defined(__cplusplus)
  return self->CreatePixelShader(pShaderBytecode, BytecodeLength, pClassLinkage,
                                 ppPixelShader);
#else
  return self->lpVtbl->CreatePixelShader(self, pShaderBytecode, BytecodeLength,
                                         pClassLinkage, ppPixelShader);
#endif
}
static inline HRESULT yk___sg_d3d11_CreateInputLayout(
    ID3D11Device *self, const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,
    UINT NumElements, const void *pShaderBytecodeWithInputSignature,
    SIZE_T BytecodeLength, ID3D11InputLayout **ppInputLayout) {
#if defined(__cplusplus)
  return self->CreateInputLayout(pInputElementDescs, NumElements,
                                 pShaderBytecodeWithInputSignature,
                                 BytecodeLength, ppInputLayout);
#else
  return self->lpVtbl->CreateInputLayout(self, pInputElementDescs, NumElements,
                                         pShaderBytecodeWithInputSignature,
                                         BytecodeLength, ppInputLayout);
#endif
}
static inline HRESULT yk___sg_d3d11_CreateRasterizerState(
    ID3D11Device *self, const D3D11_RASTERIZER_DESC *pRasterizerDesc,
    ID3D11RasterizerState **ppRasterizerState) {
#if defined(__cplusplus)
  return self->CreateRasterizerState(pRasterizerDesc, ppRasterizerState);
#else
  return self->lpVtbl->CreateRasterizerState(self, pRasterizerDesc,
                                             ppRasterizerState);
#endif
}
static inline HRESULT yk___sg_d3d11_CreateDepthStencilState(
    ID3D11Device *self, const D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc,
    ID3D11DepthStencilState **ppDepthStencilState) {
#if defined(__cplusplus)
  return self->CreateDepthStencilState(pDepthStencilDesc, ppDepthStencilState);
#else
  return self->lpVtbl->CreateDepthStencilState(self, pDepthStencilDesc,
                                               ppDepthStencilState);
#endif
}
static inline HRESULT
yk___sg_d3d11_CreateBlendState(ID3D11Device *self,
                               const D3D11_BLEND_DESC *pBlendStateDesc,
                               ID3D11BlendState **ppBlendState) {
#if defined(__cplusplus)
  return self->CreateBlendState(pBlendStateDesc, ppBlendState);
#else
  return self->lpVtbl->CreateBlendState(self, pBlendStateDesc, ppBlendState);
#endif
}
static inline HRESULT
yk___sg_d3d11_CreateRenderTargetView(ID3D11Device *self,
                                     ID3D11Resource *pResource,
                                     const D3D11_RENDER_TARGET_VIEW_DESC *pDesc,
                                     ID3D11RenderTargetView **ppRTView) {
#if defined(__cplusplus)
  return self->CreateRenderTargetView(pResource, pDesc, ppRTView);
#else
  return self->lpVtbl->CreateRenderTargetView(self, pResource, pDesc, ppRTView);
#endif
}
static inline HRESULT yk___sg_d3d11_CreateDepthStencilView(
    ID3D11Device *self, ID3D11Resource *pResource,
    const D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc,
    ID3D11DepthStencilView **ppDepthStencilView) {
#if defined(__cplusplus)
  return self->CreateDepthStencilView(pResource, pDesc, ppDepthStencilView);
#else
  return self->lpVtbl->CreateDepthStencilView(self, pResource, pDesc,
                                              ppDepthStencilView);
#endif
}
static inline void
yk___sg_d3d11_RSSetViewports(ID3D11DeviceContext *self, UINT NumViewports,
                             const D3D11_VIEWPORT *pViewports) {
#if defined(__cplusplus)
  self->RSSetViewports(NumViewports, pViewports);
#else
  self->lpVtbl->RSSetViewports(self, NumViewports, pViewports);
#endif
}
static inline void yk___sg_d3d11_RSSetScissorRects(ID3D11DeviceContext *self,
                                                   UINT NumRects,
                                                   const D3D11_RECT *pRects) {
#if defined(__cplusplus)
  self->RSSetScissorRects(NumRects, pRects);
#else
  self->lpVtbl->RSSetScissorRects(self, NumRects, pRects);
#endif
}
static inline void
yk___sg_d3d11_ClearRenderTargetView(ID3D11DeviceContext *self,
                                    ID3D11RenderTargetView *pRenderTargetView,
                                    const FLOAT ColorRGBA[4]) {
#if defined(__cplusplus)
  self->ClearRenderTargetView(pRenderTargetView, ColorRGBA);
#else
  self->lpVtbl->ClearRenderTargetView(self, pRenderTargetView, ColorRGBA);
#endif
}
static inline void yk___sg_d3d11_ClearDepthStencilView(
    ID3D11DeviceContext *self, ID3D11DepthStencilView *pDepthStencilView,
    UINT ClearFlags, FLOAT Depth, UINT8 Stencil) {
#if defined(__cplusplus)
  self->ClearDepthStencilView(pDepthStencilView, ClearFlags, Depth, Stencil);
#else
  self->lpVtbl->ClearDepthStencilView(self, pDepthStencilView, ClearFlags,
                                      Depth, Stencil);
#endif
}
static inline void yk___sg_d3d11_ResolveSubresource(
    ID3D11DeviceContext *self, ID3D11Resource *pDstResource,
    UINT DstSubresource, ID3D11Resource *pSrcResource, UINT SrcSubresource,
    DXGI_FORMAT Format) {
#if defined(__cplusplus)
  self->ResolveSubresource(pDstResource, DstSubresource, pSrcResource,
                           SrcSubresource, Format);
#else
  self->lpVtbl->ResolveSubresource(self, pDstResource, DstSubresource,
                                   pSrcResource, SrcSubresource, Format);
#endif
}
static inline void
yk___sg_d3d11_IASetPrimitiveTopology(ID3D11DeviceContext *self,
                                     D3D11_PRIMITIVE_TOPOLOGY Topology) {
#if defined(__cplusplus)
  self->IASetPrimitiveTopology(Topology);
#else
  self->lpVtbl->IASetPrimitiveTopology(self, Topology);
#endif
}
static inline void yk___sg_d3d11_UpdateSubresource(
    ID3D11DeviceContext *self, ID3D11Resource *pDstResource,
    UINT DstSubresource, const D3D11_BOX *pDstBox, const void *pSrcData,
    UINT SrcRowPitch, UINT SrcDepthPitch) {
#if defined(__cplusplus)
  self->UpdateSubresource(pDstResource, DstSubresource, pDstBox, pSrcData,
                          SrcRowPitch, SrcDepthPitch);
#else
  self->lpVtbl->UpdateSubresource(self, pDstResource, DstSubresource, pDstBox,
                                  pSrcData, SrcRowPitch, SrcDepthPitch);
#endif
}
static inline void yk___sg_d3d11_DrawIndexed(ID3D11DeviceContext *self,
                                             UINT IndexCount,
                                             UINT StartIndexLocation,
                                             INT BaseVertexLocation) {
#if defined(__cplusplus)
  self->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
#else
  self->lpVtbl->DrawIndexed(self, IndexCount, StartIndexLocation,
                            BaseVertexLocation);
#endif
}
static inline void yk___sg_d3d11_DrawIndexedInstanced(
    ID3D11DeviceContext *self, UINT IndexCountPerInstance, UINT InstanceCount,
    UINT StartIndexLocation, INT BaseVertexLocation,
    UINT StartInstanceLocation) {
#if defined(__cplusplus)
  self->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount,
                             StartIndexLocation, BaseVertexLocation,
                             StartInstanceLocation);
#else
  self->lpVtbl->DrawIndexedInstanced(self, IndexCountPerInstance, InstanceCount,
                                     StartIndexLocation, BaseVertexLocation,
                                     StartInstanceLocation);
#endif
}
static inline void yk___sg_d3d11_Draw(ID3D11DeviceContext *self,
                                      UINT VertexCount,
                                      UINT StartVertexLocation) {
#if defined(__cplusplus)
  self->Draw(VertexCount, StartVertexLocation);
#else
  self->lpVtbl->Draw(self, VertexCount, StartVertexLocation);
#endif
}
static inline void yk___sg_d3d11_DrawInstanced(ID3D11DeviceContext *self,
                                               UINT VertexCountPerInstance,
                                               UINT InstanceCount,
                                               UINT StartVertexLocation,
                                               UINT StartInstanceLocation) {
#if defined(__cplusplus)
  self->DrawInstanced(VertexCountPerInstance, InstanceCount,
                      StartVertexLocation, StartInstanceLocation);
#else
  self->lpVtbl->DrawInstanced(self, VertexCountPerInstance, InstanceCount,
                              StartVertexLocation, StartInstanceLocation);
#endif
}
static inline HRESULT
yk___sg_d3d11_Map(ID3D11DeviceContext *self, ID3D11Resource *pResource,
                  UINT Subresource, D3D11_MAP MapType, UINT MapFlags,
                  D3D11_MAPPED_SUBRESOURCE *pMappedResource) {
#if defined(__cplusplus)
  return self->Map(pResource, Subresource, MapType, MapFlags, pMappedResource);
#else
  return self->lpVtbl->Map(self, pResource, Subresource, MapType, MapFlags,
                           pMappedResource);
#endif
}
static inline void yk___sg_d3d11_Unmap(ID3D11DeviceContext *self,
                                       ID3D11Resource *pResource,
                                       UINT Subresource) {
#if defined(__cplusplus)
  self->Unmap(pResource, Subresource);
#else
  self->lpVtbl->Unmap(self, pResource, Subresource);
#endif
}
static inline void yk___sg_d3d11_ClearState(ID3D11DeviceContext *self) {
#if defined(__cplusplus)
  self->ClearState();
#else
  self->lpVtbl->ClearState(self);
#endif
}
/*-- enum translation functions ----------------------------------------------*/
YK___SOKOL_PRIVATE D3D11_USAGE yk___sg_d3d11_usage(yk__sg_usage yk__usg) {
  switch (yk__usg) {
    case YK__SG_USAGE_IMMUTABLE:
      return D3D11_USAGE_IMMUTABLE;
    case YK__SG_USAGE_DYNAMIC:
    case YK__SG_USAGE_STREAM:
      return D3D11_USAGE_DYNAMIC;
    default:
      YK__SOKOL_UNREACHABLE;
      return (D3D11_USAGE) 0;
  }
}
YK___SOKOL_PRIVATE UINT yk___sg_d3d11_cpu_access_flags(yk__sg_usage yk__usg) {
  switch (yk__usg) {
    case YK__SG_USAGE_IMMUTABLE:
      return 0;
    case YK__SG_USAGE_DYNAMIC:
    case YK__SG_USAGE_STREAM:
      return D3D11_CPU_ACCESS_WRITE;
    default:
      YK__SOKOL_UNREACHABLE;
      return 0;
  }
}
YK___SOKOL_PRIVATE DXGI_FORMAT
yk___sg_d3d11_pixel_format(yk__sg_pixel_format fmt) {
  switch (fmt) {
    case YK__SG_PIXELFORMAT_R8:
      return DXGI_FORMAT_R8_UNORM;
    case YK__SG_PIXELFORMAT_R8SN:
      return DXGI_FORMAT_R8_SNORM;
    case YK__SG_PIXELFORMAT_R8UI:
      return DXGI_FORMAT_R8_UINT;
    case YK__SG_PIXELFORMAT_R8SI:
      return DXGI_FORMAT_R8_SINT;
    case YK__SG_PIXELFORMAT_R16:
      return DXGI_FORMAT_R16_UNORM;
    case YK__SG_PIXELFORMAT_R16SN:
      return DXGI_FORMAT_R16_SNORM;
    case YK__SG_PIXELFORMAT_R16UI:
      return DXGI_FORMAT_R16_UINT;
    case YK__SG_PIXELFORMAT_R16SI:
      return DXGI_FORMAT_R16_SINT;
    case YK__SG_PIXELFORMAT_R16F:
      return DXGI_FORMAT_R16_FLOAT;
    case YK__SG_PIXELFORMAT_RG8:
      return DXGI_FORMAT_R8G8_UNORM;
    case YK__SG_PIXELFORMAT_RG8SN:
      return DXGI_FORMAT_R8G8_SNORM;
    case YK__SG_PIXELFORMAT_RG8UI:
      return DXGI_FORMAT_R8G8_UINT;
    case YK__SG_PIXELFORMAT_RG8SI:
      return DXGI_FORMAT_R8G8_SINT;
    case YK__SG_PIXELFORMAT_R32UI:
      return DXGI_FORMAT_R32_UINT;
    case YK__SG_PIXELFORMAT_R32SI:
      return DXGI_FORMAT_R32_SINT;
    case YK__SG_PIXELFORMAT_R32F:
      return DXGI_FORMAT_R32_FLOAT;
    case YK__SG_PIXELFORMAT_RG16:
      return DXGI_FORMAT_R16G16_UNORM;
    case YK__SG_PIXELFORMAT_RG16SN:
      return DXGI_FORMAT_R16G16_SNORM;
    case YK__SG_PIXELFORMAT_RG16UI:
      return DXGI_FORMAT_R16G16_UINT;
    case YK__SG_PIXELFORMAT_RG16SI:
      return DXGI_FORMAT_R16G16_SINT;
    case YK__SG_PIXELFORMAT_RG16F:
      return DXGI_FORMAT_R16G16_FLOAT;
    case YK__SG_PIXELFORMAT_RGBA8:
      return DXGI_FORMAT_R8G8B8A8_UNORM;
    case YK__SG_PIXELFORMAT_RGBA8SN:
      return DXGI_FORMAT_R8G8B8A8_SNORM;
    case YK__SG_PIXELFORMAT_RGBA8UI:
      return DXGI_FORMAT_R8G8B8A8_UINT;
    case YK__SG_PIXELFORMAT_RGBA8SI:
      return DXGI_FORMAT_R8G8B8A8_SINT;
    case YK__SG_PIXELFORMAT_BGRA8:
      return DXGI_FORMAT_B8G8R8A8_UNORM;
    case YK__SG_PIXELFORMAT_RGB10A2:
      return DXGI_FORMAT_R10G10B10A2_UNORM;
    case YK__SG_PIXELFORMAT_RG11B10F:
      return DXGI_FORMAT_R11G11B10_FLOAT;
    case YK__SG_PIXELFORMAT_RG32UI:
      return DXGI_FORMAT_R32G32_UINT;
    case YK__SG_PIXELFORMAT_RG32SI:
      return DXGI_FORMAT_R32G32_SINT;
    case YK__SG_PIXELFORMAT_RG32F:
      return DXGI_FORMAT_R32G32_FLOAT;
    case YK__SG_PIXELFORMAT_RGBA16:
      return DXGI_FORMAT_R16G16B16A16_UNORM;
    case YK__SG_PIXELFORMAT_RGBA16SN:
      return DXGI_FORMAT_R16G16B16A16_SNORM;
    case YK__SG_PIXELFORMAT_RGBA16UI:
      return DXGI_FORMAT_R16G16B16A16_UINT;
    case YK__SG_PIXELFORMAT_RGBA16SI:
      return DXGI_FORMAT_R16G16B16A16_SINT;
    case YK__SG_PIXELFORMAT_RGBA16F:
      return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case YK__SG_PIXELFORMAT_RGBA32UI:
      return DXGI_FORMAT_R32G32B32A32_UINT;
    case YK__SG_PIXELFORMAT_RGBA32SI:
      return DXGI_FORMAT_R32G32B32A32_SINT;
    case YK__SG_PIXELFORMAT_RGBA32F:
      return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case YK__SG_PIXELFORMAT_DEPTH:
      return DXGI_FORMAT_D32_FLOAT;
    case YK__SG_PIXELFORMAT_DEPTH_STENCIL:
      return DXGI_FORMAT_D24_UNORM_S8_UINT;
    case YK__SG_PIXELFORMAT_BC1_RGBA:
      return DXGI_FORMAT_BC1_UNORM;
    case YK__SG_PIXELFORMAT_BC2_RGBA:
      return DXGI_FORMAT_BC2_UNORM;
    case YK__SG_PIXELFORMAT_BC3_RGBA:
      return DXGI_FORMAT_BC3_UNORM;
    case YK__SG_PIXELFORMAT_BC4_R:
      return DXGI_FORMAT_BC4_UNORM;
    case YK__SG_PIXELFORMAT_BC4_RSN:
      return DXGI_FORMAT_BC4_SNORM;
    case YK__SG_PIXELFORMAT_BC5_RG:
      return DXGI_FORMAT_BC5_UNORM;
    case YK__SG_PIXELFORMAT_BC5_RGSN:
      return DXGI_FORMAT_BC5_SNORM;
    case YK__SG_PIXELFORMAT_BC6H_RGBF:
      return DXGI_FORMAT_BC6H_SF16;
    case YK__SG_PIXELFORMAT_BC6H_RGBUF:
      return DXGI_FORMAT_BC6H_UF16;
    case YK__SG_PIXELFORMAT_BC7_RGBA:
      return DXGI_FORMAT_BC7_UNORM;
    default:
      return DXGI_FORMAT_UNKNOWN;
  };
}
YK___SOKOL_PRIVATE D3D11_PRIMITIVE_TOPOLOGY
yk___sg_d3d11_primitive_topology(yk__sg_primitive_type prim_type) {
  switch (prim_type) {
    case YK__SG_PRIMITIVETYPE_POINTS:
      return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
    case YK__SG_PRIMITIVETYPE_LINES:
      return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    case YK__SG_PRIMITIVETYPE_LINE_STRIP:
      return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
    case YK__SG_PRIMITIVETYPE_TRIANGLES:
      return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    case YK__SG_PRIMITIVETYPE_TRIANGLE_STRIP:
      return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    default:
      YK__SOKOL_UNREACHABLE;
      return (D3D11_PRIMITIVE_TOPOLOGY) 0;
  }
}
YK___SOKOL_PRIVATE DXGI_FORMAT
yk___sg_d3d11_index_format(yk__sg_index_type index_type) {
  switch (index_type) {
    case YK__SG_INDEXTYPE_NONE:
      return DXGI_FORMAT_UNKNOWN;
    case YK__SG_INDEXTYPE_UINT16:
      return DXGI_FORMAT_R16_UINT;
    case YK__SG_INDEXTYPE_UINT32:
      return DXGI_FORMAT_R32_UINT;
    default:
      YK__SOKOL_UNREACHABLE;
      return (DXGI_FORMAT) 0;
  }
}
YK___SOKOL_PRIVATE D3D11_FILTER yk___sg_d3d11_filter(yk__sg_filter min_f,
                                                     yk__sg_filter mag_f,
                                                     uint32_t max_anisotropy) {
  if (max_anisotropy > 1) {
    return D3D11_FILTER_ANISOTROPIC;
  } else if (mag_f == YK__SG_FILTER_NEAREST) {
    switch (min_f) {
      case YK__SG_FILTER_NEAREST:
      case YK__SG_FILTER_NEAREST_MIPMAP_NEAREST:
        return D3D11_FILTER_MIN_MAG_MIP_POINT;
      case YK__SG_FILTER_LINEAR:
      case YK__SG_FILTER_LINEAR_MIPMAP_NEAREST:
        return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
      case YK__SG_FILTER_NEAREST_MIPMAP_LINEAR:
        return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
      case YK__SG_FILTER_LINEAR_MIPMAP_LINEAR:
        return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
      default:
        YK__SOKOL_UNREACHABLE;
        break;
    }
  } else if (mag_f == YK__SG_FILTER_LINEAR) {
    switch (min_f) {
      case YK__SG_FILTER_NEAREST:
      case YK__SG_FILTER_NEAREST_MIPMAP_NEAREST:
        return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
      case YK__SG_FILTER_LINEAR:
      case YK__SG_FILTER_LINEAR_MIPMAP_NEAREST:
        return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
      case YK__SG_FILTER_NEAREST_MIPMAP_LINEAR:
        return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
      case YK__SG_FILTER_LINEAR_MIPMAP_LINEAR:
        return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
      default:
        YK__SOKOL_UNREACHABLE;
        break;
    }
  }
  /* invalid value for mag filter */
  YK__SOKOL_UNREACHABLE;
  return D3D11_FILTER_MIN_MAG_MIP_POINT;
}
YK___SOKOL_PRIVATE D3D11_TEXTURE_ADDRESS_MODE
yk___sg_d3d11_address_mode(yk__sg_wrap m) {
  switch (m) {
    case YK__SG_WRAP_REPEAT:
      return D3D11_TEXTURE_ADDRESS_WRAP;
    case YK__SG_WRAP_CLAMP_TO_EDGE:
      return D3D11_TEXTURE_ADDRESS_CLAMP;
    case YK__SG_WRAP_CLAMP_TO_BORDER:
      return D3D11_TEXTURE_ADDRESS_BORDER;
    case YK__SG_WRAP_MIRRORED_REPEAT:
      return D3D11_TEXTURE_ADDRESS_MIRROR;
    default:
      YK__SOKOL_UNREACHABLE;
      return (D3D11_TEXTURE_ADDRESS_MODE) 0;
  }
}
YK___SOKOL_PRIVATE DXGI_FORMAT
yk___sg_d3d11_vertex_format(yk__sg_vertex_format fmt) {
  switch (fmt) {
    case YK__SG_VERTEXFORMAT_FLOAT:
      return DXGI_FORMAT_R32_FLOAT;
    case YK__SG_VERTEXFORMAT_FLOAT2:
      return DXGI_FORMAT_R32G32_FLOAT;
    case YK__SG_VERTEXFORMAT_FLOAT3:
      return DXGI_FORMAT_R32G32B32_FLOAT;
    case YK__SG_VERTEXFORMAT_FLOAT4:
      return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case YK__SG_VERTEXFORMAT_BYTE4:
      return DXGI_FORMAT_R8G8B8A8_SINT;
    case YK__SG_VERTEXFORMAT_BYTE4N:
      return DXGI_FORMAT_R8G8B8A8_SNORM;
    case YK__SG_VERTEXFORMAT_UBYTE4:
      return DXGI_FORMAT_R8G8B8A8_UINT;
    case YK__SG_VERTEXFORMAT_UBYTE4N:
      return DXGI_FORMAT_R8G8B8A8_UNORM;
    case YK__SG_VERTEXFORMAT_SHORT2:
      return DXGI_FORMAT_R16G16_SINT;
    case YK__SG_VERTEXFORMAT_SHORT2N:
      return DXGI_FORMAT_R16G16_SNORM;
    case YK__SG_VERTEXFORMAT_USHORT2N:
      return DXGI_FORMAT_R16G16_UNORM;
    case YK__SG_VERTEXFORMAT_SHORT4:
      return DXGI_FORMAT_R16G16B16A16_SINT;
    case YK__SG_VERTEXFORMAT_SHORT4N:
      return DXGI_FORMAT_R16G16B16A16_SNORM;
    case YK__SG_VERTEXFORMAT_USHORT4N:
      return DXGI_FORMAT_R16G16B16A16_UNORM;
    case YK__SG_VERTEXFORMAT_UINT10_N2:
      return DXGI_FORMAT_R10G10B10A2_UNORM;
    default:
      YK__SOKOL_UNREACHABLE;
      return (DXGI_FORMAT) 0;
  }
}
YK___SOKOL_PRIVATE D3D11_INPUT_CLASSIFICATION
yk___sg_d3d11_input_classification(yk__sg_vertex_step step) {
  switch (step) {
    case YK__SG_VERTEXSTEP_PER_VERTEX:
      return D3D11_INPUT_PER_VERTEX_DATA;
    case YK__SG_VERTEXSTEP_PER_INSTANCE:
      return D3D11_INPUT_PER_INSTANCE_DATA;
    default:
      YK__SOKOL_UNREACHABLE;
      return (D3D11_INPUT_CLASSIFICATION) 0;
  }
}
YK___SOKOL_PRIVATE D3D11_CULL_MODE yk___sg_d3d11_cull_mode(yk__sg_cull_mode m) {
  switch (m) {
    case YK__SG_CULLMODE_NONE:
      return D3D11_CULL_NONE;
    case YK__SG_CULLMODE_FRONT:
      return D3D11_CULL_FRONT;
    case YK__SG_CULLMODE_BACK:
      return D3D11_CULL_BACK;
    default:
      YK__SOKOL_UNREACHABLE;
      return (D3D11_CULL_MODE) 0;
  }
}
YK___SOKOL_PRIVATE D3D11_COMPARISON_FUNC
yk___sg_d3d11_compare_func(yk__sg_compare_func f) {
  switch (f) {
    case YK__SG_COMPAREFUNC_NEVER:
      return D3D11_COMPARISON_NEVER;
    case YK__SG_COMPAREFUNC_LESS:
      return D3D11_COMPARISON_LESS;
    case YK__SG_COMPAREFUNC_EQUAL:
      return D3D11_COMPARISON_EQUAL;
    case YK__SG_COMPAREFUNC_LESS_EQUAL:
      return D3D11_COMPARISON_LESS_EQUAL;
    case YK__SG_COMPAREFUNC_GREATER:
      return D3D11_COMPARISON_GREATER;
    case YK__SG_COMPAREFUNC_NOT_EQUAL:
      return D3D11_COMPARISON_NOT_EQUAL;
    case YK__SG_COMPAREFUNC_GREATER_EQUAL:
      return D3D11_COMPARISON_GREATER_EQUAL;
    case YK__SG_COMPAREFUNC_ALWAYS:
      return D3D11_COMPARISON_ALWAYS;
    default:
      YK__SOKOL_UNREACHABLE;
      return (D3D11_COMPARISON_FUNC) 0;
  }
}
YK___SOKOL_PRIVATE D3D11_STENCIL_OP
yk___sg_d3d11_stencil_op(yk__sg_stencil_op op) {
  switch (op) {
    case YK__SG_STENCILOP_KEEP:
      return D3D11_STENCIL_OP_KEEP;
    case YK__SG_STENCILOP_ZERO:
      return D3D11_STENCIL_OP_ZERO;
    case YK__SG_STENCILOP_REPLACE:
      return D3D11_STENCIL_OP_REPLACE;
    case YK__SG_STENCILOP_INCR_CLAMP:
      return D3D11_STENCIL_OP_INCR_SAT;
    case YK__SG_STENCILOP_DECR_CLAMP:
      return D3D11_STENCIL_OP_DECR_SAT;
    case YK__SG_STENCILOP_INVERT:
      return D3D11_STENCIL_OP_INVERT;
    case YK__SG_STENCILOP_INCR_WRAP:
      return D3D11_STENCIL_OP_INCR;
    case YK__SG_STENCILOP_DECR_WRAP:
      return D3D11_STENCIL_OP_DECR;
    default:
      YK__SOKOL_UNREACHABLE;
      return (D3D11_STENCIL_OP) 0;
  }
}
YK___SOKOL_PRIVATE D3D11_BLEND
yk___sg_d3d11_blend_factor(yk__sg_blend_factor f) {
  switch (f) {
    case YK__SG_BLENDFACTOR_ZERO:
      return D3D11_BLEND_ZERO;
    case YK__SG_BLENDFACTOR_ONE:
      return D3D11_BLEND_ONE;
    case YK__SG_BLENDFACTOR_SRC_COLOR:
      return D3D11_BLEND_SRC_COLOR;
    case YK__SG_BLENDFACTOR_ONE_MINUS_SRC_COLOR:
      return D3D11_BLEND_INV_SRC_COLOR;
    case YK__SG_BLENDFACTOR_SRC_ALPHA:
      return D3D11_BLEND_SRC_ALPHA;
    case YK__SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA:
      return D3D11_BLEND_INV_SRC_ALPHA;
    case YK__SG_BLENDFACTOR_DST_COLOR:
      return D3D11_BLEND_DEST_COLOR;
    case YK__SG_BLENDFACTOR_ONE_MINUS_DST_COLOR:
      return D3D11_BLEND_INV_DEST_COLOR;
    case YK__SG_BLENDFACTOR_DST_ALPHA:
      return D3D11_BLEND_DEST_ALPHA;
    case YK__SG_BLENDFACTOR_ONE_MINUS_DST_ALPHA:
      return D3D11_BLEND_INV_DEST_ALPHA;
    case YK__SG_BLENDFACTOR_SRC_ALPHA_SATURATED:
      return D3D11_BLEND_SRC_ALPHA_SAT;
    case YK__SG_BLENDFACTOR_BLEND_COLOR:
      return D3D11_BLEND_BLEND_FACTOR;
    case YK__SG_BLENDFACTOR_ONE_MINUS_BLEND_COLOR:
      return D3D11_BLEND_INV_BLEND_FACTOR;
    case YK__SG_BLENDFACTOR_BLEND_ALPHA:
      return D3D11_BLEND_BLEND_FACTOR;
    case YK__SG_BLENDFACTOR_ONE_MINUS_BLEND_ALPHA:
      return D3D11_BLEND_INV_BLEND_FACTOR;
    default:
      YK__SOKOL_UNREACHABLE;
      return (D3D11_BLEND) 0;
  }
}
YK___SOKOL_PRIVATE D3D11_BLEND_OP yk___sg_d3d11_blend_op(yk__sg_blend_op op) {
  switch (op) {
    case YK__SG_BLENDOP_ADD:
      return D3D11_BLEND_OP_ADD;
    case YK__SG_BLENDOP_SUBTRACT:
      return D3D11_BLEND_OP_SUBTRACT;
    case YK__SG_BLENDOP_REVERSE_SUBTRACT:
      return D3D11_BLEND_OP_REV_SUBTRACT;
    default:
      YK__SOKOL_UNREACHABLE;
      return (D3D11_BLEND_OP) 0;
  }
}
YK___SOKOL_PRIVATE UINT8 yk___sg_d3d11_color_write_mask(yk__sg_color_mask m) {
  UINT8 res = 0;
  if (m & YK__SG_COLORMASK_R) { res |= D3D11_COLOR_WRITE_ENABLE_RED; }
  if (m & YK__SG_COLORMASK_G) { res |= D3D11_COLOR_WRITE_ENABLE_GREEN; }
  if (m & YK__SG_COLORMASK_B) { res |= D3D11_COLOR_WRITE_ENABLE_BLUE; }
  if (m & YK__SG_COLORMASK_A) { res |= D3D11_COLOR_WRITE_ENABLE_ALPHA; }
  return res;
}
/* see: https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-limits#resource-limits-for-feature-level-11-hardware */
YK___SOKOL_PRIVATE void yk___sg_d3d11_init_caps(void) {
  yk___sg.backend = YK__SG_BACKEND_D3D11;
  yk___sg.features.instancing = true;
  yk___sg.features.origin_top_left = true;
  yk___sg.features.multiple_render_targets = true;
  yk___sg.features.msaa_render_targets = true;
  yk___sg.features.imagetype_3d = true;
  yk___sg.features.imagetype_array = true;
  yk___sg.features.image_clamp_to_border = true;
  yk___sg.features.mrt_independent_blend_state = true;
  yk___sg.features.mrt_independent_write_mask = true;
  yk___sg.limits.max_image_size_2d = 16 * 1024;
  yk___sg.limits.max_image_size_cube = 16 * 1024;
  yk___sg.limits.max_image_size_3d = 2 * 1024;
  yk___sg.limits.max_image_size_array = 16 * 1024;
  yk___sg.limits.max_image_array_layers = 2 * 1024;
  yk___sg.limits.max_vertex_attrs = YK__SG_MAX_VERTEX_ATTRIBUTES;
  /* see: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_format_support */
  for (int fmt = (YK__SG_PIXELFORMAT_NONE + 1); fmt < YK___SG_PIXELFORMAT_NUM;
       fmt++) {
    UINT dxgi_fmt_caps = 0;
    const DXGI_FORMAT dxgi_fmt =
        yk___sg_d3d11_pixel_format((yk__sg_pixel_format) fmt);
    if (dxgi_fmt != DXGI_FORMAT_UNKNOWN) {
      HRESULT hr = yk___sg_d3d11_CheckFormatSupport(yk___sg.d3d11.dev, dxgi_fmt,
                                                    &dxgi_fmt_caps);
      YK__SOKOL_ASSERT(SUCCEEDED(hr) || (E_FAIL == hr));
      if (!SUCCEEDED(hr)) { dxgi_fmt_caps = 0; }
    }
    yk__sg_pixelformat_info *info = &yk___sg.formats[fmt];
    info->sample = 0 != (dxgi_fmt_caps & D3D11_FORMAT_SUPPORT_TEXTURE2D);
    info->filter = 0 != (dxgi_fmt_caps & D3D11_FORMAT_SUPPORT_SHADER_SAMPLE);
    info->render = 0 != (dxgi_fmt_caps & D3D11_FORMAT_SUPPORT_RENDER_TARGET);
    info->blend = 0 != (dxgi_fmt_caps & D3D11_FORMAT_SUPPORT_BLENDABLE);
    info->msaa =
        0 != (dxgi_fmt_caps & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET);
    info->depth = 0 != (dxgi_fmt_caps & D3D11_FORMAT_SUPPORT_DEPTH_STENCIL);
    if (info->depth) { info->render = true; }
  }
}
YK___SOKOL_PRIVATE void yk___sg_d3d11_setup_backend(const yk__sg_desc *desc) {
  /* assume yk___sg.d3d11 already is zero-initialized */
  YK__SOKOL_ASSERT(desc);
  YK__SOKOL_ASSERT(desc->context.d3d11.device);
  YK__SOKOL_ASSERT(desc->context.d3d11.device_context);
  YK__SOKOL_ASSERT(desc->context.d3d11.render_target_view_cb ||
                   desc->context.d3d11.render_target_view_userdata_cb);
  YK__SOKOL_ASSERT(desc->context.d3d11.depth_stencil_view_cb ||
                   desc->context.d3d11.depth_stencil_view_userdata_cb);
  yk___sg.d3d11.valid = true;
  yk___sg.d3d11.dev = (ID3D11Device *) desc->context.d3d11.device;
  yk___sg.d3d11.ctx =
      (ID3D11DeviceContext *) desc->context.d3d11.device_context;
  yk___sg.d3d11.rtv_cb = desc->context.d3d11.render_target_view_cb;
  yk___sg.d3d11.rtv_userdata_cb =
      desc->context.d3d11.render_target_view_userdata_cb;
  yk___sg.d3d11.dsv_cb = desc->context.d3d11.depth_stencil_view_cb;
  yk___sg.d3d11.dsv_userdata_cb =
      desc->context.d3d11.depth_stencil_view_userdata_cb;
  yk___sg.d3d11.user_data = desc->context.d3d11.user_data;
  yk___sg_d3d11_init_caps();
}
YK___SOKOL_PRIVATE void yk___sg_d3d11_discard_backend(void) {
  YK__SOKOL_ASSERT(yk___sg.d3d11.valid);
  yk___sg.d3d11.valid = false;
}
YK___SOKOL_PRIVATE void yk___sg_d3d11_clear_state(void) {
  /* clear all the device context state, so that resource refs don't keep stuck in the d3d device context */
  yk___sg_d3d11_ClearState(yk___sg.d3d11.ctx);
}
YK___SOKOL_PRIVATE void yk___sg_d3d11_reset_state_cache(void) {
  /* just clear the d3d11 device context state */
  yk___sg_d3d11_clear_state();
}
YK___SOKOL_PRIVATE void yk___sg_d3d11_activate_context(yk___sg_context_t *ctx) {
  YK___SOKOL_UNUSED(ctx);
  yk___sg_d3d11_clear_state();
}
YK___SOKOL_PRIVATE yk__sg_resource_state
yk___sg_d3d11_create_context(yk___sg_context_t *ctx) {
  YK__SOKOL_ASSERT(ctx);
  YK___SOKOL_UNUSED(ctx);
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_d3d11_destroy_context(yk___sg_context_t *ctx) {
  YK__SOKOL_ASSERT(ctx);
  YK___SOKOL_UNUSED(ctx);
  /* empty */
}
YK___SOKOL_PRIVATE yk__sg_resource_state yk___sg_d3d11_create_buffer(
    yk___sg_buffer_t *buf, const yk__sg_buffer_desc *desc) {
  YK__SOKOL_ASSERT(buf && desc);
  YK__SOKOL_ASSERT(!buf->d3d11.buf);
  yk___sg_buffer_common_init(&buf->cmn, desc);
  const bool injected = (0 != desc->d3d11_buffer);
  if (injected) {
    buf->d3d11.buf = (ID3D11Buffer *) desc->d3d11_buffer;
    yk___sg_d3d11_AddRef(buf->d3d11.buf);
  } else {
    D3D11_BUFFER_DESC d3d11_desc;
    memset(&d3d11_desc, 0, sizeof(d3d11_desc));
    d3d11_desc.ByteWidth = (UINT) buf->cmn.size;
    d3d11_desc.Usage = yk___sg_d3d11_usage(buf->cmn.usage);
    d3d11_desc.BindFlags = buf->cmn.type == YK__SG_BUFFERTYPE_VERTEXBUFFER
                               ? D3D11_BIND_VERTEX_BUFFER
                               : D3D11_BIND_INDEX_BUFFER;
    d3d11_desc.CPUAccessFlags = yk___sg_d3d11_cpu_access_flags(buf->cmn.usage);
    D3D11_SUBRESOURCE_DATA *init_data_ptr = 0;
    D3D11_SUBRESOURCE_DATA init_data;
    memset(&init_data, 0, sizeof(init_data));
    if (buf->cmn.usage == YK__SG_USAGE_IMMUTABLE) {
      YK__SOKOL_ASSERT(desc->data.ptr);
      init_data.pSysMem = desc->data.ptr;
      init_data_ptr = &init_data;
    }
    HRESULT hr = yk___sg_d3d11_CreateBuffer(yk___sg.d3d11.dev, &d3d11_desc,
                                            init_data_ptr, &buf->d3d11.buf);
    YK___SOKOL_UNUSED(hr);
    YK__SOKOL_ASSERT(SUCCEEDED(hr) && buf->d3d11.buf);
  }
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_d3d11_destroy_buffer(yk___sg_buffer_t *buf) {
  YK__SOKOL_ASSERT(buf);
  if (buf->d3d11.buf) { yk___sg_d3d11_Release(buf->d3d11.buf); }
}
YK___SOKOL_PRIVATE void
yk___sg_d3d11_fill_subres_data(const yk___sg_image_t *img,
                               const yk__sg_image_data *data) {
  const int num_faces = (img->cmn.type == YK__SG_IMAGETYPE_CUBE) ? 6 : 1;
  const int num_slices =
      (img->cmn.type == YK__SG_IMAGETYPE_ARRAY) ? img->cmn.num_slices : 1;
  int subres_index = 0;
  for (int face_index = 0; face_index < num_faces; face_index++) {
    for (int slice_index = 0; slice_index < num_slices; slice_index++) {
      for (int mip_index = 0; mip_index < img->cmn.num_mipmaps;
           mip_index++, subres_index++) {
        YK__SOKOL_ASSERT(subres_index <
                         (YK__SG_MAX_MIPMAPS * YK__SG_MAX_TEXTUREARRAY_LAYERS));
        D3D11_SUBRESOURCE_DATA *subres_data =
            &yk___sg.d3d11.subres_data[subres_index];
        const int mip_width = ((img->cmn.width >> mip_index) > 0)
                                  ? img->cmn.width >> mip_index
                                  : 1;
        const int mip_height = ((img->cmn.height >> mip_index) > 0)
                                   ? img->cmn.height >> mip_index
                                   : 1;
        const yk__sg_range *subimg_data =
            &(data->subimage[face_index][mip_index]);
        const size_t slice_size = subimg_data->size / (size_t) num_slices;
        const size_t slice_offset = slice_size * (size_t) slice_index;
        const uint8_t *ptr = (const uint8_t *) subimg_data->ptr;
        subres_data->pSysMem = ptr + slice_offset;
        subres_data->SysMemPitch =
            (UINT) yk___sg_row_pitch(img->cmn.pixel_format, mip_width, 1);
        if (img->cmn.type == YK__SG_IMAGETYPE_3D) {
          /* FIXME? const int mip_depth = ((img->depth>>mip_index)>0) ? img->depth>>mip_index : 1; */
          subres_data->SysMemSlicePitch = (UINT) yk___sg_surface_pitch(
              img->cmn.pixel_format, mip_width, mip_height, 1);
        } else {
          subres_data->SysMemSlicePitch = 0;
        }
      }
    }
  }
}
YK___SOKOL_PRIVATE yk__sg_resource_state yk___sg_d3d11_create_image(
    yk___sg_image_t *img, const yk__sg_image_desc *desc) {
  YK__SOKOL_ASSERT(img && desc);
  YK__SOKOL_ASSERT(!img->d3d11.tex2d && !img->d3d11.tex3d &&
                   !img->d3d11.texds && !img->d3d11.texmsaa);
  YK__SOKOL_ASSERT(!img->d3d11.srv && !img->d3d11.smp);
  HRESULT hr;
  YK___SOKOL_UNUSED(hr);
  yk___sg_image_common_init(&img->cmn, desc);
  const bool injected =
      (0 != desc->d3d11_texture) || (0 != desc->d3d11_shader_resource_view);
  const bool msaa = (img->cmn.sample_count > 1);
  img->d3d11.format = yk___sg_d3d11_pixel_format(img->cmn.pixel_format);
  /* special case depth-stencil buffer? */
  if (yk___sg_is_valid_rendertarget_depth_format(img->cmn.pixel_format)) {
    /* create only a depth-texture */
    YK__SOKOL_ASSERT(!injected);
    if (img->d3d11.format == DXGI_FORMAT_UNKNOWN) {
      YK__SOKOL_LOG("trying to create a D3D11 depth-texture with unsupported "
                    "pixel format\n");
      return YK__SG_RESOURCESTATE_FAILED;
    }
    D3D11_TEXTURE2D_DESC d3d11_desc;
    memset(&d3d11_desc, 0, sizeof(d3d11_desc));
    d3d11_desc.Width = (UINT) img->cmn.width;
    d3d11_desc.Height = (UINT) img->cmn.height;
    d3d11_desc.MipLevels = 1;
    d3d11_desc.ArraySize = 1;
    d3d11_desc.Format = img->d3d11.format;
    d3d11_desc.Usage = D3D11_USAGE_DEFAULT;
    d3d11_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    d3d11_desc.SampleDesc.Count = (UINT) img->cmn.sample_count;
    d3d11_desc.SampleDesc.Quality =
        (UINT) (msaa ? D3D11_STANDARD_MULTISAMPLE_PATTERN : 0);
    hr = yk___sg_d3d11_CreateTexture2D(yk___sg.d3d11.dev, &d3d11_desc, NULL,
                                       &img->d3d11.texds);
    YK__SOKOL_ASSERT(SUCCEEDED(hr) && img->d3d11.texds);
  } else {
    /* create (or inject) color texture and shader-resource-view */
    /* prepare initial content pointers */
    D3D11_SUBRESOURCE_DATA *init_data = 0;
    if (!injected && (img->cmn.usage == YK__SG_USAGE_IMMUTABLE) &&
        !img->cmn.render_target) {
      yk___sg_d3d11_fill_subres_data(img, &desc->data);
      init_data = yk___sg.d3d11.subres_data;
    }
    if (img->cmn.type != YK__SG_IMAGETYPE_3D) {
      /* 2D-, cube- or array-texture */
      /* if this is an MSAA render target, the following texture will be the 'resolve-texture' */
      /* first check for injected texture and/or resource view */
      if (injected) {
        img->d3d11.tex2d = (ID3D11Texture2D *) desc->d3d11_texture;
        img->d3d11.srv =
            (ID3D11ShaderResourceView *) desc->d3d11_shader_resource_view;
        if (img->d3d11.tex2d) {
          yk___sg_d3d11_AddRef(img->d3d11.tex2d);
        } else {
          /* if only a shader-resource-view was provided, but no texture, lookup
                       the texture from the shader-resource-view, this also bumps the refcount
                    */
          YK__SOKOL_ASSERT(img->d3d11.srv);
          yk___sg_d3d11_GetResource((ID3D11View *) img->d3d11.srv,
                                    (ID3D11Resource **) &img->d3d11.tex2d);
          YK__SOKOL_ASSERT(img->d3d11.tex2d);
        }
        if (img->d3d11.srv) { yk___sg_d3d11_AddRef(img->d3d11.srv); }
      }
      /* if not injected, create texture */
      if (0 == img->d3d11.tex2d) {
        D3D11_TEXTURE2D_DESC d3d11_tex_desc;
        memset(&d3d11_tex_desc, 0, sizeof(d3d11_tex_desc));
        d3d11_tex_desc.Width = (UINT) img->cmn.width;
        d3d11_tex_desc.Height = (UINT) img->cmn.height;
        d3d11_tex_desc.MipLevels = (UINT) img->cmn.num_mipmaps;
        switch (img->cmn.type) {
          case YK__SG_IMAGETYPE_ARRAY:
            d3d11_tex_desc.ArraySize = (UINT) img->cmn.num_slices;
            break;
          case YK__SG_IMAGETYPE_CUBE:
            d3d11_tex_desc.ArraySize = 6;
            break;
          default:
            d3d11_tex_desc.ArraySize = 1;
            break;
        }
        d3d11_tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        d3d11_tex_desc.Format = img->d3d11.format;
        if (img->cmn.render_target) {
          d3d11_tex_desc.Usage = D3D11_USAGE_DEFAULT;
          if (!msaa) { d3d11_tex_desc.BindFlags |= D3D11_BIND_RENDER_TARGET; }
          d3d11_tex_desc.CPUAccessFlags = 0;
        } else {
          d3d11_tex_desc.Usage = yk___sg_d3d11_usage(img->cmn.usage);
          d3d11_tex_desc.CPUAccessFlags =
              yk___sg_d3d11_cpu_access_flags(img->cmn.usage);
        }
        if (img->d3d11.format == DXGI_FORMAT_UNKNOWN) {
          /* trying to create a texture format that's not supported by D3D */
          YK__SOKOL_LOG("trying to create a D3D11 texture with unsupported "
                        "pixel format\n");
          return YK__SG_RESOURCESTATE_FAILED;
        }
        d3d11_tex_desc.SampleDesc.Count = 1;
        d3d11_tex_desc.SampleDesc.Quality = 0;
        d3d11_tex_desc.MiscFlags = (img->cmn.type == YK__SG_IMAGETYPE_CUBE)
                                       ? D3D11_RESOURCE_MISC_TEXTURECUBE
                                       : 0;
        hr = yk___sg_d3d11_CreateTexture2D(yk___sg.d3d11.dev, &d3d11_tex_desc,
                                           init_data, &img->d3d11.tex2d);
        YK__SOKOL_ASSERT(SUCCEEDED(hr) && img->d3d11.tex2d);
      }
      /* ...and similar, if not injected, create shader-resource-view */
      if (0 == img->d3d11.srv) {
        D3D11_SHADER_RESOURCE_VIEW_DESC d3d11_srv_desc;
        memset(&d3d11_srv_desc, 0, sizeof(d3d11_srv_desc));
        d3d11_srv_desc.Format = img->d3d11.format;
        switch (img->cmn.type) {
          case YK__SG_IMAGETYPE_2D:
            d3d11_srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            d3d11_srv_desc.Texture2D.MipLevels = (UINT) img->cmn.num_mipmaps;
            break;
          case YK__SG_IMAGETYPE_CUBE:
            d3d11_srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
            d3d11_srv_desc.TextureCube.MipLevels = (UINT) img->cmn.num_mipmaps;
            break;
          case YK__SG_IMAGETYPE_ARRAY:
            d3d11_srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            d3d11_srv_desc.Texture2DArray.MipLevels =
                (UINT) img->cmn.num_mipmaps;
            d3d11_srv_desc.Texture2DArray.ArraySize =
                (UINT) img->cmn.num_slices;
            break;
          default:
            YK__SOKOL_UNREACHABLE;
            break;
        }
        hr = yk___sg_d3d11_CreateShaderResourceView(
            yk___sg.d3d11.dev, (ID3D11Resource *) img->d3d11.tex2d,
            &d3d11_srv_desc, &img->d3d11.srv);
        YK__SOKOL_ASSERT(SUCCEEDED(hr) && img->d3d11.srv);
      }
    } else {
      /* 3D texture - same procedure, first check if injected, than create non-injected */
      if (injected) {
        img->d3d11.tex3d = (ID3D11Texture3D *) desc->d3d11_texture;
        img->d3d11.srv =
            (ID3D11ShaderResourceView *) desc->d3d11_shader_resource_view;
        if (img->d3d11.tex3d) {
          yk___sg_d3d11_AddRef(img->d3d11.tex3d);
        } else {
          YK__SOKOL_ASSERT(img->d3d11.srv);
          yk___sg_d3d11_GetResource((ID3D11View *) img->d3d11.srv,
                                    (ID3D11Resource **) &img->d3d11.tex3d);
          YK__SOKOL_ASSERT(img->d3d11.tex3d);
        }
        if (img->d3d11.srv) { yk___sg_d3d11_AddRef(img->d3d11.srv); }
      }
      if (0 == img->d3d11.tex3d) {
        D3D11_TEXTURE3D_DESC d3d11_tex_desc;
        memset(&d3d11_tex_desc, 0, sizeof(d3d11_tex_desc));
        d3d11_tex_desc.Width = (UINT) img->cmn.width;
        d3d11_tex_desc.Height = (UINT) img->cmn.height;
        d3d11_tex_desc.Depth = (UINT) img->cmn.num_slices;
        d3d11_tex_desc.MipLevels = (UINT) img->cmn.num_mipmaps;
        d3d11_tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        d3d11_tex_desc.Format = img->d3d11.format;
        if (img->cmn.render_target) {
          d3d11_tex_desc.Usage = D3D11_USAGE_DEFAULT;
          if (!msaa) { d3d11_tex_desc.BindFlags |= D3D11_BIND_RENDER_TARGET; }
          d3d11_tex_desc.CPUAccessFlags = 0;
        } else {
          d3d11_tex_desc.Usage = yk___sg_d3d11_usage(img->cmn.usage);
          d3d11_tex_desc.CPUAccessFlags =
              yk___sg_d3d11_cpu_access_flags(img->cmn.usage);
        }
        if (img->d3d11.format == DXGI_FORMAT_UNKNOWN) {
          /* trying to create a texture format that's not supported by D3D */
          YK__SOKOL_LOG("trying to create a D3D11 texture with unsupported "
                        "pixel format\n");
          return YK__SG_RESOURCESTATE_FAILED;
        }
        hr = yk___sg_d3d11_CreateTexture3D(yk___sg.d3d11.dev, &d3d11_tex_desc,
                                           init_data, &img->d3d11.tex3d);
        YK__SOKOL_ASSERT(SUCCEEDED(hr) && img->d3d11.tex3d);
      }
      if (0 == img->d3d11.srv) {
        D3D11_SHADER_RESOURCE_VIEW_DESC d3d11_srv_desc;
        memset(&d3d11_srv_desc, 0, sizeof(d3d11_srv_desc));
        d3d11_srv_desc.Format = img->d3d11.format;
        d3d11_srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
        d3d11_srv_desc.Texture3D.MipLevels = (UINT) img->cmn.num_mipmaps;
        hr = yk___sg_d3d11_CreateShaderResourceView(
            yk___sg.d3d11.dev, (ID3D11Resource *) img->d3d11.tex3d,
            &d3d11_srv_desc, &img->d3d11.srv);
        YK__SOKOL_ASSERT(SUCCEEDED(hr) && img->d3d11.srv);
      }
    }
    /* also need to create a separate MSAA render target texture? */
    if (msaa) {
      D3D11_TEXTURE2D_DESC d3d11_tex_desc;
      memset(&d3d11_tex_desc, 0, sizeof(d3d11_tex_desc));
      d3d11_tex_desc.Width = (UINT) img->cmn.width;
      d3d11_tex_desc.Height = (UINT) img->cmn.height;
      d3d11_tex_desc.MipLevels = 1;
      d3d11_tex_desc.ArraySize = 1;
      d3d11_tex_desc.Format = img->d3d11.format;
      d3d11_tex_desc.Usage = D3D11_USAGE_DEFAULT;
      d3d11_tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET;
      d3d11_tex_desc.CPUAccessFlags = 0;
      d3d11_tex_desc.SampleDesc.Count = (UINT) img->cmn.sample_count;
      d3d11_tex_desc.SampleDesc.Quality =
          (UINT) D3D11_STANDARD_MULTISAMPLE_PATTERN;
      hr = yk___sg_d3d11_CreateTexture2D(yk___sg.d3d11.dev, &d3d11_tex_desc,
                                         NULL, &img->d3d11.texmsaa);
      YK__SOKOL_ASSERT(SUCCEEDED(hr) && img->d3d11.texmsaa);
    }
    /* sampler state object, note D3D11 implements an internal shared-pool for sampler objects */
    D3D11_SAMPLER_DESC d3d11_smp_desc;
    memset(&d3d11_smp_desc, 0, sizeof(d3d11_smp_desc));
    d3d11_smp_desc.Filter = yk___sg_d3d11_filter(
        img->cmn.min_filter, img->cmn.mag_filter, img->cmn.max_anisotropy);
    d3d11_smp_desc.AddressU = yk___sg_d3d11_address_mode(img->cmn.wrap_u);
    d3d11_smp_desc.AddressV = yk___sg_d3d11_address_mode(img->cmn.wrap_v);
    d3d11_smp_desc.AddressW = yk___sg_d3d11_address_mode(img->cmn.wrap_w);
    switch (img->cmn.border_color) {
      case YK__SG_BORDERCOLOR_TRANSPARENT_BLACK:
        /* all 0.0f */
        break;
      case YK__SG_BORDERCOLOR_OPAQUE_WHITE:
        for (int i = 0; i < 4; i++) { d3d11_smp_desc.BorderColor[i] = 1.0f; }
        break;
      default:
        /* opaque black */
        d3d11_smp_desc.BorderColor[3] = 1.0f;
        break;
    }
    d3d11_smp_desc.MaxAnisotropy = img->cmn.max_anisotropy;
    d3d11_smp_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    d3d11_smp_desc.MinLOD = desc->min_lod;
    d3d11_smp_desc.MaxLOD = desc->max_lod;
    hr = yk___sg_d3d11_CreateSamplerState(yk___sg.d3d11.dev, &d3d11_smp_desc,
                                          &img->d3d11.smp);
    YK__SOKOL_ASSERT(SUCCEEDED(hr) && img->d3d11.smp);
  }
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_d3d11_destroy_image(yk___sg_image_t *img) {
  YK__SOKOL_ASSERT(img);
  if (img->d3d11.tex2d) { yk___sg_d3d11_Release(img->d3d11.tex2d); }
  if (img->d3d11.tex3d) { yk___sg_d3d11_Release(img->d3d11.tex3d); }
  if (img->d3d11.texds) { yk___sg_d3d11_Release(img->d3d11.texds); }
  if (img->d3d11.texmsaa) { yk___sg_d3d11_Release(img->d3d11.texmsaa); }
  if (img->d3d11.srv) { yk___sg_d3d11_Release(img->d3d11.srv); }
  if (img->d3d11.smp) { yk___sg_d3d11_Release(img->d3d11.smp); }
}
YK___SOKOL_PRIVATE bool yk___sg_d3d11_load_d3dcompiler_dll(void) {
/* on UWP, don't do anything (not tested) */
#if (defined(WINAPI_FAMILY_PARTITION) &&                                       \
     !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP))
  return true;
#else
  /* load DLL on demand */
  if ((0 == yk___sg.d3d11.d3dcompiler_dll) &&
      !yk___sg.d3d11.d3dcompiler_dll_load_failed) {
    yk___sg.d3d11.d3dcompiler_dll = LoadLibraryA("d3dcompiler_47.dll");
    if (0 == yk___sg.d3d11.d3dcompiler_dll) {
      /* don't attempt to load missing DLL in the future */
      YK__SOKOL_LOG("failed to load d3dcompiler_47.dll!\n");
      yk___sg.d3d11.d3dcompiler_dll_load_failed = true;
      return false;
    }
    /* look up function pointers */
    yk___sg.d3d11.D3DCompile_func = (pD3DCompile) (void *) GetProcAddress(
        yk___sg.d3d11.d3dcompiler_dll, "D3DCompile");
    YK__SOKOL_ASSERT(yk___sg.d3d11.D3DCompile_func);
  }
  return 0 != yk___sg.d3d11.d3dcompiler_dll;
#endif
}
#if (defined(WINAPI_FAMILY_PARTITION) &&                                       \
     !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP))
#define yk___sg_d3d11_D3DCompile D3DCompile
#else
#define yk___sg_d3d11_D3DCompile yk___sg.d3d11.D3DCompile_func
#endif
YK___SOKOL_PRIVATE ID3DBlob *
yk___sg_d3d11_compile_shader(const yk__sg_shader_stage_desc *stage_desc) {
  if (!yk___sg_d3d11_load_d3dcompiler_dll()) { return NULL; }
  YK__SOKOL_ASSERT(stage_desc->d3d11_target);
  ID3DBlob *output = NULL;
  ID3DBlob *errors_or_warnings = NULL;
  HRESULT hr = yk___sg_d3d11_D3DCompile(
      stage_desc->source,                             /* pSrcData */
      strlen(stage_desc->source),                     /* SrcDataSize */
      NULL,                                           /* pSourceName */
      NULL,                                           /* pDefines */
      NULL,                                           /* pInclude */
      stage_desc->entry ? stage_desc->entry : "main", /* pEntryPoint */
      stage_desc->d3d11_target, /* pTarget (vs_5_0 or ps_5_0) */
      D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR |
          D3DCOMPILE_OPTIMIZATION_LEVEL3, /* Flags1 */
      0,                                  /* Flags2 */
      &output,                            /* ppCode */
      &errors_or_warnings);               /* ppErrorMsgs */
  if (errors_or_warnings) {
    YK__SOKOL_LOG((LPCSTR) yk___sg_d3d11_GetBufferPointer(errors_or_warnings));
    yk___sg_d3d11_Release(errors_or_warnings);
    errors_or_warnings = NULL;
  }
  if (FAILED(hr)) {
    /* just in case, usually output is NULL here */
    if (output) {
      yk___sg_d3d11_Release(output);
      output = NULL;
    }
  }
  return output;
}
YK___SOKOL_PRIVATE yk__sg_resource_state yk___sg_d3d11_create_shader(
    yk___sg_shader_t *shd, const yk__sg_shader_desc *desc) {
  YK__SOKOL_ASSERT(shd && desc);
  YK__SOKOL_ASSERT(!shd->d3d11.vs && !shd->d3d11.fs && !shd->d3d11.vs_blob);
  HRESULT hr;
  YK___SOKOL_UNUSED(hr);
  yk___sg_shader_common_init(&shd->cmn, desc);
  /* copy vertex attribute semantic names and indices */
  for (int i = 0; i < YK__SG_MAX_VERTEX_ATTRIBUTES; i++) {
    yk___sg_strcpy(&shd->d3d11.attrs[i].sem_name, desc->attrs[i].sem_name);
    shd->d3d11.attrs[i].sem_index = desc->attrs[i].sem_index;
  }
  /* shader stage uniform blocks and image slots */
  for (int stage_index = 0; stage_index < YK__SG_NUM_SHADER_STAGES;
       stage_index++) {
    yk___sg_shader_stage_t *cmn_stage = &shd->cmn.stage[stage_index];
    yk___sg_d3d11_shader_stage_t *d3d11_stage = &shd->d3d11.stage[stage_index];
    for (int ub_index = 0; ub_index < cmn_stage->num_uniform_blocks;
         ub_index++) {
      const yk___sg_uniform_block_t *ub = &cmn_stage->uniform_blocks[ub_index];
      /* create a D3D constant buffer for each uniform block */
      YK__SOKOL_ASSERT(0 == d3d11_stage->cbufs[ub_index]);
      D3D11_BUFFER_DESC cb_desc;
      memset(&cb_desc, 0, sizeof(cb_desc));
      cb_desc.ByteWidth = (UINT) yk___sg_roundup((int) ub->size, 16);
      cb_desc.Usage = D3D11_USAGE_DEFAULT;
      cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
      hr = yk___sg_d3d11_CreateBuffer(yk___sg.d3d11.dev, &cb_desc, NULL,
                                      &d3d11_stage->cbufs[ub_index]);
      YK__SOKOL_ASSERT(SUCCEEDED(hr) && d3d11_stage->cbufs[ub_index]);
    }
  }
  const void *vs_ptr = 0, *fs_ptr = 0;
  SIZE_T vs_length = 0, fs_length = 0;
  ID3DBlob *vs_blob = 0, *fs_blob = 0;
  if (desc->vs.bytecode.ptr && desc->fs.bytecode.ptr) {
    /* create from shader byte code */
    vs_ptr = desc->vs.bytecode.ptr;
    fs_ptr = desc->fs.bytecode.ptr;
    vs_length = desc->vs.bytecode.size;
    fs_length = desc->fs.bytecode.size;
  } else {
    /* compile from shader source code */
    vs_blob = yk___sg_d3d11_compile_shader(&desc->vs);
    fs_blob = yk___sg_d3d11_compile_shader(&desc->fs);
    if (vs_blob && fs_blob) {
      vs_ptr = yk___sg_d3d11_GetBufferPointer(vs_blob);
      vs_length = yk___sg_d3d11_GetBufferSize(vs_blob);
      fs_ptr = yk___sg_d3d11_GetBufferPointer(fs_blob);
      fs_length = yk___sg_d3d11_GetBufferSize(fs_blob);
    }
  }
  yk__sg_resource_state result = YK__SG_RESOURCESTATE_FAILED;
  if (vs_ptr && fs_ptr && (vs_length > 0) && (fs_length > 0)) {
    /* create the D3D vertex- and pixel-shader objects */
    hr = yk___sg_d3d11_CreateVertexShader(yk___sg.d3d11.dev, vs_ptr, vs_length,
                                          NULL, &shd->d3d11.vs);
    bool vs_succeeded = SUCCEEDED(hr) && shd->d3d11.vs;
    hr = yk___sg_d3d11_CreatePixelShader(yk___sg.d3d11.dev, fs_ptr, fs_length,
                                         NULL, &shd->d3d11.fs);
    bool fs_succeeded = SUCCEEDED(hr) && shd->d3d11.fs;
    /* need to store the vertex shader byte code, this is needed later in sg_create_pipeline */
    if (vs_succeeded && fs_succeeded) {
      shd->d3d11.vs_blob_length = vs_length;
      shd->d3d11.vs_blob = YK__SOKOL_MALLOC((size_t) vs_length);
      YK__SOKOL_ASSERT(shd->d3d11.vs_blob);
      memcpy(shd->d3d11.vs_blob, vs_ptr, vs_length);
      result = YK__SG_RESOURCESTATE_VALID;
    }
  }
  if (vs_blob) {
    yk___sg_d3d11_Release(vs_blob);
    vs_blob = 0;
  }
  if (fs_blob) {
    yk___sg_d3d11_Release(fs_blob);
    fs_blob = 0;
  }
  return result;
}
YK___SOKOL_PRIVATE void yk___sg_d3d11_destroy_shader(yk___sg_shader_t *shd) {
  YK__SOKOL_ASSERT(shd);
  if (shd->d3d11.vs) { yk___sg_d3d11_Release(shd->d3d11.vs); }
  if (shd->d3d11.fs) { yk___sg_d3d11_Release(shd->d3d11.fs); }
  if (shd->d3d11.vs_blob) { YK__SOKOL_FREE(shd->d3d11.vs_blob); }
  for (int stage_index = 0; stage_index < YK__SG_NUM_SHADER_STAGES;
       stage_index++) {
    yk___sg_shader_stage_t *cmn_stage = &shd->cmn.stage[stage_index];
    yk___sg_d3d11_shader_stage_t *d3d11_stage = &shd->d3d11.stage[stage_index];
    for (int ub_index = 0; ub_index < cmn_stage->num_uniform_blocks;
         ub_index++) {
      if (d3d11_stage->cbufs[ub_index]) {
        yk___sg_d3d11_Release(d3d11_stage->cbufs[ub_index]);
      }
    }
  }
}
YK___SOKOL_PRIVATE yk__sg_resource_state
yk___sg_d3d11_create_pipeline(yk___sg_pipeline_t *pip, yk___sg_shader_t *shd,
                              const yk__sg_pipeline_desc *desc) {
  YK__SOKOL_ASSERT(pip && shd && desc);
  YK__SOKOL_ASSERT(desc->shader.id == shd->slot.id);
  YK__SOKOL_ASSERT(shd->slot.state == YK__SG_RESOURCESTATE_VALID);
  YK__SOKOL_ASSERT(shd->d3d11.vs_blob && shd->d3d11.vs_blob_length > 0);
  YK__SOKOL_ASSERT(!pip->d3d11.il && !pip->d3d11.rs && !pip->d3d11.dss &&
                   !pip->d3d11.bs);
  pip->shader = shd;
  yk___sg_pipeline_common_init(&pip->cmn, desc);
  pip->d3d11.index_format = yk___sg_d3d11_index_format(pip->cmn.index_type);
  pip->d3d11.topology = yk___sg_d3d11_primitive_topology(desc->primitive_type);
  pip->d3d11.stencil_ref = desc->stencil.ref;
  /* create input layout object */
  HRESULT hr;
  YK___SOKOL_UNUSED(hr);
  D3D11_INPUT_ELEMENT_DESC d3d11_comps[YK__SG_MAX_VERTEX_ATTRIBUTES];
  memset(d3d11_comps, 0, sizeof(d3d11_comps));
  int attr_index = 0;
  for (; attr_index < YK__SG_MAX_VERTEX_ATTRIBUTES; attr_index++) {
    const yk__sg_vertex_attr_desc *a_desc = &desc->layout.attrs[attr_index];
    if (a_desc->format == YK__SG_VERTEXFORMAT_INVALID) { break; }
    YK__SOKOL_ASSERT(a_desc->buffer_index < YK__SG_MAX_SHADERSTAGE_BUFFERS);
    const yk__sg_buffer_layout_desc *l_desc =
        &desc->layout.buffers[a_desc->buffer_index];
    const yk__sg_vertex_step step_func = l_desc->step_func;
    const int step_rate = l_desc->step_rate;
    D3D11_INPUT_ELEMENT_DESC *d3d11_comp = &d3d11_comps[attr_index];
    d3d11_comp->SemanticName =
        yk___sg_strptr(&shd->d3d11.attrs[attr_index].sem_name);
    d3d11_comp->SemanticIndex = (UINT) shd->d3d11.attrs[attr_index].sem_index;
    d3d11_comp->Format = yk___sg_d3d11_vertex_format(a_desc->format);
    d3d11_comp->InputSlot = (UINT) a_desc->buffer_index;
    d3d11_comp->AlignedByteOffset = (UINT) a_desc->offset;
    d3d11_comp->InputSlotClass = yk___sg_d3d11_input_classification(step_func);
    if (YK__SG_VERTEXSTEP_PER_INSTANCE == step_func) {
      d3d11_comp->InstanceDataStepRate = (UINT) step_rate;
      pip->cmn.use_instanced_draw = true;
    }
    pip->cmn.vertex_layout_valid[a_desc->buffer_index] = true;
  }
  for (int layout_index = 0; layout_index < YK__SG_MAX_SHADERSTAGE_BUFFERS;
       layout_index++) {
    if (pip->cmn.vertex_layout_valid[layout_index]) {
      const yk__sg_buffer_layout_desc *l_desc =
          &desc->layout.buffers[layout_index];
      YK__SOKOL_ASSERT(l_desc->stride > 0);
      pip->d3d11.vb_strides[layout_index] = (UINT) l_desc->stride;
    } else {
      pip->d3d11.vb_strides[layout_index] = 0;
    }
  }
  hr = yk___sg_d3d11_CreateInputLayout(
      yk___sg.d3d11.dev, d3d11_comps, /* pInputElementDesc */
      (UINT) attr_index,              /* NumElements */
      shd->d3d11.vs_blob,             /* pShaderByteCodeWithInputSignature */
      shd->d3d11.vs_blob_length,      /* BytecodeLength */
      &pip->d3d11.il);
  YK__SOKOL_ASSERT(SUCCEEDED(hr) && pip->d3d11.il);
  /* create rasterizer state */
  D3D11_RASTERIZER_DESC rs_desc;
  memset(&rs_desc, 0, sizeof(rs_desc));
  rs_desc.FillMode = D3D11_FILL_SOLID;
  rs_desc.CullMode = yk___sg_d3d11_cull_mode(desc->cull_mode);
  rs_desc.FrontCounterClockwise = desc->face_winding == YK__SG_FACEWINDING_CCW;
  rs_desc.DepthBias = (INT) pip->cmn.depth_bias;
  rs_desc.DepthBiasClamp = pip->cmn.depth_bias_clamp;
  rs_desc.SlopeScaledDepthBias = pip->cmn.depth_bias_slope_scale;
  rs_desc.DepthClipEnable = TRUE;
  rs_desc.ScissorEnable = TRUE;
  rs_desc.MultisampleEnable = desc->sample_count > 1;
  rs_desc.AntialiasedLineEnable = FALSE;
  hr = yk___sg_d3d11_CreateRasterizerState(yk___sg.d3d11.dev, &rs_desc,
                                           &pip->d3d11.rs);
  YK__SOKOL_ASSERT(SUCCEEDED(hr) && pip->d3d11.rs);
  /* create depth-stencil state */
  D3D11_DEPTH_STENCIL_DESC dss_desc;
  memset(&dss_desc, 0, sizeof(dss_desc));
  dss_desc.DepthEnable = TRUE;
  dss_desc.DepthWriteMask = desc->depth.write_enabled
                                ? D3D11_DEPTH_WRITE_MASK_ALL
                                : D3D11_DEPTH_WRITE_MASK_ZERO;
  dss_desc.DepthFunc = yk___sg_d3d11_compare_func(desc->depth.compare);
  dss_desc.StencilEnable = desc->stencil.enabled;
  dss_desc.StencilReadMask = desc->stencil.read_mask;
  dss_desc.StencilWriteMask = desc->stencil.write_mask;
  const yk__sg_stencil_face_state *sf = &desc->stencil.front;
  dss_desc.FrontFace.StencilFailOp = yk___sg_d3d11_stencil_op(sf->fail_op);
  dss_desc.FrontFace.StencilDepthFailOp =
      yk___sg_d3d11_stencil_op(sf->depth_fail_op);
  dss_desc.FrontFace.StencilPassOp = yk___sg_d3d11_stencil_op(sf->pass_op);
  dss_desc.FrontFace.StencilFunc = yk___sg_d3d11_compare_func(sf->compare);
  const yk__sg_stencil_face_state *sb = &desc->stencil.back;
  dss_desc.BackFace.StencilFailOp = yk___sg_d3d11_stencil_op(sb->fail_op);
  dss_desc.BackFace.StencilDepthFailOp =
      yk___sg_d3d11_stencil_op(sb->depth_fail_op);
  dss_desc.BackFace.StencilPassOp = yk___sg_d3d11_stencil_op(sb->pass_op);
  dss_desc.BackFace.StencilFunc = yk___sg_d3d11_compare_func(sb->compare);
  hr = yk___sg_d3d11_CreateDepthStencilState(yk___sg.d3d11.dev, &dss_desc,
                                             &pip->d3d11.dss);
  YK__SOKOL_ASSERT(SUCCEEDED(hr) && pip->d3d11.dss);
  /* create blend state */
  D3D11_BLEND_DESC bs_desc;
  memset(&bs_desc, 0, sizeof(bs_desc));
  bs_desc.AlphaToCoverageEnable = desc->alpha_to_coverage_enabled;
  bs_desc.IndependentBlendEnable = TRUE;
  {
    int i = 0;
    for (i = 0; i < desc->color_count; i++) {
      const yk__sg_blend_state *src = &desc->colors[i].blend;
      D3D11_RENDER_TARGET_BLEND_DESC *dst = &bs_desc.RenderTarget[i];
      dst->BlendEnable = src->enabled;
      dst->SrcBlend = yk___sg_d3d11_blend_factor(src->src_factor_rgb);
      dst->DestBlend = yk___sg_d3d11_blend_factor(src->dst_factor_rgb);
      dst->BlendOp = yk___sg_d3d11_blend_op(src->op_rgb);
      dst->SrcBlendAlpha = yk___sg_d3d11_blend_factor(src->src_factor_alpha);
      dst->DestBlendAlpha = yk___sg_d3d11_blend_factor(src->dst_factor_alpha);
      dst->BlendOpAlpha = yk___sg_d3d11_blend_op(src->op_alpha);
      dst->RenderTargetWriteMask =
          yk___sg_d3d11_color_write_mask(desc->colors[i].write_mask);
    }
    for (; i < 8; i++) {
      D3D11_RENDER_TARGET_BLEND_DESC *dst = &bs_desc.RenderTarget[i];
      dst->BlendEnable = FALSE;
      dst->SrcBlend = dst->SrcBlendAlpha = D3D11_BLEND_ONE;
      dst->DestBlend = dst->DestBlendAlpha = D3D11_BLEND_ZERO;
      dst->BlendOp = dst->BlendOpAlpha = D3D11_BLEND_OP_ADD;
      dst->RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    }
  }
  hr = yk___sg_d3d11_CreateBlendState(yk___sg.d3d11.dev, &bs_desc,
                                      &pip->d3d11.bs);
  YK__SOKOL_ASSERT(SUCCEEDED(hr) && pip->d3d11.bs);
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void
yk___sg_d3d11_destroy_pipeline(yk___sg_pipeline_t *pip) {
  YK__SOKOL_ASSERT(pip);
  if (pip == yk___sg.d3d11.cur_pipeline) {
    yk___sg.d3d11.cur_pipeline = 0;
    yk___sg.d3d11.cur_pipeline_id.id = YK__SG_INVALID_ID;
  }
  if (pip->d3d11.il) { yk___sg_d3d11_Release(pip->d3d11.il); }
  if (pip->d3d11.rs) { yk___sg_d3d11_Release(pip->d3d11.rs); }
  if (pip->d3d11.dss) { yk___sg_d3d11_Release(pip->d3d11.dss); }
  if (pip->d3d11.bs) { yk___sg_d3d11_Release(pip->d3d11.bs); }
}
YK___SOKOL_PRIVATE yk__sg_resource_state
yk___sg_d3d11_create_pass(yk___sg_pass_t *pass, yk___sg_image_t **att_images,
                          const yk__sg_pass_desc *desc) {
  YK__SOKOL_ASSERT(pass && desc);
  YK__SOKOL_ASSERT(att_images && att_images[0]);
  YK__SOKOL_ASSERT(yk___sg.d3d11.dev);
  yk___sg_pass_common_init(&pass->cmn, desc);
  for (int i = 0; i < pass->cmn.num_color_atts; i++) {
    const yk__sg_pass_attachment_desc *att_desc = &desc->color_attachments[i];
    YK___SOKOL_UNUSED(att_desc);
    YK__SOKOL_ASSERT(att_desc->image.id != YK__SG_INVALID_ID);
    yk___sg_image_t *att_img = att_images[i];
    YK__SOKOL_ASSERT(att_img && (att_img->slot.id == att_desc->image.id));
    YK__SOKOL_ASSERT(
        yk___sg_is_valid_rendertarget_color_format(att_img->cmn.pixel_format));
    YK__SOKOL_ASSERT(0 == pass->d3d11.color_atts[i].image);
    pass->d3d11.color_atts[i].image = att_img;
    /* create D3D11 render-target-view */
    const yk___sg_pass_attachment_t *cmn_att = &pass->cmn.color_atts[i];
    YK__SOKOL_ASSERT(0 == pass->d3d11.color_atts[i].rtv);
    ID3D11Resource *d3d11_res = 0;
    const bool is_msaa = att_img->cmn.sample_count > 1;
    D3D11_RENDER_TARGET_VIEW_DESC d3d11_rtv_desc;
    memset(&d3d11_rtv_desc, 0, sizeof(d3d11_rtv_desc));
    d3d11_rtv_desc.Format = att_img->d3d11.format;
    if ((att_img->cmn.type == YK__SG_IMAGETYPE_2D) || is_msaa) {
      if (is_msaa) {
        d3d11_res = (ID3D11Resource *) att_img->d3d11.texmsaa;
        d3d11_rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
      } else {
        d3d11_res = (ID3D11Resource *) att_img->d3d11.tex2d;
        d3d11_rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        d3d11_rtv_desc.Texture2D.MipSlice = (UINT) cmn_att->mip_level;
      }
    } else if ((att_img->cmn.type == YK__SG_IMAGETYPE_CUBE) ||
               (att_img->cmn.type == YK__SG_IMAGETYPE_ARRAY)) {
      d3d11_res = (ID3D11Resource *) att_img->d3d11.tex2d;
      d3d11_rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
      d3d11_rtv_desc.Texture2DArray.MipSlice = (UINT) cmn_att->mip_level;
      d3d11_rtv_desc.Texture2DArray.FirstArraySlice = (UINT) cmn_att->slice;
      d3d11_rtv_desc.Texture2DArray.ArraySize = 1;
    } else {
      YK__SOKOL_ASSERT(att_img->cmn.type == YK__SG_IMAGETYPE_3D);
      d3d11_res = (ID3D11Resource *) att_img->d3d11.tex3d;
      d3d11_rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
      d3d11_rtv_desc.Texture3D.MipSlice = (UINT) cmn_att->mip_level;
      d3d11_rtv_desc.Texture3D.FirstWSlice = (UINT) cmn_att->slice;
      d3d11_rtv_desc.Texture3D.WSize = 1;
    }
    YK__SOKOL_ASSERT(d3d11_res);
    HRESULT hr = yk___sg_d3d11_CreateRenderTargetView(
        yk___sg.d3d11.dev, d3d11_res, &d3d11_rtv_desc,
        &pass->d3d11.color_atts[i].rtv);
    YK___SOKOL_UNUSED(hr);
    YK__SOKOL_ASSERT(SUCCEEDED(hr) && pass->d3d11.color_atts[i].rtv);
  }
  /* optional depth-stencil image */
  YK__SOKOL_ASSERT(0 == pass->d3d11.ds_att.image);
  YK__SOKOL_ASSERT(0 == pass->d3d11.ds_att.dsv);
  if (desc->depth_stencil_attachment.image.id != YK__SG_INVALID_ID) {
    const int ds_img_index = YK__SG_MAX_COLOR_ATTACHMENTS;
    const yk__sg_pass_attachment_desc *att_desc =
        &desc->depth_stencil_attachment;
    YK___SOKOL_UNUSED(att_desc);
    yk___sg_image_t *att_img = att_images[ds_img_index];
    YK__SOKOL_ASSERT(att_img && (att_img->slot.id == att_desc->image.id));
    YK__SOKOL_ASSERT(
        yk___sg_is_valid_rendertarget_depth_format(att_img->cmn.pixel_format));
    YK__SOKOL_ASSERT(0 == pass->d3d11.ds_att.image);
    pass->d3d11.ds_att.image = att_img;
    /* create D3D11 depth-stencil-view */
    D3D11_DEPTH_STENCIL_VIEW_DESC d3d11_dsv_desc;
    memset(&d3d11_dsv_desc, 0, sizeof(d3d11_dsv_desc));
    d3d11_dsv_desc.Format = att_img->d3d11.format;
    const bool is_msaa = att_img->cmn.sample_count > 1;
    if (is_msaa) {
      d3d11_dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    } else {
      d3d11_dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    }
    ID3D11Resource *d3d11_res = (ID3D11Resource *) att_img->d3d11.texds;
    YK__SOKOL_ASSERT(d3d11_res);
    HRESULT hr = yk___sg_d3d11_CreateDepthStencilView(
        yk___sg.d3d11.dev, d3d11_res, &d3d11_dsv_desc, &pass->d3d11.ds_att.dsv);
    YK___SOKOL_UNUSED(hr);
    YK__SOKOL_ASSERT(SUCCEEDED(hr) && pass->d3d11.ds_att.dsv);
  }
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_d3d11_destroy_pass(yk___sg_pass_t *pass) {
  YK__SOKOL_ASSERT(pass);
  YK__SOKOL_ASSERT(pass != yk___sg.d3d11.cur_pass);
  for (int i = 0; i < YK__SG_MAX_COLOR_ATTACHMENTS; i++) {
    if (pass->d3d11.color_atts[i].rtv) {
      yk___sg_d3d11_Release(pass->d3d11.color_atts[i].rtv);
    }
  }
  if (pass->d3d11.ds_att.dsv) { yk___sg_d3d11_Release(pass->d3d11.ds_att.dsv); }
}
YK___SOKOL_PRIVATE yk___sg_image_t *
yk___sg_d3d11_pass_color_image(const yk___sg_pass_t *pass, int index) {
  YK__SOKOL_ASSERT(pass && (index >= 0) &&
                   (index < YK__SG_MAX_COLOR_ATTACHMENTS));
  /* NOTE: may return null */
  return pass->d3d11.color_atts[index].image;
}
YK___SOKOL_PRIVATE yk___sg_image_t *
yk___sg_d3d11_pass_ds_image(const yk___sg_pass_t *pass) {
  /* NOTE: may return null */
  YK__SOKOL_ASSERT(pass);
  return pass->d3d11.ds_att.image;
}
YK___SOKOL_PRIVATE void
yk___sg_d3d11_begin_pass(yk___sg_pass_t *pass, const yk__sg_pass_action *action,
                         int w, int h) {
  YK__SOKOL_ASSERT(action);
  YK__SOKOL_ASSERT(!yk___sg.d3d11.in_pass);
  YK__SOKOL_ASSERT(yk___sg.d3d11.rtv_cb || yk___sg.d3d11.rtv_userdata_cb);
  YK__SOKOL_ASSERT(yk___sg.d3d11.dsv_cb || yk___sg.d3d11.dsv_userdata_cb);
  yk___sg.d3d11.in_pass = true;
  yk___sg.d3d11.cur_width = w;
  yk___sg.d3d11.cur_height = h;
  if (pass) {
    yk___sg.d3d11.cur_pass = pass;
    yk___sg.d3d11.cur_pass_id.id = pass->slot.id;
    yk___sg.d3d11.num_rtvs = 0;
    for (int i = 0; i < YK__SG_MAX_COLOR_ATTACHMENTS; i++) {
      yk___sg.d3d11.cur_rtvs[i] = pass->d3d11.color_atts[i].rtv;
      if (yk___sg.d3d11.cur_rtvs[i]) { yk___sg.d3d11.num_rtvs++; }
    }
    yk___sg.d3d11.cur_dsv = pass->d3d11.ds_att.dsv;
  } else {
    /* render to default frame buffer */
    yk___sg.d3d11.cur_pass = 0;
    yk___sg.d3d11.cur_pass_id.id = YK__SG_INVALID_ID;
    yk___sg.d3d11.num_rtvs = 1;
    if (yk___sg.d3d11.rtv_cb) {
      yk___sg.d3d11.cur_rtvs[0] =
          (ID3D11RenderTargetView *) yk___sg.d3d11.rtv_cb();
    } else {
      yk___sg.d3d11.cur_rtvs[0] =
          (ID3D11RenderTargetView *) yk___sg.d3d11.rtv_userdata_cb(
              yk___sg.d3d11.user_data);
    }
    for (int i = 1; i < YK__SG_MAX_COLOR_ATTACHMENTS; i++) {
      yk___sg.d3d11.cur_rtvs[i] = 0;
    }
    if (yk___sg.d3d11.dsv_cb) {
      yk___sg.d3d11.cur_dsv = (ID3D11DepthStencilView *) yk___sg.d3d11.dsv_cb();
    } else {
      yk___sg.d3d11.cur_dsv =
          (ID3D11DepthStencilView *) yk___sg.d3d11.dsv_userdata_cb(
              yk___sg.d3d11.user_data);
    }
    YK__SOKOL_ASSERT(yk___sg.d3d11.cur_rtvs[0] && yk___sg.d3d11.cur_dsv);
  }
  /* apply the render-target- and depth-stencil-views */
  yk___sg_d3d11_OMSetRenderTargets(
      yk___sg.d3d11.ctx, YK__SG_MAX_COLOR_ATTACHMENTS, yk___sg.d3d11.cur_rtvs,
      yk___sg.d3d11.cur_dsv);
  /* set viewport and scissor rect to cover whole screen */
  D3D11_VIEWPORT vp;
  memset(&vp, 0, sizeof(vp));
  vp.Width = (FLOAT) w;
  vp.Height = (FLOAT) h;
  vp.MaxDepth = 1.0f;
  yk___sg_d3d11_RSSetViewports(yk___sg.d3d11.ctx, 1, &vp);
  D3D11_RECT rect;
  rect.left = 0;
  rect.top = 0;
  rect.right = w;
  rect.bottom = h;
  yk___sg_d3d11_RSSetScissorRects(yk___sg.d3d11.ctx, 1, &rect);
  /* perform clear action */
  for (int i = 0; i < yk___sg.d3d11.num_rtvs; i++) {
    if (action->colors[i].action == YK__SG_ACTION_CLEAR) {
      yk___sg_d3d11_ClearRenderTargetView(yk___sg.d3d11.ctx,
                                          yk___sg.d3d11.cur_rtvs[i],
                                          &action->colors[i].value.r);
    }
  }
  UINT ds_flags = 0;
  if (action->depth.action == YK__SG_ACTION_CLEAR) {
    ds_flags |= D3D11_CLEAR_DEPTH;
  }
  if (action->stencil.action == YK__SG_ACTION_CLEAR) {
    ds_flags |= D3D11_CLEAR_STENCIL;
  }
  if ((0 != ds_flags) && yk___sg.d3d11.cur_dsv) {
    yk___sg_d3d11_ClearDepthStencilView(
        yk___sg.d3d11.ctx, yk___sg.d3d11.cur_dsv, ds_flags, action->depth.value,
        action->stencil.value);
  }
}
/* D3D11CalcSubresource only exists for C++ */
YK___SOKOL_PRIVATE UINT yk___sg_d3d11_calcsubresource(UINT mip_slice,
                                                      UINT array_slice,
                                                      UINT mip_levels) {
  return mip_slice + array_slice * mip_levels;
}
YK___SOKOL_PRIVATE void yk___sg_d3d11_end_pass(void) {
  YK__SOKOL_ASSERT(yk___sg.d3d11.in_pass && yk___sg.d3d11.ctx);
  yk___sg.d3d11.in_pass = false;
  /* need to resolve MSAA render target into texture? */
  if (yk___sg.d3d11.cur_pass) {
    YK__SOKOL_ASSERT(yk___sg.d3d11.cur_pass->slot.id ==
                     yk___sg.d3d11.cur_pass_id.id);
    for (int i = 0; i < yk___sg.d3d11.num_rtvs; i++) {
      yk___sg_pass_attachment_t *cmn_att =
          &yk___sg.d3d11.cur_pass->cmn.color_atts[i];
      yk___sg_image_t *att_img =
          yk___sg.d3d11.cur_pass->d3d11.color_atts[i].image;
      YK__SOKOL_ASSERT(att_img && (att_img->slot.id == cmn_att->image_id.id));
      if (att_img->cmn.sample_count > 1) {
        /* FIXME: support MSAA resolve into 3D texture */
        YK__SOKOL_ASSERT(att_img->d3d11.tex2d && att_img->d3d11.texmsaa &&
                         !att_img->d3d11.tex3d);
        YK__SOKOL_ASSERT(DXGI_FORMAT_UNKNOWN != att_img->d3d11.format);
        UINT dst_subres = yk___sg_d3d11_calcsubresource(
            (UINT) cmn_att->mip_level, (UINT) cmn_att->slice,
            (UINT) att_img->cmn.num_mipmaps);
        yk___sg_d3d11_ResolveSubresource(
            yk___sg.d3d11.ctx,
            (ID3D11Resource *) att_img->d3d11.tex2d,   /* pDstResource */
            dst_subres,                                /* DstSubresource */
            (ID3D11Resource *) att_img->d3d11.texmsaa, /* pSrcResource */
            0,                                         /* SrcSubresource */
            att_img->d3d11.format);
      }
    }
  }
  yk___sg.d3d11.cur_pass = 0;
  yk___sg.d3d11.cur_pass_id.id = YK__SG_INVALID_ID;
  yk___sg.d3d11.cur_pipeline = 0;
  yk___sg.d3d11.cur_pipeline_id.id = YK__SG_INVALID_ID;
  for (int i = 0; i < YK__SG_MAX_COLOR_ATTACHMENTS; i++) {
    yk___sg.d3d11.cur_rtvs[i] = 0;
  }
  yk___sg.d3d11.cur_dsv = 0;
  yk___sg_d3d11_clear_state();
}
YK___SOKOL_PRIVATE void yk___sg_d3d11_apply_viewport(int x, int y, int w, int h,
                                                     bool origin_top_left) {
  YK__SOKOL_ASSERT(yk___sg.d3d11.ctx);
  YK__SOKOL_ASSERT(yk___sg.d3d11.in_pass);
  D3D11_VIEWPORT vp;
  vp.TopLeftX = (FLOAT) x;
  vp.TopLeftY =
      (FLOAT) (origin_top_left ? y : (yk___sg.d3d11.cur_height - (y + h)));
  vp.Width = (FLOAT) w;
  vp.Height = (FLOAT) h;
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  yk___sg_d3d11_RSSetViewports(yk___sg.d3d11.ctx, 1, &vp);
}
YK___SOKOL_PRIVATE void yk___sg_d3d11_apply_scissor_rect(int x, int y, int w,
                                                         int h,
                                                         bool origin_top_left) {
  YK__SOKOL_ASSERT(yk___sg.d3d11.ctx);
  YK__SOKOL_ASSERT(yk___sg.d3d11.in_pass);
  D3D11_RECT rect;
  rect.left = x;
  rect.top = (origin_top_left ? y : (yk___sg.d3d11.cur_height - (y + h)));
  rect.right = x + w;
  rect.bottom = origin_top_left ? (y + h) : (yk___sg.d3d11.cur_height - y);
  yk___sg_d3d11_RSSetScissorRects(yk___sg.d3d11.ctx, 1, &rect);
}
YK___SOKOL_PRIVATE void yk___sg_d3d11_apply_pipeline(yk___sg_pipeline_t *pip) {
  YK__SOKOL_ASSERT(pip);
  YK__SOKOL_ASSERT(pip->shader &&
                   (pip->cmn.shader_id.id == pip->shader->slot.id));
  YK__SOKOL_ASSERT(yk___sg.d3d11.ctx);
  YK__SOKOL_ASSERT(yk___sg.d3d11.in_pass);
  YK__SOKOL_ASSERT(pip->d3d11.rs && pip->d3d11.bs && pip->d3d11.dss &&
                   pip->d3d11.il);
  yk___sg.d3d11.cur_pipeline = pip;
  yk___sg.d3d11.cur_pipeline_id.id = pip->slot.id;
  yk___sg.d3d11.use_indexed_draw =
      (pip->d3d11.index_format != DXGI_FORMAT_UNKNOWN);
  yk___sg.d3d11.use_instanced_draw = pip->cmn.use_instanced_draw;
  yk___sg_d3d11_RSSetState(yk___sg.d3d11.ctx, pip->d3d11.rs);
  yk___sg_d3d11_OMSetDepthStencilState(yk___sg.d3d11.ctx, pip->d3d11.dss,
                                       pip->d3d11.stencil_ref);
  yk___sg_d3d11_OMSetBlendState(yk___sg.d3d11.ctx, pip->d3d11.bs,
                                &pip->cmn.blend_color.r, 0xFFFFFFFF);
  yk___sg_d3d11_IASetPrimitiveTopology(yk___sg.d3d11.ctx, pip->d3d11.topology);
  yk___sg_d3d11_IASetInputLayout(yk___sg.d3d11.ctx, pip->d3d11.il);
  yk___sg_d3d11_VSSetShader(yk___sg.d3d11.ctx, pip->shader->d3d11.vs, NULL, 0);
  yk___sg_d3d11_VSSetConstantBuffers(
      yk___sg.d3d11.ctx, 0, YK__SG_MAX_SHADERSTAGE_UBS,
      pip->shader->d3d11.stage[YK__SG_SHADERSTAGE_VS].cbufs);
  yk___sg_d3d11_PSSetShader(yk___sg.d3d11.ctx, pip->shader->d3d11.fs, NULL, 0);
  yk___sg_d3d11_PSSetConstantBuffers(
      yk___sg.d3d11.ctx, 0, YK__SG_MAX_SHADERSTAGE_UBS,
      pip->shader->d3d11.stage[YK__SG_SHADERSTAGE_FS].cbufs);
}
YK___SOKOL_PRIVATE void yk___sg_d3d11_apply_bindings(
    yk___sg_pipeline_t *pip, yk___sg_buffer_t **vbs, const int *vb_offsets,
    int num_vbs, yk___sg_buffer_t *ib, int ib_offset, yk___sg_image_t **vs_imgs,
    int num_vs_imgs, yk___sg_image_t **fs_imgs, int num_fs_imgs) {
  YK__SOKOL_ASSERT(pip);
  YK__SOKOL_ASSERT(yk___sg.d3d11.ctx);
  YK__SOKOL_ASSERT(yk___sg.d3d11.in_pass);
  /* gather all the D3D11 resources into arrays */
  ID3D11Buffer *d3d11_ib = ib ? ib->d3d11.buf : 0;
  ID3D11Buffer *d3d11_vbs[YK__SG_MAX_SHADERSTAGE_BUFFERS];
  UINT d3d11_vb_offsets[YK__SG_MAX_SHADERSTAGE_BUFFERS];
  ID3D11ShaderResourceView *d3d11_vs_srvs[YK__SG_MAX_SHADERSTAGE_IMAGES];
  ID3D11SamplerState *d3d11_vs_smps[YK__SG_MAX_SHADERSTAGE_IMAGES];
  ID3D11ShaderResourceView *d3d11_fs_srvs[YK__SG_MAX_SHADERSTAGE_IMAGES];
  ID3D11SamplerState *d3d11_fs_smps[YK__SG_MAX_SHADERSTAGE_IMAGES];
  int i;
  for (i = 0; i < num_vbs; i++) {
    YK__SOKOL_ASSERT(vbs[i]->d3d11.buf);
    d3d11_vbs[i] = vbs[i]->d3d11.buf;
    d3d11_vb_offsets[i] = (UINT) vb_offsets[i];
  }
  for (; i < YK__SG_MAX_SHADERSTAGE_BUFFERS; i++) {
    d3d11_vbs[i] = 0;
    d3d11_vb_offsets[i] = 0;
  }
  for (i = 0; i < num_vs_imgs; i++) {
    YK__SOKOL_ASSERT(vs_imgs[i]->d3d11.srv);
    YK__SOKOL_ASSERT(vs_imgs[i]->d3d11.smp);
    d3d11_vs_srvs[i] = vs_imgs[i]->d3d11.srv;
    d3d11_vs_smps[i] = vs_imgs[i]->d3d11.smp;
  }
  for (; i < YK__SG_MAX_SHADERSTAGE_IMAGES; i++) {
    d3d11_vs_srvs[i] = 0;
    d3d11_vs_smps[i] = 0;
  }
  for (i = 0; i < num_fs_imgs; i++) {
    YK__SOKOL_ASSERT(fs_imgs[i]->d3d11.srv);
    YK__SOKOL_ASSERT(fs_imgs[i]->d3d11.smp);
    d3d11_fs_srvs[i] = fs_imgs[i]->d3d11.srv;
    d3d11_fs_smps[i] = fs_imgs[i]->d3d11.smp;
  }
  for (; i < YK__SG_MAX_SHADERSTAGE_IMAGES; i++) {
    d3d11_fs_srvs[i] = 0;
    d3d11_fs_smps[i] = 0;
  }
  yk___sg_d3d11_IASetVertexBuffers(yk___sg.d3d11.ctx, 0,
                                   YK__SG_MAX_SHADERSTAGE_BUFFERS, d3d11_vbs,
                                   pip->d3d11.vb_strides, d3d11_vb_offsets);
  yk___sg_d3d11_IASetIndexBuffer(yk___sg.d3d11.ctx, d3d11_ib,
                                 pip->d3d11.index_format, (UINT) ib_offset);
  yk___sg_d3d11_VSSetShaderResources(
      yk___sg.d3d11.ctx, 0, YK__SG_MAX_SHADERSTAGE_IMAGES, d3d11_vs_srvs);
  yk___sg_d3d11_VSSetSamplers(yk___sg.d3d11.ctx, 0,
                              YK__SG_MAX_SHADERSTAGE_IMAGES, d3d11_vs_smps);
  yk___sg_d3d11_PSSetShaderResources(
      yk___sg.d3d11.ctx, 0, YK__SG_MAX_SHADERSTAGE_IMAGES, d3d11_fs_srvs);
  yk___sg_d3d11_PSSetSamplers(yk___sg.d3d11.ctx, 0,
                              YK__SG_MAX_SHADERSTAGE_IMAGES, d3d11_fs_smps);
}
YK___SOKOL_PRIVATE void
yk___sg_d3d11_apply_uniforms(yk__sg_shader_stage stage_index, int ub_index,
                             const yk__sg_range *data) {
  YK__SOKOL_ASSERT(yk___sg.d3d11.ctx && yk___sg.d3d11.in_pass);
  YK__SOKOL_ASSERT(yk___sg.d3d11.cur_pipeline &&
                   yk___sg.d3d11.cur_pipeline->slot.id ==
                       yk___sg.d3d11.cur_pipeline_id.id);
  YK__SOKOL_ASSERT(yk___sg.d3d11.cur_pipeline->shader &&
                   yk___sg.d3d11.cur_pipeline->shader->slot.id ==
                       yk___sg.d3d11.cur_pipeline->cmn.shader_id.id);
  YK__SOKOL_ASSERT(ub_index <
                   yk___sg.d3d11.cur_pipeline->shader->cmn.stage[stage_index]
                       .num_uniform_blocks);
  YK__SOKOL_ASSERT(data->size ==
                   yk___sg.d3d11.cur_pipeline->shader->cmn.stage[stage_index]
                       .uniform_blocks[ub_index]
                       .size);
  ID3D11Buffer *cb =
      yk___sg.d3d11.cur_pipeline->shader->d3d11.stage[stage_index]
          .cbufs[ub_index];
  YK__SOKOL_ASSERT(cb);
  yk___sg_d3d11_UpdateSubresource(yk___sg.d3d11.ctx, (ID3D11Resource *) cb, 0,
                                  NULL, data->ptr, 0, 0);
}
YK___SOKOL_PRIVATE void yk___sg_d3d11_draw(int base_element, int num_elements,
                                           int num_instances) {
  YK__SOKOL_ASSERT(yk___sg.d3d11.in_pass);
  if (yk___sg.d3d11.use_indexed_draw) {
    if (yk___sg.d3d11.use_instanced_draw) {
      yk___sg_d3d11_DrawIndexedInstanced(yk___sg.d3d11.ctx, (UINT) num_elements,
                                         (UINT) num_instances,
                                         (UINT) base_element, 0, 0);
    } else {
      yk___sg_d3d11_DrawIndexed(yk___sg.d3d11.ctx, (UINT) num_elements,
                                (UINT) base_element, 0);
    }
  } else {
    if (yk___sg.d3d11.use_instanced_draw) {
      yk___sg_d3d11_DrawInstanced(yk___sg.d3d11.ctx, (UINT) num_elements,
                                  (UINT) num_instances, (UINT) base_element, 0);
    } else {
      yk___sg_d3d11_Draw(yk___sg.d3d11.ctx, (UINT) num_elements,
                         (UINT) base_element);
    }
  }
}
YK___SOKOL_PRIVATE void yk___sg_d3d11_commit(void) {
  YK__SOKOL_ASSERT(!yk___sg.d3d11.in_pass);
}
YK___SOKOL_PRIVATE void yk___sg_d3d11_update_buffer(yk___sg_buffer_t *buf,
                                                    const yk__sg_range *data) {
  YK__SOKOL_ASSERT(buf && data && data->ptr && (data->size > 0));
  YK__SOKOL_ASSERT(yk___sg.d3d11.ctx);
  YK__SOKOL_ASSERT(buf->d3d11.buf);
  D3D11_MAPPED_SUBRESOURCE d3d11_msr;
  HRESULT hr =
      yk___sg_d3d11_Map(yk___sg.d3d11.ctx, (ID3D11Resource *) buf->d3d11.buf, 0,
                        D3D11_MAP_WRITE_DISCARD, 0, &d3d11_msr);
  YK___SOKOL_UNUSED(hr);
  YK__SOKOL_ASSERT(SUCCEEDED(hr));
  memcpy(d3d11_msr.pData, data->ptr, data->size);
  yk___sg_d3d11_Unmap(yk___sg.d3d11.ctx, (ID3D11Resource *) buf->d3d11.buf, 0);
}
YK___SOKOL_PRIVATE int yk___sg_d3d11_append_buffer(yk___sg_buffer_t *buf,
                                                   const yk__sg_range *data,
                                                   bool new_frame) {
  YK__SOKOL_ASSERT(buf && data && data->ptr && (data->size > 0));
  YK__SOKOL_ASSERT(yk___sg.d3d11.ctx);
  YK__SOKOL_ASSERT(buf->d3d11.buf);
  D3D11_MAP map_type =
      new_frame ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE;
  D3D11_MAPPED_SUBRESOURCE d3d11_msr;
  HRESULT hr =
      yk___sg_d3d11_Map(yk___sg.d3d11.ctx, (ID3D11Resource *) buf->d3d11.buf, 0,
                        map_type, 0, &d3d11_msr);
  YK___SOKOL_UNUSED(hr);
  YK__SOKOL_ASSERT(SUCCEEDED(hr));
  uint8_t *dst_ptr = (uint8_t *) d3d11_msr.pData + buf->cmn.append_pos;
  memcpy(dst_ptr, data->ptr, data->size);
  yk___sg_d3d11_Unmap(yk___sg.d3d11.ctx, (ID3D11Resource *) buf->d3d11.buf, 0);
  /* NOTE: this is a requirement from WebGPU, but we want identical behaviour across all backend */
  return yk___sg_roundup((int) data->size, 4);
}
YK___SOKOL_PRIVATE void
yk___sg_d3d11_update_image(yk___sg_image_t *img,
                           const yk__sg_image_data *data) {
  YK__SOKOL_ASSERT(img && data);
  YK__SOKOL_ASSERT(yk___sg.d3d11.ctx);
  YK__SOKOL_ASSERT(img->d3d11.tex2d || img->d3d11.tex3d);
  ID3D11Resource *d3d11_res = 0;
  if (img->d3d11.tex3d) {
    d3d11_res = (ID3D11Resource *) img->d3d11.tex3d;
  } else {
    d3d11_res = (ID3D11Resource *) img->d3d11.tex2d;
  }
  YK__SOKOL_ASSERT(d3d11_res);
  const int num_faces = (img->cmn.type == YK__SG_IMAGETYPE_CUBE) ? 6 : 1;
  const int num_slices =
      (img->cmn.type == YK__SG_IMAGETYPE_ARRAY) ? img->cmn.num_slices : 1;
  UINT subres_index = 0;
  HRESULT hr;
  YK___SOKOL_UNUSED(hr);
  D3D11_MAPPED_SUBRESOURCE d3d11_msr;
  for (int face_index = 0; face_index < num_faces; face_index++) {
    for (int slice_index = 0; slice_index < num_slices; slice_index++) {
      for (int mip_index = 0; mip_index < img->cmn.num_mipmaps;
           mip_index++, subres_index++) {
        YK__SOKOL_ASSERT(subres_index <
                         (YK__SG_MAX_MIPMAPS * YK__SG_MAX_TEXTUREARRAY_LAYERS));
        const int mip_width = ((img->cmn.width >> mip_index) > 0)
                                  ? img->cmn.width >> mip_index
                                  : 1;
        const int mip_height = ((img->cmn.height >> mip_index) > 0)
                                   ? img->cmn.height >> mip_index
                                   : 1;
        const int src_pitch =
            yk___sg_row_pitch(img->cmn.pixel_format, mip_width, 1);
        const yk__sg_range *subimg_data =
            &(data->subimage[face_index][mip_index]);
        const size_t slice_size = subimg_data->size / (size_t) num_slices;
        const size_t slice_offset = slice_size * (size_t) slice_index;
        const uint8_t *slice_ptr =
            ((const uint8_t *) subimg_data->ptr) + slice_offset;
        hr = yk___sg_d3d11_Map(yk___sg.d3d11.ctx, d3d11_res, subres_index,
                               D3D11_MAP_WRITE_DISCARD, 0, &d3d11_msr);
        YK__SOKOL_ASSERT(SUCCEEDED(hr));
        /* FIXME: need to handle difference in depth-pitch for 3D textures as well! */
        if (src_pitch == (int) d3d11_msr.RowPitch) {
          memcpy(d3d11_msr.pData, slice_ptr, slice_size);
        } else {
          YK__SOKOL_ASSERT(src_pitch < (int) d3d11_msr.RowPitch);
          const uint8_t *src_ptr = slice_ptr;
          uint8_t *dst_ptr = (uint8_t *) d3d11_msr.pData;
          for (int row_index = 0; row_index < mip_height; row_index++) {
            memcpy(dst_ptr, src_ptr, (size_t) src_pitch);
            src_ptr += src_pitch;
            dst_ptr += d3d11_msr.RowPitch;
          }
        }
        yk___sg_d3d11_Unmap(yk___sg.d3d11.ctx, d3d11_res, subres_index);
      }
    }
  }
}
/*== METAL BACKEND IMPLEMENTATION ============================================*/
#elif defined(YK__SOKOL_METAL)
#if __has_feature(objc_arc)
#define YK___SG_OBJC_RETAIN(obj)                                               \
  {}
#define YK___SG_OBJC_RELEASE(obj)                                              \
  { obj = nil; }
#define YK___SG_OBJC_RELEASE_WITH_NULL(obj)                                    \
  { obj = [NSNull null]; }
#else
#define YK___SG_OBJC_RETAIN(obj)                                               \
  { [obj retain]; }
#define YK___SG_OBJC_RELEASE(obj)                                              \
  {                                                                            \
    [obj release];                                                             \
    obj = nil;                                                                 \
  }
#define YK___SG_OBJC_RELEASE_WITH_NULL(obj)                                    \
  {                                                                            \
    [obj release];                                                             \
    obj = [NSNull null];                                                       \
  }
#endif
/*-- enum translation functions ----------------------------------------------*/
YK___SOKOL_PRIVATE MTLLoadAction yk___sg_mtl_load_action(yk__sg_action a) {
  switch (a) {
    case YK__SG_ACTION_CLEAR:
      return MTLLoadActionClear;
    case YK__SG_ACTION_LOAD:
      return MTLLoadActionLoad;
    case YK__SG_ACTION_DONTCARE:
      return MTLLoadActionDontCare;
    default:
      YK__SOKOL_UNREACHABLE;
      return (MTLLoadAction) 0;
  }
}
YK___SOKOL_PRIVATE MTLResourceOptions
yk___sg_mtl_buffer_resource_options(yk__sg_usage yk__usg) {
  switch (yk__usg) {
    case YK__SG_USAGE_IMMUTABLE:
#if defined(YK___SG_TARGET_MACOS)
      return MTLResourceStorageModeManaged;
#else
      return MTLResourceStorageModeShared;
#endif
    case YK__SG_USAGE_DYNAMIC:
    case YK__SG_USAGE_STREAM:
#if defined(YK___SG_TARGET_MACOS)
      return MTLResourceCPUCacheModeWriteCombined |
             MTLResourceStorageModeManaged;
#else
      return MTLResourceCPUCacheModeWriteCombined |
             MTLResourceStorageModeShared;
#endif
    default:
      YK__SOKOL_UNREACHABLE;
      return 0;
  }
}
YK___SOKOL_PRIVATE MTLVertexStepFunction
yk___sg_mtl_step_function(yk__sg_vertex_step step) {
  switch (step) {
    case YK__SG_VERTEXSTEP_PER_VERTEX:
      return MTLVertexStepFunctionPerVertex;
    case YK__SG_VERTEXSTEP_PER_INSTANCE:
      return MTLVertexStepFunctionPerInstance;
    default:
      YK__SOKOL_UNREACHABLE;
      return (MTLVertexStepFunction) 0;
  }
}
YK___SOKOL_PRIVATE MTLVertexFormat
yk___sg_mtl_vertex_format(yk__sg_vertex_format fmt) {
  switch (fmt) {
    case YK__SG_VERTEXFORMAT_FLOAT:
      return MTLVertexFormatFloat;
    case YK__SG_VERTEXFORMAT_FLOAT2:
      return MTLVertexFormatFloat2;
    case YK__SG_VERTEXFORMAT_FLOAT3:
      return MTLVertexFormatFloat3;
    case YK__SG_VERTEXFORMAT_FLOAT4:
      return MTLVertexFormatFloat4;
    case YK__SG_VERTEXFORMAT_BYTE4:
      return MTLVertexFormatChar4;
    case YK__SG_VERTEXFORMAT_BYTE4N:
      return MTLVertexFormatChar4Normalized;
    case YK__SG_VERTEXFORMAT_UBYTE4:
      return MTLVertexFormatUChar4;
    case YK__SG_VERTEXFORMAT_UBYTE4N:
      return MTLVertexFormatUChar4Normalized;
    case YK__SG_VERTEXFORMAT_SHORT2:
      return MTLVertexFormatShort2;
    case YK__SG_VERTEXFORMAT_SHORT2N:
      return MTLVertexFormatShort2Normalized;
    case YK__SG_VERTEXFORMAT_USHORT2N:
      return MTLVertexFormatUShort2Normalized;
    case YK__SG_VERTEXFORMAT_SHORT4:
      return MTLVertexFormatShort4;
    case YK__SG_VERTEXFORMAT_SHORT4N:
      return MTLVertexFormatShort4Normalized;
    case YK__SG_VERTEXFORMAT_USHORT4N:
      return MTLVertexFormatUShort4Normalized;
    case YK__SG_VERTEXFORMAT_UINT10_N2:
      return MTLVertexFormatUInt1010102Normalized;
    default:
      YK__SOKOL_UNREACHABLE;
      return (MTLVertexFormat) 0;
  }
}
YK___SOKOL_PRIVATE MTLPrimitiveType
yk___sg_mtl_primitive_type(yk__sg_primitive_type t) {
  switch (t) {
    case YK__SG_PRIMITIVETYPE_POINTS:
      return MTLPrimitiveTypePoint;
    case YK__SG_PRIMITIVETYPE_LINES:
      return MTLPrimitiveTypeLine;
    case YK__SG_PRIMITIVETYPE_LINE_STRIP:
      return MTLPrimitiveTypeLineStrip;
    case YK__SG_PRIMITIVETYPE_TRIANGLES:
      return MTLPrimitiveTypeTriangle;
    case YK__SG_PRIMITIVETYPE_TRIANGLE_STRIP:
      return MTLPrimitiveTypeTriangleStrip;
    default:
      YK__SOKOL_UNREACHABLE;
      return (MTLPrimitiveType) 0;
  }
}
YK___SOKOL_PRIVATE MTLPixelFormat
yk___sg_mtl_pixel_format(yk__sg_pixel_format fmt) {
  switch (fmt) {
    case YK__SG_PIXELFORMAT_R8:
      return MTLPixelFormatR8Unorm;
    case YK__SG_PIXELFORMAT_R8SN:
      return MTLPixelFormatR8Snorm;
    case YK__SG_PIXELFORMAT_R8UI:
      return MTLPixelFormatR8Uint;
    case YK__SG_PIXELFORMAT_R8SI:
      return MTLPixelFormatR8Sint;
    case YK__SG_PIXELFORMAT_R16:
      return MTLPixelFormatR16Unorm;
    case YK__SG_PIXELFORMAT_R16SN:
      return MTLPixelFormatR16Snorm;
    case YK__SG_PIXELFORMAT_R16UI:
      return MTLPixelFormatR16Uint;
    case YK__SG_PIXELFORMAT_R16SI:
      return MTLPixelFormatR16Sint;
    case YK__SG_PIXELFORMAT_R16F:
      return MTLPixelFormatR16Float;
    case YK__SG_PIXELFORMAT_RG8:
      return MTLPixelFormatRG8Unorm;
    case YK__SG_PIXELFORMAT_RG8SN:
      return MTLPixelFormatRG8Snorm;
    case YK__SG_PIXELFORMAT_RG8UI:
      return MTLPixelFormatRG8Uint;
    case YK__SG_PIXELFORMAT_RG8SI:
      return MTLPixelFormatRG8Sint;
    case YK__SG_PIXELFORMAT_R32UI:
      return MTLPixelFormatR32Uint;
    case YK__SG_PIXELFORMAT_R32SI:
      return MTLPixelFormatR32Sint;
    case YK__SG_PIXELFORMAT_R32F:
      return MTLPixelFormatR32Float;
    case YK__SG_PIXELFORMAT_RG16:
      return MTLPixelFormatRG16Unorm;
    case YK__SG_PIXELFORMAT_RG16SN:
      return MTLPixelFormatRG16Snorm;
    case YK__SG_PIXELFORMAT_RG16UI:
      return MTLPixelFormatRG16Uint;
    case YK__SG_PIXELFORMAT_RG16SI:
      return MTLPixelFormatRG16Sint;
    case YK__SG_PIXELFORMAT_RG16F:
      return MTLPixelFormatRG16Float;
    case YK__SG_PIXELFORMAT_RGBA8:
      return MTLPixelFormatRGBA8Unorm;
    case YK__SG_PIXELFORMAT_RGBA8SN:
      return MTLPixelFormatRGBA8Snorm;
    case YK__SG_PIXELFORMAT_RGBA8UI:
      return MTLPixelFormatRGBA8Uint;
    case YK__SG_PIXELFORMAT_RGBA8SI:
      return MTLPixelFormatRGBA8Sint;
    case YK__SG_PIXELFORMAT_BGRA8:
      return MTLPixelFormatBGRA8Unorm;
    case YK__SG_PIXELFORMAT_RGB10A2:
      return MTLPixelFormatRGB10A2Unorm;
    case YK__SG_PIXELFORMAT_RG11B10F:
      return MTLPixelFormatRG11B10Float;
    case YK__SG_PIXELFORMAT_RG32UI:
      return MTLPixelFormatRG32Uint;
    case YK__SG_PIXELFORMAT_RG32SI:
      return MTLPixelFormatRG32Sint;
    case YK__SG_PIXELFORMAT_RG32F:
      return MTLPixelFormatRG32Float;
    case YK__SG_PIXELFORMAT_RGBA16:
      return MTLPixelFormatRGBA16Unorm;
    case YK__SG_PIXELFORMAT_RGBA16SN:
      return MTLPixelFormatRGBA16Snorm;
    case YK__SG_PIXELFORMAT_RGBA16UI:
      return MTLPixelFormatRGBA16Uint;
    case YK__SG_PIXELFORMAT_RGBA16SI:
      return MTLPixelFormatRGBA16Sint;
    case YK__SG_PIXELFORMAT_RGBA16F:
      return MTLPixelFormatRGBA16Float;
    case YK__SG_PIXELFORMAT_RGBA32UI:
      return MTLPixelFormatRGBA32Uint;
    case YK__SG_PIXELFORMAT_RGBA32SI:
      return MTLPixelFormatRGBA32Sint;
    case YK__SG_PIXELFORMAT_RGBA32F:
      return MTLPixelFormatRGBA32Float;
    case YK__SG_PIXELFORMAT_DEPTH:
      return MTLPixelFormatDepth32Float;
    case YK__SG_PIXELFORMAT_DEPTH_STENCIL:
      return MTLPixelFormatDepth32Float_Stencil8;
#if defined(YK___SG_TARGET_MACOS)
    case YK__SG_PIXELFORMAT_BC1_RGBA:
      return MTLPixelFormatBC1_RGBA;
    case YK__SG_PIXELFORMAT_BC2_RGBA:
      return MTLPixelFormatBC2_RGBA;
    case YK__SG_PIXELFORMAT_BC3_RGBA:
      return MTLPixelFormatBC3_RGBA;
    case YK__SG_PIXELFORMAT_BC4_R:
      return MTLPixelFormatBC4_RUnorm;
    case YK__SG_PIXELFORMAT_BC4_RSN:
      return MTLPixelFormatBC4_RSnorm;
    case YK__SG_PIXELFORMAT_BC5_RG:
      return MTLPixelFormatBC5_RGUnorm;
    case YK__SG_PIXELFORMAT_BC5_RGSN:
      return MTLPixelFormatBC5_RGSnorm;
    case YK__SG_PIXELFORMAT_BC6H_RGBF:
      return MTLPixelFormatBC6H_RGBFloat;
    case YK__SG_PIXELFORMAT_BC6H_RGBUF:
      return MTLPixelFormatBC6H_RGBUfloat;
    case YK__SG_PIXELFORMAT_BC7_RGBA:
      return MTLPixelFormatBC7_RGBAUnorm;
#else
    case YK__SG_PIXELFORMAT_PVRTC_RGB_2BPP:
      return MTLPixelFormatPVRTC_RGB_2BPP;
    case YK__SG_PIXELFORMAT_PVRTC_RGB_4BPP:
      return MTLPixelFormatPVRTC_RGB_4BPP;
    case YK__SG_PIXELFORMAT_PVRTC_RGBA_2BPP:
      return MTLPixelFormatPVRTC_RGBA_2BPP;
    case YK__SG_PIXELFORMAT_PVRTC_RGBA_4BPP:
      return MTLPixelFormatPVRTC_RGBA_4BPP;
    case YK__SG_PIXELFORMAT_ETC2_RGB8:
      return MTLPixelFormatETC2_RGB8;
    case YK__SG_PIXELFORMAT_ETC2_RGB8A1:
      return MTLPixelFormatETC2_RGB8A1;
    case YK__SG_PIXELFORMAT_ETC2_RGBA8:
      return MTLPixelFormatEAC_RGBA8;
    case YK__SG_PIXELFORMAT_ETC2_RG11:
      return MTLPixelFormatEAC_RG11Unorm;
    case YK__SG_PIXELFORMAT_ETC2_RG11SN:
      return MTLPixelFormatEAC_RG11Snorm;
#endif
    default:
      return MTLPixelFormatInvalid;
  }
}
YK___SOKOL_PRIVATE MTLColorWriteMask
yk___sg_mtl_color_write_mask(yk__sg_color_mask m) {
  MTLColorWriteMask mtl_mask = MTLColorWriteMaskNone;
  if (m & YK__SG_COLORMASK_R) { mtl_mask |= MTLColorWriteMaskRed; }
  if (m & YK__SG_COLORMASK_G) { mtl_mask |= MTLColorWriteMaskGreen; }
  if (m & YK__SG_COLORMASK_B) { mtl_mask |= MTLColorWriteMaskBlue; }
  if (m & YK__SG_COLORMASK_A) { mtl_mask |= MTLColorWriteMaskAlpha; }
  return mtl_mask;
}
YK___SOKOL_PRIVATE MTLBlendOperation yk___sg_mtl_blend_op(yk__sg_blend_op op) {
  switch (op) {
    case YK__SG_BLENDOP_ADD:
      return MTLBlendOperationAdd;
    case YK__SG_BLENDOP_SUBTRACT:
      return MTLBlendOperationSubtract;
    case YK__SG_BLENDOP_REVERSE_SUBTRACT:
      return MTLBlendOperationReverseSubtract;
    default:
      YK__SOKOL_UNREACHABLE;
      return (MTLBlendOperation) 0;
  }
}
YK___SOKOL_PRIVATE MTLBlendFactor
yk___sg_mtl_blend_factor(yk__sg_blend_factor f) {
  switch (f) {
    case YK__SG_BLENDFACTOR_ZERO:
      return MTLBlendFactorZero;
    case YK__SG_BLENDFACTOR_ONE:
      return MTLBlendFactorOne;
    case YK__SG_BLENDFACTOR_SRC_COLOR:
      return MTLBlendFactorSourceColor;
    case YK__SG_BLENDFACTOR_ONE_MINUS_SRC_COLOR:
      return MTLBlendFactorOneMinusSourceColor;
    case YK__SG_BLENDFACTOR_SRC_ALPHA:
      return MTLBlendFactorSourceAlpha;
    case YK__SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA:
      return MTLBlendFactorOneMinusSourceAlpha;
    case YK__SG_BLENDFACTOR_DST_COLOR:
      return MTLBlendFactorDestinationColor;
    case YK__SG_BLENDFACTOR_ONE_MINUS_DST_COLOR:
      return MTLBlendFactorOneMinusDestinationColor;
    case YK__SG_BLENDFACTOR_DST_ALPHA:
      return MTLBlendFactorDestinationAlpha;
    case YK__SG_BLENDFACTOR_ONE_MINUS_DST_ALPHA:
      return MTLBlendFactorOneMinusDestinationAlpha;
    case YK__SG_BLENDFACTOR_SRC_ALPHA_SATURATED:
      return MTLBlendFactorSourceAlphaSaturated;
    case YK__SG_BLENDFACTOR_BLEND_COLOR:
      return MTLBlendFactorBlendColor;
    case YK__SG_BLENDFACTOR_ONE_MINUS_BLEND_COLOR:
      return MTLBlendFactorOneMinusBlendColor;
    case YK__SG_BLENDFACTOR_BLEND_ALPHA:
      return MTLBlendFactorBlendAlpha;
    case YK__SG_BLENDFACTOR_ONE_MINUS_BLEND_ALPHA:
      return MTLBlendFactorOneMinusBlendAlpha;
    default:
      YK__SOKOL_UNREACHABLE;
      return (MTLBlendFactor) 0;
  }
}
YK___SOKOL_PRIVATE MTLCompareFunction
yk___sg_mtl_compare_func(yk__sg_compare_func f) {
  switch (f) {
    case YK__SG_COMPAREFUNC_NEVER:
      return MTLCompareFunctionNever;
    case YK__SG_COMPAREFUNC_LESS:
      return MTLCompareFunctionLess;
    case YK__SG_COMPAREFUNC_EQUAL:
      return MTLCompareFunctionEqual;
    case YK__SG_COMPAREFUNC_LESS_EQUAL:
      return MTLCompareFunctionLessEqual;
    case YK__SG_COMPAREFUNC_GREATER:
      return MTLCompareFunctionGreater;
    case YK__SG_COMPAREFUNC_NOT_EQUAL:
      return MTLCompareFunctionNotEqual;
    case YK__SG_COMPAREFUNC_GREATER_EQUAL:
      return MTLCompareFunctionGreaterEqual;
    case YK__SG_COMPAREFUNC_ALWAYS:
      return MTLCompareFunctionAlways;
    default:
      YK__SOKOL_UNREACHABLE;
      return (MTLCompareFunction) 0;
  }
}
YK___SOKOL_PRIVATE MTLStencilOperation
yk___sg_mtl_stencil_op(yk__sg_stencil_op op) {
  switch (op) {
    case YK__SG_STENCILOP_KEEP:
      return MTLStencilOperationKeep;
    case YK__SG_STENCILOP_ZERO:
      return MTLStencilOperationZero;
    case YK__SG_STENCILOP_REPLACE:
      return MTLStencilOperationReplace;
    case YK__SG_STENCILOP_INCR_CLAMP:
      return MTLStencilOperationIncrementClamp;
    case YK__SG_STENCILOP_DECR_CLAMP:
      return MTLStencilOperationDecrementClamp;
    case YK__SG_STENCILOP_INVERT:
      return MTLStencilOperationInvert;
    case YK__SG_STENCILOP_INCR_WRAP:
      return MTLStencilOperationIncrementWrap;
    case YK__SG_STENCILOP_DECR_WRAP:
      return MTLStencilOperationDecrementWrap;
    default:
      YK__SOKOL_UNREACHABLE;
      return (MTLStencilOperation) 0;
  }
}
YK___SOKOL_PRIVATE MTLCullMode yk___sg_mtl_cull_mode(yk__sg_cull_mode m) {
  switch (m) {
    case YK__SG_CULLMODE_NONE:
      return MTLCullModeNone;
    case YK__SG_CULLMODE_FRONT:
      return MTLCullModeFront;
    case YK__SG_CULLMODE_BACK:
      return MTLCullModeBack;
    default:
      YK__SOKOL_UNREACHABLE;
      return (MTLCullMode) 0;
  }
}
YK___SOKOL_PRIVATE MTLWinding yk___sg_mtl_winding(yk__sg_face_winding w) {
  switch (w) {
    case YK__SG_FACEWINDING_CW:
      return MTLWindingClockwise;
    case YK__SG_FACEWINDING_CCW:
      return MTLWindingCounterClockwise;
    default:
      YK__SOKOL_UNREACHABLE;
      return (MTLWinding) 0;
  }
}
YK___SOKOL_PRIVATE MTLIndexType yk___sg_mtl_index_type(yk__sg_index_type t) {
  switch (t) {
    case YK__SG_INDEXTYPE_UINT16:
      return MTLIndexTypeUInt16;
    case YK__SG_INDEXTYPE_UINT32:
      return MTLIndexTypeUInt32;
    default:
      YK__SOKOL_UNREACHABLE;
      return (MTLIndexType) 0;
  }
}
YK___SOKOL_PRIVATE int yk___sg_mtl_index_size(yk__sg_index_type t) {
  switch (t) {
    case YK__SG_INDEXTYPE_NONE:
      return 0;
    case YK__SG_INDEXTYPE_UINT16:
      return 2;
    case YK__SG_INDEXTYPE_UINT32:
      return 4;
    default:
      YK__SOKOL_UNREACHABLE;
      return 0;
  }
}
YK___SOKOL_PRIVATE MTLTextureType
yk___sg_mtl_texture_type(yk__sg_image_type t) {
  switch (t) {
    case YK__SG_IMAGETYPE_2D:
      return MTLTextureType2D;
    case YK__SG_IMAGETYPE_CUBE:
      return MTLTextureTypeCube;
    case YK__SG_IMAGETYPE_3D:
      return MTLTextureType3D;
    case YK__SG_IMAGETYPE_ARRAY:
      return MTLTextureType2DArray;
    default:
      YK__SOKOL_UNREACHABLE;
      return (MTLTextureType) 0;
  }
}
YK___SOKOL_PRIVATE bool yk___sg_mtl_is_pvrtc(yk__sg_pixel_format fmt) {
  switch (fmt) {
    case YK__SG_PIXELFORMAT_PVRTC_RGB_2BPP:
    case YK__SG_PIXELFORMAT_PVRTC_RGB_4BPP:
    case YK__SG_PIXELFORMAT_PVRTC_RGBA_2BPP:
    case YK__SG_PIXELFORMAT_PVRTC_RGBA_4BPP:
      return true;
    default:
      return false;
  }
}
YK___SOKOL_PRIVATE MTLSamplerAddressMode
yk___sg_mtl_address_mode(yk__sg_wrap w) {
  switch (w) {
    case YK__SG_WRAP_REPEAT:
      return MTLSamplerAddressModeRepeat;
    case YK__SG_WRAP_CLAMP_TO_EDGE:
      return MTLSamplerAddressModeClampToEdge;
#if defined(YK___SG_TARGET_MACOS)
    case YK__SG_WRAP_CLAMP_TO_BORDER:
      return MTLSamplerAddressModeClampToBorderColor;
#else
    /* clamp-to-border not supported on iOS, fall back to clamp-to-edge */
    case YK__SG_WRAP_CLAMP_TO_BORDER:
      return MTLSamplerAddressModeClampToEdge;
#endif
    case YK__SG_WRAP_MIRRORED_REPEAT:
      return MTLSamplerAddressModeMirrorRepeat;
    default:
      YK__SOKOL_UNREACHABLE;
      return (MTLSamplerAddressMode) 0;
  }
}
#if defined(YK___SG_TARGET_MACOS)
YK___SOKOL_PRIVATE MTLSamplerBorderColor
yk___sg_mtl_border_color(yk__sg_border_color c) {
  switch (c) {
    case YK__SG_BORDERCOLOR_TRANSPARENT_BLACK:
      return MTLSamplerBorderColorTransparentBlack;
    case YK__SG_BORDERCOLOR_OPAQUE_BLACK:
      return MTLSamplerBorderColorOpaqueBlack;
    case YK__SG_BORDERCOLOR_OPAQUE_WHITE:
      return MTLSamplerBorderColorOpaqueWhite;
    default:
      YK__SOKOL_UNREACHABLE;
      return (MTLSamplerBorderColor) 0;
  }
}
#endif
YK___SOKOL_PRIVATE MTLSamplerMinMagFilter
yk___sg_mtl_minmag_filter(yk__sg_filter f) {
  switch (f) {
    case YK__SG_FILTER_NEAREST:
    case YK__SG_FILTER_NEAREST_MIPMAP_NEAREST:
    case YK__SG_FILTER_NEAREST_MIPMAP_LINEAR:
      return MTLSamplerMinMagFilterNearest;
    case YK__SG_FILTER_LINEAR:
    case YK__SG_FILTER_LINEAR_MIPMAP_NEAREST:
    case YK__SG_FILTER_LINEAR_MIPMAP_LINEAR:
      return MTLSamplerMinMagFilterLinear;
    default:
      YK__SOKOL_UNREACHABLE;
      return (MTLSamplerMinMagFilter) 0;
  }
}
YK___SOKOL_PRIVATE MTLSamplerMipFilter yk___sg_mtl_mip_filter(yk__sg_filter f) {
  switch (f) {
    case YK__SG_FILTER_NEAREST:
    case YK__SG_FILTER_LINEAR:
      return MTLSamplerMipFilterNotMipmapped;
    case YK__SG_FILTER_NEAREST_MIPMAP_NEAREST:
    case YK__SG_FILTER_LINEAR_MIPMAP_NEAREST:
      return MTLSamplerMipFilterNearest;
    case YK__SG_FILTER_NEAREST_MIPMAP_LINEAR:
    case YK__SG_FILTER_LINEAR_MIPMAP_LINEAR:
      return MTLSamplerMipFilterLinear;
    default:
      YK__SOKOL_UNREACHABLE;
      return (MTLSamplerMipFilter) 0;
  }
}
/*-- a pool for all Metal resource objects, with deferred release queue -------*/
YK___SOKOL_PRIVATE void yk___sg_mtl_init_pool(const yk__sg_desc *desc) {
  yk___sg.mtl.idpool.num_slots =
      2 * (2 * desc->buffer_pool_size + 5 * desc->image_pool_size +
           4 * desc->shader_pool_size + 2 * desc->pipeline_pool_size +
           desc->pass_pool_size);
  yk___sg.mtl.idpool.pool = [NSMutableArray
      arrayWithCapacity:(NSUInteger) yk___sg.mtl.idpool.num_slots];
  YK___SG_OBJC_RETAIN(yk___sg.mtl.idpool.pool);
  NSNull *null = [NSNull null];
  for (int i = 0; i < yk___sg.mtl.idpool.num_slots; i++) {
    [yk___sg.mtl.idpool.pool addObject:null];
  }
  YK__SOKOL_ASSERT([yk___sg.mtl.idpool.pool count] ==
                   (NSUInteger) yk___sg.mtl.idpool.num_slots);
  /* a queue of currently free slot indices */
  yk___sg.mtl.idpool.free_queue_top = 0;
  yk___sg.mtl.idpool.free_queue = (int *) YK__SOKOL_MALLOC(
      (size_t) yk___sg.mtl.idpool.num_slots * sizeof(int));
  /* pool slot 0 is reserved! */
  for (int i = yk___sg.mtl.idpool.num_slots - 1; i >= 1; i--) {
    yk___sg.mtl.idpool.free_queue[yk___sg.mtl.idpool.free_queue_top++] = i;
  }
  /* a circular queue which holds release items (frame index
       when a resource is to be released, and the resource's
       pool index
    */
  yk___sg.mtl.idpool.release_queue_front = 0;
  yk___sg.mtl.idpool.release_queue_back = 0;
  yk___sg.mtl.idpool.release_queue =
      (yk___sg_mtl_release_item_t *) YK__SOKOL_MALLOC(
          (size_t) yk___sg.mtl.idpool.num_slots *
          sizeof(yk___sg_mtl_release_item_t));
  for (int i = 0; i < yk___sg.mtl.idpool.num_slots; i++) {
    yk___sg.mtl.idpool.release_queue[i].frame_index = 0;
    yk___sg.mtl.idpool.release_queue[i].slot_index =
        YK___SG_MTL_INVALID_SLOT_INDEX;
  }
}
YK___SOKOL_PRIVATE void yk___sg_mtl_destroy_pool(void) {
  YK__SOKOL_FREE(yk___sg.mtl.idpool.release_queue);
  yk___sg.mtl.idpool.release_queue = 0;
  YK__SOKOL_FREE(yk___sg.mtl.idpool.free_queue);
  yk___sg.mtl.idpool.free_queue = 0;
  YK___SG_OBJC_RELEASE(yk___sg.mtl.idpool.pool);
}
/* get a new free resource pool slot */
YK___SOKOL_PRIVATE int yk___sg_mtl_alloc_pool_slot(void) {
  YK__SOKOL_ASSERT(yk___sg.mtl.idpool.free_queue_top > 0);
  const int slot_index =
      yk___sg.mtl.idpool.free_queue[--yk___sg.mtl.idpool.free_queue_top];
  YK__SOKOL_ASSERT((slot_index > 0) &&
                   (slot_index < yk___sg.mtl.idpool.num_slots));
  return slot_index;
}
/* put a free resource pool slot back into the free-queue */
YK___SOKOL_PRIVATE void yk___sg_mtl_free_pool_slot(int slot_index) {
  YK__SOKOL_ASSERT(yk___sg.mtl.idpool.free_queue_top <
                   yk___sg.mtl.idpool.num_slots);
  YK__SOKOL_ASSERT((slot_index > 0) &&
                   (slot_index < yk___sg.mtl.idpool.num_slots));
  yk___sg.mtl.idpool.free_queue[yk___sg.mtl.idpool.free_queue_top++] =
      slot_index;
}
/*  add an MTLResource to the pool, return pool index or 0 if input was 'nil' */
YK___SOKOL_PRIVATE int yk___sg_mtl_add_resource(id res) {
  if (nil == res) { return YK___SG_MTL_INVALID_SLOT_INDEX; }
  const int slot_index = yk___sg_mtl_alloc_pool_slot();
  YK__SOKOL_ASSERT([NSNull null] ==
                   yk___sg.mtl.idpool.pool[(NSUInteger) slot_index]);
  yk___sg.mtl.idpool.pool[(NSUInteger) slot_index] = res;
  return slot_index;
}
/*  mark an MTLResource for release, this will put the resource into the
    deferred-release queue, and the resource will then be released N frames later,
    the special pool index 0 will be ignored (this means that a nil
    value was provided to yk___sg_mtl_add_resource()
*/
YK___SOKOL_PRIVATE void yk___sg_mtl_release_resource(uint32_t frame_index,
                                                     int slot_index) {
  if (slot_index == YK___SG_MTL_INVALID_SLOT_INDEX) { return; }
  YK__SOKOL_ASSERT((slot_index > 0) &&
                   (slot_index < yk___sg.mtl.idpool.num_slots));
  YK__SOKOL_ASSERT([NSNull null] !=
                   yk___sg.mtl.idpool.pool[(NSUInteger) slot_index]);
  int release_index = yk___sg.mtl.idpool.release_queue_front++;
  if (yk___sg.mtl.idpool.release_queue_front >= yk___sg.mtl.idpool.num_slots) {
    /* wrap-around */
    yk___sg.mtl.idpool.release_queue_front = 0;
  }
  /* release queue full? */
  YK__SOKOL_ASSERT(yk___sg.mtl.idpool.release_queue_front !=
                   yk___sg.mtl.idpool.release_queue_back);
  YK__SOKOL_ASSERT(0 ==
                   yk___sg.mtl.idpool.release_queue[release_index].frame_index);
  const uint32_t safe_to_release_frame_index =
      frame_index + YK__SG_NUM_INFLIGHT_FRAMES + 1;
  yk___sg.mtl.idpool.release_queue[release_index].frame_index =
      safe_to_release_frame_index;
  yk___sg.mtl.idpool.release_queue[release_index].slot_index = slot_index;
}
/* run garbage-collection pass on all resources in the release-queue */
YK___SOKOL_PRIVATE void yk___sg_mtl_garbage_collect(uint32_t frame_index) {
  while (yk___sg.mtl.idpool.release_queue_back !=
         yk___sg.mtl.idpool.release_queue_front) {
    if (frame_index <
        yk___sg.mtl.idpool.release_queue[yk___sg.mtl.idpool.release_queue_back]
            .frame_index) {
      /* don't need to check further, release-items past this are too young */
      break;
    }
    /* safe to release this resource */
    const int slot_index =
        yk___sg.mtl.idpool.release_queue[yk___sg.mtl.idpool.release_queue_back]
            .slot_index;
    YK__SOKOL_ASSERT((slot_index > 0) &&
                     (slot_index < yk___sg.mtl.idpool.num_slots));
    YK__SOKOL_ASSERT(yk___sg.mtl.idpool.pool[(NSUInteger) slot_index] !=
                     [NSNull null]);
    YK___SG_OBJC_RELEASE_WITH_NULL(
        yk___sg.mtl.idpool.pool[(NSUInteger) slot_index]);
    /* put the now free pool index back on the free queue */
    yk___sg_mtl_free_pool_slot(slot_index);
    /* reset the release queue slot and advance the back index */
    yk___sg.mtl.idpool.release_queue[yk___sg.mtl.idpool.release_queue_back]
        .frame_index = 0;
    yk___sg.mtl.idpool.release_queue[yk___sg.mtl.idpool.release_queue_back]
        .slot_index = YK___SG_MTL_INVALID_SLOT_INDEX;
    yk___sg.mtl.idpool.release_queue_back++;
    if (yk___sg.mtl.idpool.release_queue_back >= yk___sg.mtl.idpool.num_slots) {
      /* wrap-around */
      yk___sg.mtl.idpool.release_queue_back = 0;
    }
  }
}
YK___SOKOL_PRIVATE id yk___sg_mtl_id(int slot_index) {
  return yk___sg.mtl.idpool.pool[(NSUInteger) slot_index];
}
YK___SOKOL_PRIVATE void
yk___sg_mtl_init_sampler_cache(const yk__sg_desc *desc) {
  YK__SOKOL_ASSERT(desc->sampler_cache_size > 0);
  yk___sg_smpcache_init(&yk___sg.mtl.sampler_cache, desc->sampler_cache_size);
}
/* destroy the sampler cache, and release all sampler objects */
YK___SOKOL_PRIVATE void
yk___sg_mtl_destroy_sampler_cache(uint32_t frame_index) {
  YK__SOKOL_ASSERT(yk___sg.mtl.sampler_cache.items);
  YK__SOKOL_ASSERT(yk___sg.mtl.sampler_cache.num_items <=
                   yk___sg.mtl.sampler_cache.capacity);
  for (int i = 0; i < yk___sg.mtl.sampler_cache.num_items; i++) {
    yk___sg_mtl_release_resource(
        frame_index,
        (int) yk___sg_smpcache_sampler(&yk___sg.mtl.sampler_cache, i));
  }
  yk___sg_smpcache_discard(&yk___sg.mtl.sampler_cache);
}
/*
    create and add an MTLSamplerStateObject and return its resource pool index,
    reuse identical sampler state if one exists
*/
YK___SOKOL_PRIVATE int
yk___sg_mtl_create_sampler(id<MTLDevice> mtl_device,
                           const yk__sg_image_desc *img_desc) {
  YK__SOKOL_ASSERT(img_desc);
  int index = yk___sg_smpcache_find_item(&yk___sg.mtl.sampler_cache, img_desc);
  if (index >= 0) {
    /* reuse existing sampler */
    return (int) yk___sg_smpcache_sampler(&yk___sg.mtl.sampler_cache, index);
  } else {
    /* create a new Metal sampler state object and add to sampler cache */
    MTLSamplerDescriptor *mtl_desc = [[MTLSamplerDescriptor alloc] init];
    mtl_desc.sAddressMode = yk___sg_mtl_address_mode(img_desc->wrap_u);
    mtl_desc.tAddressMode = yk___sg_mtl_address_mode(img_desc->wrap_v);
    if (YK__SG_IMAGETYPE_3D == img_desc->type) {
      mtl_desc.rAddressMode = yk___sg_mtl_address_mode(img_desc->wrap_w);
    }
#if defined(YK___SG_TARGET_MACOS)
    mtl_desc.borderColor = yk___sg_mtl_border_color(img_desc->border_color);
#endif
    mtl_desc.minFilter = yk___sg_mtl_minmag_filter(img_desc->min_filter);
    mtl_desc.magFilter = yk___sg_mtl_minmag_filter(img_desc->mag_filter);
    mtl_desc.mipFilter = yk___sg_mtl_mip_filter(img_desc->min_filter);
    mtl_desc.lodMinClamp = img_desc->min_lod;
    mtl_desc.lodMaxClamp = img_desc->max_lod;
    mtl_desc.maxAnisotropy = img_desc->max_anisotropy;
    mtl_desc.normalizedCoordinates = YES;
    id<MTLSamplerState> mtl_sampler =
        [mtl_device newSamplerStateWithDescriptor:mtl_desc];
    YK___SG_OBJC_RELEASE(mtl_desc);
    int sampler_handle = yk___sg_mtl_add_resource(mtl_sampler);
    yk___sg_smpcache_add_item(&yk___sg.mtl.sampler_cache, img_desc,
                              (uintptr_t) sampler_handle);
    return sampler_handle;
  }
}
YK___SOKOL_PRIVATE void yk___sg_mtl_clear_state_cache(void) {
  memset(&yk___sg.mtl.state_cache, 0, sizeof(yk___sg.mtl.state_cache));
}
/* https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf */
YK___SOKOL_PRIVATE void yk___sg_mtl_init_caps(void) {
#if defined(YK___SG_TARGET_MACOS)
  yk___sg.backend = YK__SG_BACKEND_METAL_MACOS;
#elif defined(YK___SG_TARGET_IOS)
#if defined(YK___SG_TARGET_IOS_SIMULATOR)
  yk___sg.backend = YK__SG_BACKEND_METAL_SIMULATOR;
#else
  yk___sg.backend = YK__SG_BACKEND_METAL_IOS;
#endif
#endif
  yk___sg.features.instancing = true;
  yk___sg.features.origin_top_left = true;
  yk___sg.features.multiple_render_targets = true;
  yk___sg.features.msaa_render_targets = true;
  yk___sg.features.imagetype_3d = true;
  yk___sg.features.imagetype_array = true;
#if defined(YK___SG_TARGET_MACOS)
  yk___sg.features.image_clamp_to_border = true;
#else
  yk___sg.features.image_clamp_to_border = false;
#endif
  yk___sg.features.mrt_independent_blend_state = true;
  yk___sg.features.mrt_independent_write_mask = true;
#if defined(YK___SG_TARGET_MACOS)
  yk___sg.limits.max_image_size_2d = 16 * 1024;
  yk___sg.limits.max_image_size_cube = 16 * 1024;
  yk___sg.limits.max_image_size_3d = 2 * 1024;
  yk___sg.limits.max_image_size_array = 16 * 1024;
  yk___sg.limits.max_image_array_layers = 2 * 1024;
#else
  /* newer iOS devices support 16k textures */
  yk___sg.limits.max_image_size_2d = 8 * 1024;
  yk___sg.limits.max_image_size_cube = 8 * 1024;
  yk___sg.limits.max_image_size_3d = 2 * 1024;
  yk___sg.limits.max_image_size_array = 8 * 1024;
  yk___sg.limits.max_image_array_layers = 2 * 1024;
#endif
  yk___sg.limits.max_vertex_attrs = YK__SG_MAX_VERTEX_ATTRIBUTES;
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_R8]);
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_R8SN]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_R8UI]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_R8SI]);
#if defined(YK___SG_TARGET_MACOS)
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_R16]);
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_R16SN]);
#else
  yk___sg_pixelformat_sfbr(&yk___sg.formats[YK__SG_PIXELFORMAT_R16]);
  yk___sg_pixelformat_sfbr(&yk___sg.formats[YK__SG_PIXELFORMAT_R16SN]);
#endif
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_R16UI]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_R16SI]);
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_R16F]);
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RG8]);
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RG8SN]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RG8UI]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RG8SI]);
  yk___sg_pixelformat_sr(&yk___sg.formats[YK__SG_PIXELFORMAT_R32UI]);
  yk___sg_pixelformat_sr(&yk___sg.formats[YK__SG_PIXELFORMAT_R32SI]);
#if defined(YK___SG_TARGET_MACOS)
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_R32F]);
#else
  yk___sg_pixelformat_sbr(&yk___sg.formats[YK__SG_PIXELFORMAT_R32F]);
#endif
#if defined(YK___SG_TARGET_MACOS)
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RG16]);
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RG16SN]);
#else
  yk___sg_pixelformat_sfbr(&yk___sg.formats[YK__SG_PIXELFORMAT_RG16]);
  yk___sg_pixelformat_sfbr(&yk___sg.formats[YK__SG_PIXELFORMAT_RG16SN]);
#endif
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RG16UI]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RG16SI]);
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RG16F]);
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA8]);
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA8SN]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA8UI]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA8SI]);
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_BGRA8]);
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RGB10A2]);
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RG11B10F]);
#if defined(YK___SG_TARGET_MACOS)
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RG32UI]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RG32SI]);
#else
  yk___sg_pixelformat_sr(&yk___sg.formats[YK__SG_PIXELFORMAT_RG32UI]);
  yk___sg_pixelformat_sr(&yk___sg.formats[YK__SG_PIXELFORMAT_RG32SI]);
#endif
#if defined(YK___SG_TARGET_MACOS)
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RG32F]);
#else
  yk___sg_pixelformat_sbr(&yk___sg.formats[YK__SG_PIXELFORMAT_RG32F]);
#endif
#if defined(YK___SG_TARGET_MACOS)
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA16]);
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA16SN]);
#else
  yk___sg_pixelformat_sfbr(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA16]);
  yk___sg_pixelformat_sfbr(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA16SN]);
#endif
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA16UI]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA16SI]);
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA16F]);
#if defined(YK___SG_TARGET_MACOS)
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA32UI]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA32SI]);
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA32F]);
#else
  yk___sg_pixelformat_sr(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA32UI]);
  yk___sg_pixelformat_sr(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA32SI]);
  yk___sg_pixelformat_sr(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA32F]);
#endif
  yk___sg_pixelformat_srmd(&yk___sg.formats[YK__SG_PIXELFORMAT_DEPTH]);
  yk___sg_pixelformat_srmd(&yk___sg.formats[YK__SG_PIXELFORMAT_DEPTH_STENCIL]);
#if defined(YK___SG_TARGET_MACOS)
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC1_RGBA]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC2_RGBA]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC3_RGBA]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC4_R]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC4_RSN]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC5_RG]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC5_RGSN]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC6H_RGBF]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC6H_RGBUF]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC7_RGBA]);
#else
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_PVRTC_RGB_2BPP]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_PVRTC_RGB_4BPP]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_PVRTC_RGBA_2BPP]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_PVRTC_RGBA_4BPP]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_ETC2_RGB8]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_ETC2_RGB8A1]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_ETC2_RGBA8]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_ETC2_RG11]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_ETC2_RG11SN]);
#endif
}
/*-- main Metal backend state and functions ----------------------------------*/
YK___SOKOL_PRIVATE void yk___sg_mtl_setup_backend(const yk__sg_desc *desc) {
  /* assume already zero-initialized */
  YK__SOKOL_ASSERT(desc);
  YK__SOKOL_ASSERT(desc->context.metal.device);
  YK__SOKOL_ASSERT(desc->context.metal.renderpass_descriptor_cb ||
                   desc->context.metal.renderpass_descriptor_userdata_cb);
  YK__SOKOL_ASSERT(desc->context.metal.drawable_cb ||
                   desc->context.metal.drawable_userdata_cb);
  YK__SOKOL_ASSERT(desc->uniform_buffer_size > 0);
  yk___sg_mtl_init_pool(desc);
  yk___sg_mtl_init_sampler_cache(desc);
  yk___sg_mtl_clear_state_cache();
  yk___sg.mtl.valid = true;
  yk___sg.mtl.renderpass_descriptor_cb =
      desc->context.metal.renderpass_descriptor_cb;
  yk___sg.mtl.renderpass_descriptor_userdata_cb =
      desc->context.metal.renderpass_descriptor_userdata_cb;
  yk___sg.mtl.drawable_cb = desc->context.metal.drawable_cb;
  yk___sg.mtl.drawable_userdata_cb = desc->context.metal.drawable_userdata_cb;
  yk___sg.mtl.user_data = desc->context.metal.user_data;
  yk___sg.mtl.frame_index = 1;
  yk___sg.mtl.ub_size = desc->uniform_buffer_size;
  yk___sg.mtl.sem = dispatch_semaphore_create(YK__SG_NUM_INFLIGHT_FRAMES);
  yk___sg.mtl.device = (__bridge id<MTLDevice>) desc->context.metal.device;
  yk___sg.mtl.cmd_queue = [yk___sg.mtl.device newCommandQueue];
  for (int i = 0; i < YK__SG_NUM_INFLIGHT_FRAMES; i++) {
    yk___sg.mtl.uniform_buffers[i] = [yk___sg.mtl.device
        newBufferWithLength:(NSUInteger) yk___sg.mtl.ub_size
                    options:MTLResourceCPUCacheModeWriteCombined |
                            MTLResourceStorageModeShared];
  }
  yk___sg_mtl_init_caps();
}
YK___SOKOL_PRIVATE void yk___sg_mtl_discard_backend(void) {
  YK__SOKOL_ASSERT(yk___sg.mtl.valid);
  /* wait for the last frame to finish */
  for (int i = 0; i < YK__SG_NUM_INFLIGHT_FRAMES; i++) {
    dispatch_semaphore_wait(yk___sg.mtl.sem, DISPATCH_TIME_FOREVER);
  }
  /* semaphore must be "relinquished" before destruction */
  for (int i = 0; i < YK__SG_NUM_INFLIGHT_FRAMES; i++) {
    dispatch_semaphore_signal(yk___sg.mtl.sem);
  }
  yk___sg_mtl_destroy_sampler_cache(yk___sg.mtl.frame_index);
  yk___sg_mtl_garbage_collect(yk___sg.mtl.frame_index +
                              YK__SG_NUM_INFLIGHT_FRAMES + 2);
  yk___sg_mtl_destroy_pool();
  yk___sg.mtl.valid = false;
  YK___SG_OBJC_RELEASE(yk___sg.mtl.sem);
  YK___SG_OBJC_RELEASE(yk___sg.mtl.device);
  YK___SG_OBJC_RELEASE(yk___sg.mtl.cmd_queue);
  for (int i = 0; i < YK__SG_NUM_INFLIGHT_FRAMES; i++) {
    YK___SG_OBJC_RELEASE(yk___sg.mtl.uniform_buffers[i]);
  }
  /* NOTE: MTLCommandBuffer and MTLRenderCommandEncoder are auto-released */
  yk___sg.mtl.cmd_buffer = nil;
  yk___sg.mtl.cmd_encoder = nil;
}
YK___SOKOL_PRIVATE void yk___sg_mtl_bind_uniform_buffers(void) {
  YK__SOKOL_ASSERT(nil != yk___sg.mtl.cmd_encoder);
  for (int slot = 0; slot < YK__SG_MAX_SHADERSTAGE_UBS; slot++) {
    [yk___sg.mtl.cmd_encoder
        setVertexBuffer:yk___sg.mtl
                            .uniform_buffers[yk___sg.mtl.cur_frame_rotate_index]
                 offset:0
                atIndex:(NSUInteger) slot];
    [yk___sg.mtl.cmd_encoder
        setFragmentBuffer:yk___sg.mtl.uniform_buffers
                              [yk___sg.mtl.cur_frame_rotate_index]
                   offset:0
                  atIndex:(NSUInteger) slot];
  }
}
YK___SOKOL_PRIVATE void yk___sg_mtl_reset_state_cache(void) {
  yk___sg_mtl_clear_state_cache();
  /* need to restore the uniform buffer binding (normally happens in
       yk___sg_mtl_begin_pass()
    */
  if (nil != yk___sg.mtl.cmd_encoder) { yk___sg_mtl_bind_uniform_buffers(); }
}
YK___SOKOL_PRIVATE yk__sg_resource_state
yk___sg_mtl_create_context(yk___sg_context_t *ctx) {
  YK__SOKOL_ASSERT(ctx);
  YK___SOKOL_UNUSED(ctx);
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_mtl_destroy_context(yk___sg_context_t *ctx) {
  YK__SOKOL_ASSERT(ctx);
  YK___SOKOL_UNUSED(ctx);
  /* empty */
}
YK___SOKOL_PRIVATE void yk___sg_mtl_activate_context(yk___sg_context_t *ctx) {
  YK___SOKOL_UNUSED(ctx);
  yk___sg_mtl_clear_state_cache();
}
YK___SOKOL_PRIVATE yk__sg_resource_state yk___sg_mtl_create_buffer(
    yk___sg_buffer_t *buf, const yk__sg_buffer_desc *desc) {
  YK__SOKOL_ASSERT(buf && desc);
  yk___sg_buffer_common_init(&buf->cmn, desc);
  const bool injected = (0 != desc->mtl_buffers[0]);
  MTLResourceOptions mtl_options =
      yk___sg_mtl_buffer_resource_options(buf->cmn.usage);
  for (int slot = 0; slot < buf->cmn.num_slots; slot++) {
    id<MTLBuffer> mtl_buf;
    if (injected) {
      YK__SOKOL_ASSERT(desc->mtl_buffers[slot]);
      mtl_buf = (__bridge id<MTLBuffer>) desc->mtl_buffers[slot];
    } else {
      if (buf->cmn.usage == YK__SG_USAGE_IMMUTABLE) {
        YK__SOKOL_ASSERT(desc->data.ptr);
        mtl_buf =
            [yk___sg.mtl.device newBufferWithBytes:desc->data.ptr
                                            length:(NSUInteger) buf->cmn.size
                                           options:mtl_options];
      } else {
        mtl_buf =
            [yk___sg.mtl.device newBufferWithLength:(NSUInteger) buf->cmn.size
                                            options:mtl_options];
      }
    }
    buf->mtl.buf[slot] = yk___sg_mtl_add_resource(mtl_buf);
  }
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_mtl_destroy_buffer(yk___sg_buffer_t *buf) {
  YK__SOKOL_ASSERT(buf);
  for (int slot = 0; slot < buf->cmn.num_slots; slot++) {
    /* it's valid to call release resource with '0' */
    yk___sg_mtl_release_resource(yk___sg.mtl.frame_index, buf->mtl.buf[slot]);
  }
}
YK___SOKOL_PRIVATE void
yk___sg_mtl_copy_image_data(const yk___sg_image_t *img,
                            __unsafe_unretained id<MTLTexture> mtl_tex,
                            const yk__sg_image_data *data) {
  const int num_faces = (img->cmn.type == YK__SG_IMAGETYPE_CUBE) ? 6 : 1;
  const int num_slices =
      (img->cmn.type == YK__SG_IMAGETYPE_ARRAY) ? img->cmn.num_slices : 1;
  for (int face_index = 0; face_index < num_faces; face_index++) {
    for (int mip_index = 0; mip_index < img->cmn.num_mipmaps; mip_index++) {
      YK__SOKOL_ASSERT(data->subimage[face_index][mip_index].ptr);
      YK__SOKOL_ASSERT(data->subimage[face_index][mip_index].size > 0);
      const uint8_t *data_ptr =
          (const uint8_t *) data->subimage[face_index][mip_index].ptr;
      const int mip_width = yk___sg_max(img->cmn.width >> mip_index, 1);
      const int mip_height = yk___sg_max(img->cmn.height >> mip_index, 1);
      /* special case PVRTC formats: bytePerRow and bytesPerImage must be 0 */
      int bytes_per_row = 0;
      int bytes_per_slice = 0;
      if (!yk___sg_mtl_is_pvrtc(img->cmn.pixel_format)) {
        bytes_per_row = yk___sg_row_pitch(img->cmn.pixel_format, mip_width, 1);
        bytes_per_slice = yk___sg_surface_pitch(img->cmn.pixel_format,
                                                mip_width, mip_height, 1);
      }
      /* bytesPerImage special case: https://developer.apple.com/documentation/metal/mtltexture/1515679-replaceregion

                "Supply a nonzero value only when you copy data to a MTLTextureType3D type texture"
            */
      MTLRegion region;
      int bytes_per_image;
      if (img->cmn.type == YK__SG_IMAGETYPE_3D) {
        const int mip_depth = yk___sg_max(img->cmn.num_slices >> mip_index, 1);
        region =
            MTLRegionMake3D(0, 0, 0, (NSUInteger) mip_width,
                            (NSUInteger) mip_height, (NSUInteger) mip_depth);
        bytes_per_image = bytes_per_slice;
        /* FIXME: apparently the minimal bytes_per_image size for 3D texture
                 is 4 KByte... somehow need to handle this */
      } else {
        region = MTLRegionMake2D(0, 0, (NSUInteger) mip_width,
                                 (NSUInteger) mip_height);
        bytes_per_image = 0;
      }
      for (int slice_index = 0; slice_index < num_slices; slice_index++) {
        const int mtl_slice_index =
            (img->cmn.type == YK__SG_IMAGETYPE_CUBE) ? face_index : slice_index;
        const int slice_offset = slice_index * bytes_per_slice;
        YK__SOKOL_ASSERT((slice_offset + bytes_per_slice) <=
                         (int) data->subimage[face_index][mip_index].size);
        [mtl_tex replaceRegion:region
                   mipmapLevel:(NSUInteger) mip_index
                         slice:(NSUInteger) mtl_slice_index
                     withBytes:data_ptr + slice_offset
                   bytesPerRow:(NSUInteger) bytes_per_row
                 bytesPerImage:(NSUInteger) bytes_per_image];
      }
    }
  }
}
/*
    FIXME: METAL RESOURCE STORAGE MODE FOR macOS AND iOS

    For immutable textures on macOS, the recommended procedure is to create
    a MTLStorageModeManaged texture with the immutable content first,
    and then use the GPU to blit the content into a MTLStorageModePrivate
    texture before the first use.

    On iOS use the same one-time-blit procedure, but from a
    MTLStorageModeShared to a MTLStorageModePrivate texture.

    It probably makes sense to handle this in a separate 'resource manager'
    with a recycable pool of blit-source-textures?
*/
/* initialize MTLTextureDescritor with common attributes */
YK___SOKOL_PRIVATE bool
yk___sg_mtl_init_texdesc_common(MTLTextureDescriptor *mtl_desc,
                                yk___sg_image_t *img) {
  mtl_desc.textureType = yk___sg_mtl_texture_type(img->cmn.type);
  mtl_desc.pixelFormat = yk___sg_mtl_pixel_format(img->cmn.pixel_format);
  if (MTLPixelFormatInvalid == mtl_desc.pixelFormat) {
    YK__SOKOL_LOG("Unsupported texture pixel format!\n");
    return false;
  }
  mtl_desc.width = (NSUInteger) img->cmn.width;
  mtl_desc.height = (NSUInteger) img->cmn.height;
  if (YK__SG_IMAGETYPE_3D == img->cmn.type) {
    mtl_desc.depth = (NSUInteger) img->cmn.num_slices;
  } else {
    mtl_desc.depth = 1;
  }
  mtl_desc.mipmapLevelCount = (NSUInteger) img->cmn.num_mipmaps;
  if (YK__SG_IMAGETYPE_ARRAY == img->cmn.type) {
    mtl_desc.arrayLength = (NSUInteger) img->cmn.num_slices;
  } else {
    mtl_desc.arrayLength = 1;
  }
  mtl_desc.usage = MTLTextureUsageShaderRead;
  MTLResourceOptions res_options = 0;
  if (img->cmn.usage != YK__SG_USAGE_IMMUTABLE) {
    res_options |= MTLResourceCPUCacheModeWriteCombined;
  }
#if defined(YK___SG_TARGET_MACOS)
  /* macOS: use managed textures */
  res_options |= MTLResourceStorageModeManaged;
#else
  /* iOS: use CPU/GPU shared memory */
  res_options |= MTLResourceStorageModeShared;
#endif
  mtl_desc.resourceOptions = res_options;
  return true;
}
/* initialize MTLTextureDescritor with rendertarget attributes */
YK___SOKOL_PRIVATE void
yk___sg_mtl_init_texdesc_rt(MTLTextureDescriptor *mtl_desc,
                            yk___sg_image_t *img) {
  YK__SOKOL_ASSERT(img->cmn.render_target);
  YK___SOKOL_UNUSED(img);
  /* render targets are only visible to the GPU */
  mtl_desc.resourceOptions = MTLResourceStorageModePrivate;
  /* non-MSAA render targets are shader-readable */
  mtl_desc.usage = MTLTextureUsageShaderRead | MTLTextureUsageRenderTarget;
}
/* initialize MTLTextureDescritor with MSAA attributes */
YK___SOKOL_PRIVATE void
yk___sg_mtl_init_texdesc_rt_msaa(MTLTextureDescriptor *mtl_desc,
                                 yk___sg_image_t *img) {
  YK__SOKOL_ASSERT(img->cmn.sample_count > 1);
  /* render targets are only visible to the GPU */
  mtl_desc.resourceOptions = MTLResourceStorageModePrivate;
  /* MSAA render targets are not shader-readable (instead they are resolved) */
  mtl_desc.usage = MTLTextureUsageRenderTarget;
  mtl_desc.textureType = MTLTextureType2DMultisample;
  mtl_desc.depth = 1;
  mtl_desc.arrayLength = 1;
  mtl_desc.mipmapLevelCount = 1;
  mtl_desc.sampleCount = (NSUInteger) img->cmn.sample_count;
}
YK___SOKOL_PRIVATE yk__sg_resource_state
yk___sg_mtl_create_image(yk___sg_image_t *img, const yk__sg_image_desc *desc) {
  YK__SOKOL_ASSERT(img && desc);
  yk___sg_image_common_init(&img->cmn, desc);
  const bool injected = (0 != desc->mtl_textures[0]);
  const bool msaa = (img->cmn.sample_count > 1);
  /* first initialize all Metal resource pool slots to 'empty' */
  for (int i = 0; i < YK__SG_NUM_INFLIGHT_FRAMES; i++) {
    img->mtl.tex[i] = yk___sg_mtl_add_resource(nil);
  }
  img->mtl.sampler_state = yk___sg_mtl_add_resource(nil);
  img->mtl.depth_tex = yk___sg_mtl_add_resource(nil);
  img->mtl.msaa_tex = yk___sg_mtl_add_resource(nil);
  /* initialize a Metal texture descriptor with common attributes */
  MTLTextureDescriptor *mtl_desc = [[MTLTextureDescriptor alloc] init];
  if (!yk___sg_mtl_init_texdesc_common(mtl_desc, img)) {
    YK___SG_OBJC_RELEASE(mtl_desc);
    return YK__SG_RESOURCESTATE_FAILED;
  }
  /* special case depth-stencil-buffer? */
  if (yk___sg_is_valid_rendertarget_depth_format(img->cmn.pixel_format)) {
    /* depth-stencil buffer texture must always be a render target */
    YK__SOKOL_ASSERT(img->cmn.render_target);
    YK__SOKOL_ASSERT(img->cmn.type == YK__SG_IMAGETYPE_2D);
    YK__SOKOL_ASSERT(img->cmn.num_mipmaps == 1);
    YK__SOKOL_ASSERT(!injected);
    if (msaa) {
      yk___sg_mtl_init_texdesc_rt_msaa(mtl_desc, img);
    } else {
      yk___sg_mtl_init_texdesc_rt(mtl_desc, img);
    }
    id<MTLTexture> tex = [yk___sg.mtl.device newTextureWithDescriptor:mtl_desc];
    YK__SOKOL_ASSERT(nil != tex);
    img->mtl.depth_tex = yk___sg_mtl_add_resource(tex);
  } else {
    /* create the color texture
            In case this is a render target without MSAA, add the relevant
            render-target descriptor attributes.
            In case this is a render target *with* MSAA, the color texture
            will serve as MSAA-resolve target (not as render target), and rendering
            will go into a separate render target texture of type
            MTLTextureType2DMultisample.
        */
    if (img->cmn.render_target && !msaa) {
      yk___sg_mtl_init_texdesc_rt(mtl_desc, img);
    }
    for (int slot = 0; slot < img->cmn.num_slots; slot++) {
      id<MTLTexture> tex;
      if (injected) {
        YK__SOKOL_ASSERT(desc->mtl_textures[slot]);
        tex = (__bridge id<MTLTexture>) desc->mtl_textures[slot];
      } else {
        tex = [yk___sg.mtl.device newTextureWithDescriptor:mtl_desc];
        if ((img->cmn.usage == YK__SG_USAGE_IMMUTABLE) &&
            !img->cmn.render_target) {
          yk___sg_mtl_copy_image_data(img, tex, &desc->data);
        }
      }
      img->mtl.tex[slot] = yk___sg_mtl_add_resource(tex);
    }
    /* if MSAA color render target, create an additional MSAA render-surface texture */
    if (img->cmn.render_target && msaa) {
      yk___sg_mtl_init_texdesc_rt_msaa(mtl_desc, img);
      id<MTLTexture> tex =
          [yk___sg.mtl.device newTextureWithDescriptor:mtl_desc];
      img->mtl.msaa_tex = yk___sg_mtl_add_resource(tex);
    }
    /* create (possibly shared) sampler state */
    img->mtl.sampler_state =
        yk___sg_mtl_create_sampler(yk___sg.mtl.device, desc);
  }
  YK___SG_OBJC_RELEASE(mtl_desc);
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_mtl_destroy_image(yk___sg_image_t *img) {
  YK__SOKOL_ASSERT(img);
  /* it's valid to call release resource with a 'null resource' */
  for (int slot = 0; slot < img->cmn.num_slots; slot++) {
    yk___sg_mtl_release_resource(yk___sg.mtl.frame_index, img->mtl.tex[slot]);
  }
  yk___sg_mtl_release_resource(yk___sg.mtl.frame_index, img->mtl.depth_tex);
  yk___sg_mtl_release_resource(yk___sg.mtl.frame_index, img->mtl.msaa_tex);
  /* NOTE: sampler state objects are shared and not released until shutdown */
}
YK___SOKOL_PRIVATE id<MTLLibrary> yk___sg_mtl_compile_library(const char *src) {
  NSError *err = NULL;
  id<MTLLibrary> lib = [yk___sg.mtl.device
      newLibraryWithSource:[NSString stringWithUTF8String:src]
                   options:nil
                     error:&err];
  if (err) { YK__SOKOL_LOG([err.localizedDescription UTF8String]); }
  return lib;
}
YK___SOKOL_PRIVATE id<MTLLibrary>
yk___sg_mtl_library_from_bytecode(const void *ptr, size_t num_bytes) {
  NSError *err = NULL;
  dispatch_data_t lib_data = dispatch_data_create(
      ptr, num_bytes, NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
  id<MTLLibrary> lib = [yk___sg.mtl.device newLibraryWithData:lib_data
                                                        error:&err];
  if (err) { YK__SOKOL_LOG([err.localizedDescription UTF8String]); }
  YK___SG_OBJC_RELEASE(lib_data);
  return lib;
}
YK___SOKOL_PRIVATE yk__sg_resource_state yk___sg_mtl_create_shader(
    yk___sg_shader_t *shd, const yk__sg_shader_desc *desc) {
  YK__SOKOL_ASSERT(shd && desc);
  yk___sg_shader_common_init(&shd->cmn, desc);
  /* create metal libray objects and lookup entry functions */
  id<MTLLibrary> vs_lib;
  id<MTLLibrary> fs_lib;
  id<MTLFunction> vs_func;
  id<MTLFunction> fs_func;
  const char *vs_entry = desc->vs.entry;
  const char *fs_entry = desc->fs.entry;
  if (desc->vs.bytecode.ptr && desc->fs.bytecode.ptr) {
    /* separate byte code provided */
    vs_lib = yk___sg_mtl_library_from_bytecode(desc->vs.bytecode.ptr,
                                               desc->vs.bytecode.size);
    fs_lib = yk___sg_mtl_library_from_bytecode(desc->fs.bytecode.ptr,
                                               desc->fs.bytecode.size);
    if (nil == vs_lib || nil == fs_lib) { return YK__SG_RESOURCESTATE_FAILED; }
    vs_func =
        [vs_lib newFunctionWithName:[NSString stringWithUTF8String:vs_entry]];
    fs_func =
        [fs_lib newFunctionWithName:[NSString stringWithUTF8String:fs_entry]];
  } else if (desc->vs.source && desc->fs.source) {
    /* separate sources provided */
    vs_lib = yk___sg_mtl_compile_library(desc->vs.source);
    fs_lib = yk___sg_mtl_compile_library(desc->fs.source);
    if (nil == vs_lib || nil == fs_lib) { return YK__SG_RESOURCESTATE_FAILED; }
    vs_func =
        [vs_lib newFunctionWithName:[NSString stringWithUTF8String:vs_entry]];
    fs_func =
        [fs_lib newFunctionWithName:[NSString stringWithUTF8String:fs_entry]];
  } else {
    return YK__SG_RESOURCESTATE_FAILED;
  }
  if (nil == vs_func) {
    YK__SOKOL_LOG("vertex shader entry function not found\n");
    return YK__SG_RESOURCESTATE_FAILED;
  }
  if (nil == fs_func) {
    YK__SOKOL_LOG("fragment shader entry function not found\n");
    return YK__SG_RESOURCESTATE_FAILED;
  }
  /* it is legal to call yk___sg_mtl_add_resource with a nil value, this will return a special 0xFFFFFFFF index */
  shd->mtl.stage[YK__SG_SHADERSTAGE_VS].mtl_lib =
      yk___sg_mtl_add_resource(vs_lib);
  shd->mtl.stage[YK__SG_SHADERSTAGE_FS].mtl_lib =
      yk___sg_mtl_add_resource(fs_lib);
  shd->mtl.stage[YK__SG_SHADERSTAGE_VS].mtl_func =
      yk___sg_mtl_add_resource(vs_func);
  shd->mtl.stage[YK__SG_SHADERSTAGE_FS].mtl_func =
      yk___sg_mtl_add_resource(fs_func);
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_mtl_destroy_shader(yk___sg_shader_t *shd) {
  YK__SOKOL_ASSERT(shd);
  /* it is valid to call yk___sg_mtl_release_resource with a 'null resource' */
  yk___sg_mtl_release_resource(yk___sg.mtl.frame_index,
                               shd->mtl.stage[YK__SG_SHADERSTAGE_VS].mtl_func);
  yk___sg_mtl_release_resource(yk___sg.mtl.frame_index,
                               shd->mtl.stage[YK__SG_SHADERSTAGE_VS].mtl_lib);
  yk___sg_mtl_release_resource(yk___sg.mtl.frame_index,
                               shd->mtl.stage[YK__SG_SHADERSTAGE_FS].mtl_func);
  yk___sg_mtl_release_resource(yk___sg.mtl.frame_index,
                               shd->mtl.stage[YK__SG_SHADERSTAGE_FS].mtl_lib);
}
YK___SOKOL_PRIVATE yk__sg_resource_state
yk___sg_mtl_create_pipeline(yk___sg_pipeline_t *pip, yk___sg_shader_t *shd,
                            const yk__sg_pipeline_desc *desc) {
  YK__SOKOL_ASSERT(pip && shd && desc);
  YK__SOKOL_ASSERT(desc->shader.id == shd->slot.id);
  pip->shader = shd;
  yk___sg_pipeline_common_init(&pip->cmn, desc);
  yk__sg_primitive_type prim_type = desc->primitive_type;
  pip->mtl.prim_type = yk___sg_mtl_primitive_type(prim_type);
  pip->mtl.index_size = yk___sg_mtl_index_size(pip->cmn.index_type);
  if (YK__SG_INDEXTYPE_NONE != pip->cmn.index_type) {
    pip->mtl.index_type = yk___sg_mtl_index_type(pip->cmn.index_type);
  }
  pip->mtl.cull_mode = yk___sg_mtl_cull_mode(desc->cull_mode);
  pip->mtl.winding = yk___sg_mtl_winding(desc->face_winding);
  pip->mtl.stencil_ref = desc->stencil.ref;
  /* create vertex-descriptor */
  MTLVertexDescriptor *vtx_desc = [MTLVertexDescriptor vertexDescriptor];
  for (NSUInteger attr_index = 0; attr_index < YK__SG_MAX_VERTEX_ATTRIBUTES;
       attr_index++) {
    const yk__sg_vertex_attr_desc *a_desc = &desc->layout.attrs[attr_index];
    if (a_desc->format == YK__SG_VERTEXFORMAT_INVALID) { break; }
    YK__SOKOL_ASSERT(a_desc->buffer_index < YK__SG_MAX_SHADERSTAGE_BUFFERS);
    vtx_desc.attributes[attr_index].format =
        yk___sg_mtl_vertex_format(a_desc->format);
    vtx_desc.attributes[attr_index].offset = (NSUInteger) a_desc->offset;
    vtx_desc.attributes[attr_index].bufferIndex =
        (NSUInteger) (a_desc->buffer_index + YK__SG_MAX_SHADERSTAGE_UBS);
    pip->cmn.vertex_layout_valid[a_desc->buffer_index] = true;
  }
  for (NSUInteger layout_index = 0;
       layout_index < YK__SG_MAX_SHADERSTAGE_BUFFERS; layout_index++) {
    if (pip->cmn.vertex_layout_valid[layout_index]) {
      const yk__sg_buffer_layout_desc *l_desc =
          &desc->layout.buffers[layout_index];
      const NSUInteger mtl_vb_slot = layout_index + YK__SG_MAX_SHADERSTAGE_UBS;
      YK__SOKOL_ASSERT(l_desc->stride > 0);
      vtx_desc.layouts[mtl_vb_slot].stride = (NSUInteger) l_desc->stride;
      vtx_desc.layouts[mtl_vb_slot].stepFunction =
          yk___sg_mtl_step_function(l_desc->step_func);
      vtx_desc.layouts[mtl_vb_slot].stepRate = (NSUInteger) l_desc->step_rate;
      if (YK__SG_VERTEXSTEP_PER_INSTANCE == l_desc->step_func) {
        // NOTE: not actually used in yk___sg_mtl_draw()
        pip->cmn.use_instanced_draw = true;
      }
    }
  }
  /* render-pipeline descriptor */
  MTLRenderPipelineDescriptor *rp_desc =
      [[MTLRenderPipelineDescriptor alloc] init];
  rp_desc.vertexDescriptor = vtx_desc;
  YK__SOKOL_ASSERT(shd->mtl.stage[YK__SG_SHADERSTAGE_VS].mtl_func !=
                   YK___SG_MTL_INVALID_SLOT_INDEX);
  rp_desc.vertexFunction =
      yk___sg_mtl_id(shd->mtl.stage[YK__SG_SHADERSTAGE_VS].mtl_func);
  YK__SOKOL_ASSERT(shd->mtl.stage[YK__SG_SHADERSTAGE_FS].mtl_func !=
                   YK___SG_MTL_INVALID_SLOT_INDEX);
  rp_desc.fragmentFunction =
      yk___sg_mtl_id(shd->mtl.stage[YK__SG_SHADERSTAGE_FS].mtl_func);
  rp_desc.sampleCount = (NSUInteger) desc->sample_count;
  rp_desc.alphaToCoverageEnabled = desc->alpha_to_coverage_enabled;
  rp_desc.alphaToOneEnabled = NO;
  rp_desc.rasterizationEnabled = YES;
  rp_desc.depthAttachmentPixelFormat =
      yk___sg_mtl_pixel_format(desc->depth.pixel_format);
  if (desc->depth.pixel_format == YK__SG_PIXELFORMAT_DEPTH_STENCIL) {
    rp_desc.stencilAttachmentPixelFormat =
        yk___sg_mtl_pixel_format(desc->depth.pixel_format);
  }
  /* FIXME: this only works on macOS 10.13!
    for (int i = 0; i < (YK__SG_MAX_SHADERSTAGE_UBS+YK__SG_MAX_SHADERSTAGE_BUFFERS); i++) {
        rp_desc.vertexBuffers[i].mutability = MTLMutabilityImmutable;
    }
    for (int i = 0; i < YK__SG_MAX_SHADERSTAGE_UBS; i++) {
        rp_desc.fragmentBuffers[i].mutability = MTLMutabilityImmutable;
    }
    */
  for (NSUInteger i = 0; i < (NSUInteger) desc->color_count; i++) {
    YK__SOKOL_ASSERT(i < YK__SG_MAX_COLOR_ATTACHMENTS);
    const yk__sg_color_state *cs = &desc->colors[i];
    rp_desc.colorAttachments[i].pixelFormat =
        yk___sg_mtl_pixel_format(cs->pixel_format);
    rp_desc.colorAttachments[i].writeMask =
        yk___sg_mtl_color_write_mask(cs->write_mask);
    rp_desc.colorAttachments[i].blendingEnabled = cs->blend.enabled;
    rp_desc.colorAttachments[i].alphaBlendOperation =
        yk___sg_mtl_blend_op(cs->blend.op_alpha);
    rp_desc.colorAttachments[i].rgbBlendOperation =
        yk___sg_mtl_blend_op(cs->blend.op_rgb);
    rp_desc.colorAttachments[i].destinationAlphaBlendFactor =
        yk___sg_mtl_blend_factor(cs->blend.dst_factor_alpha);
    rp_desc.colorAttachments[i].destinationRGBBlendFactor =
        yk___sg_mtl_blend_factor(cs->blend.dst_factor_rgb);
    rp_desc.colorAttachments[i].sourceAlphaBlendFactor =
        yk___sg_mtl_blend_factor(cs->blend.src_factor_alpha);
    rp_desc.colorAttachments[i].sourceRGBBlendFactor =
        yk___sg_mtl_blend_factor(cs->blend.src_factor_rgb);
  }
  NSError *err = NULL;
  id<MTLRenderPipelineState> mtl_rps =
      [yk___sg.mtl.device newRenderPipelineStateWithDescriptor:rp_desc
                                                         error:&err];
  YK___SG_OBJC_RELEASE(rp_desc);
  if (nil == mtl_rps) {
    YK__SOKOL_ASSERT(err);
    YK__SOKOL_LOG([err.localizedDescription UTF8String]);
    return YK__SG_RESOURCESTATE_FAILED;
  }
  /* depth-stencil-state */
  MTLDepthStencilDescriptor *ds_desc = [[MTLDepthStencilDescriptor alloc] init];
  ds_desc.depthCompareFunction = yk___sg_mtl_compare_func(desc->depth.compare);
  ds_desc.depthWriteEnabled = desc->depth.write_enabled;
  if (desc->stencil.enabled) {
    const yk__sg_stencil_face_state *sb = &desc->stencil.back;
    ds_desc.backFaceStencil = [[MTLStencilDescriptor alloc] init];
    ds_desc.backFaceStencil.stencilFailureOperation =
        yk___sg_mtl_stencil_op(sb->fail_op);
    ds_desc.backFaceStencil.depthFailureOperation =
        yk___sg_mtl_stencil_op(sb->depth_fail_op);
    ds_desc.backFaceStencil.depthStencilPassOperation =
        yk___sg_mtl_stencil_op(sb->pass_op);
    ds_desc.backFaceStencil.stencilCompareFunction =
        yk___sg_mtl_compare_func(sb->compare);
    ds_desc.backFaceStencil.readMask = desc->stencil.read_mask;
    ds_desc.backFaceStencil.writeMask = desc->stencil.write_mask;
    const yk__sg_stencil_face_state *sf = &desc->stencil.front;
    ds_desc.frontFaceStencil = [[MTLStencilDescriptor alloc] init];
    ds_desc.frontFaceStencil.stencilFailureOperation =
        yk___sg_mtl_stencil_op(sf->fail_op);
    ds_desc.frontFaceStencil.depthFailureOperation =
        yk___sg_mtl_stencil_op(sf->depth_fail_op);
    ds_desc.frontFaceStencil.depthStencilPassOperation =
        yk___sg_mtl_stencil_op(sf->pass_op);
    ds_desc.frontFaceStencil.stencilCompareFunction =
        yk___sg_mtl_compare_func(sf->compare);
    ds_desc.frontFaceStencil.readMask = desc->stencil.read_mask;
    ds_desc.frontFaceStencil.writeMask = desc->stencil.write_mask;
  }
  id<MTLDepthStencilState> mtl_dss =
      [yk___sg.mtl.device newDepthStencilStateWithDescriptor:ds_desc];
  YK___SG_OBJC_RELEASE(ds_desc);
  pip->mtl.rps = yk___sg_mtl_add_resource(mtl_rps);
  pip->mtl.dss = yk___sg_mtl_add_resource(mtl_dss);
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_mtl_destroy_pipeline(yk___sg_pipeline_t *pip) {
  YK__SOKOL_ASSERT(pip);
  /* it's valid to call release resource with a 'null resource' */
  yk___sg_mtl_release_resource(yk___sg.mtl.frame_index, pip->mtl.rps);
  yk___sg_mtl_release_resource(yk___sg.mtl.frame_index, pip->mtl.dss);
}
YK___SOKOL_PRIVATE yk__sg_resource_state
yk___sg_mtl_create_pass(yk___sg_pass_t *pass, yk___sg_image_t **att_images,
                        const yk__sg_pass_desc *desc) {
  YK__SOKOL_ASSERT(pass && desc);
  YK__SOKOL_ASSERT(att_images && att_images[0]);
  yk___sg_pass_common_init(&pass->cmn, desc);
  /* copy image pointers */
  const yk__sg_pass_attachment_desc *att_desc;
  for (int i = 0; i < pass->cmn.num_color_atts; i++) {
    att_desc = &desc->color_attachments[i];
    if (att_desc->image.id != YK__SG_INVALID_ID) {
      YK__SOKOL_ASSERT(att_desc->image.id != YK__SG_INVALID_ID);
      YK__SOKOL_ASSERT(0 == pass->mtl.color_atts[i].image);
      YK__SOKOL_ASSERT(att_images[i] &&
                       (att_images[i]->slot.id == att_desc->image.id));
      YK__SOKOL_ASSERT(yk___sg_is_valid_rendertarget_color_format(
          att_images[i]->cmn.pixel_format));
      pass->mtl.color_atts[i].image = att_images[i];
    }
  }
  YK__SOKOL_ASSERT(0 == pass->mtl.ds_att.image);
  att_desc = &desc->depth_stencil_attachment;
  if (att_desc->image.id != YK__SG_INVALID_ID) {
    const int ds_img_index = YK__SG_MAX_COLOR_ATTACHMENTS;
    YK__SOKOL_ASSERT(att_images[ds_img_index] &&
                     (att_images[ds_img_index]->slot.id == att_desc->image.id));
    YK__SOKOL_ASSERT(yk___sg_is_valid_rendertarget_depth_format(
        att_images[ds_img_index]->cmn.pixel_format));
    pass->mtl.ds_att.image = att_images[ds_img_index];
  }
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_mtl_destroy_pass(yk___sg_pass_t *pass) {
  YK__SOKOL_ASSERT(pass);
  YK___SOKOL_UNUSED(pass);
}
YK___SOKOL_PRIVATE yk___sg_image_t *
yk___sg_mtl_pass_color_image(const yk___sg_pass_t *pass, int index) {
  YK__SOKOL_ASSERT(pass && (index >= 0) &&
                   (index < YK__SG_MAX_COLOR_ATTACHMENTS));
  /* NOTE: may return null */
  return pass->mtl.color_atts[index].image;
}
YK___SOKOL_PRIVATE yk___sg_image_t *
yk___sg_mtl_pass_ds_image(const yk___sg_pass_t *pass) {
  /* NOTE: may return null */
  YK__SOKOL_ASSERT(pass);
  return pass->mtl.ds_att.image;
}
YK___SOKOL_PRIVATE void yk___sg_mtl_begin_pass(yk___sg_pass_t *pass,
                                               const yk__sg_pass_action *action,
                                               int w, int h) {
  YK__SOKOL_ASSERT(action);
  YK__SOKOL_ASSERT(!yk___sg.mtl.in_pass);
  YK__SOKOL_ASSERT(yk___sg.mtl.cmd_queue);
  YK__SOKOL_ASSERT(nil == yk___sg.mtl.cmd_encoder);
  YK__SOKOL_ASSERT(yk___sg.mtl.renderpass_descriptor_cb ||
                   yk___sg.mtl.renderpass_descriptor_userdata_cb);
  yk___sg.mtl.in_pass = true;
  yk___sg.mtl.cur_width = w;
  yk___sg.mtl.cur_height = h;
  yk___sg_mtl_clear_state_cache();
  /* if this is the first pass in the frame, create a command buffer */
  if (nil == yk___sg.mtl.cmd_buffer) {
    /* block until the oldest frame in flight has finished */
    dispatch_semaphore_wait(yk___sg.mtl.sem, DISPATCH_TIME_FOREVER);
    yk___sg.mtl.cmd_buffer =
        [yk___sg.mtl.cmd_queue commandBufferWithUnretainedReferences];
    [yk___sg.mtl.cmd_buffer
        addCompletedHandler:^(id<MTLCommandBuffer> cmd_buffer) {
          // NOTE: this code is called on a different thread!
          YK___SOKOL_UNUSED(cmd_buffer);
          dispatch_semaphore_signal(yk___sg.mtl.sem);
        }];
  }
  /* if this is first pass in frame, get uniform buffer base pointer */
  if (0 == yk___sg.mtl.cur_ub_base_ptr) {
    yk___sg.mtl.cur_ub_base_ptr = (uint8_t *)
        [yk___sg.mtl.uniform_buffers[yk___sg.mtl.cur_frame_rotate_index]
            contents];
  }
  /* initialize a render pass descriptor */
  MTLRenderPassDescriptor *pass_desc = nil;
  if (pass) {
    /* offscreen render pass */
    pass_desc = [MTLRenderPassDescriptor renderPassDescriptor];
  } else {
    /* default render pass, call user-provided callback to provide render pass descriptor */
    if (yk___sg.mtl.renderpass_descriptor_cb) {
      pass_desc = (__bridge MTLRenderPassDescriptor *)
                      yk___sg.mtl.renderpass_descriptor_cb();
    } else {
      pass_desc = (__bridge MTLRenderPassDescriptor *) yk___sg.mtl
                      .renderpass_descriptor_userdata_cb(yk___sg.mtl.user_data);
    }
  }
  if (pass_desc) {
    yk___sg.mtl.pass_valid = true;
  } else {
    /* default pass descriptor will not be valid if window is minimized,
           don't do any rendering in this case */
    yk___sg.mtl.pass_valid = false;
    return;
  }
  if (pass) {
    /* setup pass descriptor for offscreen rendering */
    YK__SOKOL_ASSERT(pass->slot.state == YK__SG_RESOURCESTATE_VALID);
    for (NSUInteger i = 0; i < (NSUInteger) pass->cmn.num_color_atts; i++) {
      const yk___sg_pass_attachment_t *cmn_att = &pass->cmn.color_atts[i];
      const yk___sg_mtl_attachment_t *mtl_att = &pass->mtl.color_atts[i];
      const yk___sg_image_t *att_img = mtl_att->image;
      YK__SOKOL_ASSERT(att_img->slot.state == YK__SG_RESOURCESTATE_VALID);
      YK__SOKOL_ASSERT(att_img->slot.id == cmn_att->image_id.id);
      const bool is_msaa = (att_img->cmn.sample_count > 1);
      pass_desc.colorAttachments[i].loadAction =
          yk___sg_mtl_load_action(action->colors[i].action);
      pass_desc.colorAttachments[i].storeAction =
          is_msaa ? MTLStoreActionMultisampleResolve : MTLStoreActionStore;
      yk__sg_color c = action->colors[i].value;
      pass_desc.colorAttachments[i].clearColor =
          MTLClearColorMake(c.r, c.g, c.b, c.a);
      if (is_msaa) {
        YK__SOKOL_ASSERT(att_img->mtl.msaa_tex !=
                         YK___SG_MTL_INVALID_SLOT_INDEX);
        YK__SOKOL_ASSERT(att_img->mtl.tex[mtl_att->image->cmn.active_slot] !=
                         YK___SG_MTL_INVALID_SLOT_INDEX);
        pass_desc.colorAttachments[i].texture =
            yk___sg_mtl_id(att_img->mtl.msaa_tex);
        pass_desc.colorAttachments[i].resolveTexture =
            yk___sg_mtl_id(att_img->mtl.tex[att_img->cmn.active_slot]);
        pass_desc.colorAttachments[i].resolveLevel =
            (NSUInteger) cmn_att->mip_level;
        switch (att_img->cmn.type) {
          case YK__SG_IMAGETYPE_CUBE:
          case YK__SG_IMAGETYPE_ARRAY:
            pass_desc.colorAttachments[i].resolveSlice =
                (NSUInteger) cmn_att->slice;
            break;
          case YK__SG_IMAGETYPE_3D:
            pass_desc.colorAttachments[i].resolveDepthPlane =
                (NSUInteger) cmn_att->slice;
            break;
          default:
            break;
        }
      } else {
        YK__SOKOL_ASSERT(att_img->mtl.tex[att_img->cmn.active_slot] !=
                         YK___SG_MTL_INVALID_SLOT_INDEX);
        pass_desc.colorAttachments[i].texture =
            yk___sg_mtl_id(att_img->mtl.tex[att_img->cmn.active_slot]);
        pass_desc.colorAttachments[i].level = (NSUInteger) cmn_att->mip_level;
        switch (att_img->cmn.type) {
          case YK__SG_IMAGETYPE_CUBE:
          case YK__SG_IMAGETYPE_ARRAY:
            pass_desc.colorAttachments[i].slice = (NSUInteger) cmn_att->slice;
            break;
          case YK__SG_IMAGETYPE_3D:
            pass_desc.colorAttachments[i].depthPlane =
                (NSUInteger) cmn_att->slice;
            break;
          default:
            break;
        }
      }
    }
    const yk___sg_image_t *ds_att_img = pass->mtl.ds_att.image;
    if (0 != ds_att_img) {
      YK__SOKOL_ASSERT(ds_att_img->slot.state == YK__SG_RESOURCESTATE_VALID);
      YK__SOKOL_ASSERT(ds_att_img->slot.id == pass->cmn.ds_att.image_id.id);
      YK__SOKOL_ASSERT(ds_att_img->mtl.depth_tex !=
                       YK___SG_MTL_INVALID_SLOT_INDEX);
      pass_desc.depthAttachment.texture =
          yk___sg_mtl_id(ds_att_img->mtl.depth_tex);
      pass_desc.depthAttachment.loadAction =
          yk___sg_mtl_load_action(action->depth.action);
      pass_desc.depthAttachment.clearDepth = action->depth.value;
      if (yk___sg_is_depth_stencil_format(ds_att_img->cmn.pixel_format)) {
        pass_desc.stencilAttachment.texture =
            yk___sg_mtl_id(ds_att_img->mtl.depth_tex);
        pass_desc.stencilAttachment.loadAction =
            yk___sg_mtl_load_action(action->stencil.action);
        pass_desc.stencilAttachment.clearStencil = action->stencil.value;
      }
    }
  } else {
    /* setup pass descriptor for default rendering */
    pass_desc.colorAttachments[0].loadAction =
        yk___sg_mtl_load_action(action->colors[0].action);
    yk__sg_color c = action->colors[0].value;
    pass_desc.colorAttachments[0].clearColor =
        MTLClearColorMake(c.r, c.g, c.b, c.a);
    pass_desc.depthAttachment.loadAction =
        yk___sg_mtl_load_action(action->depth.action);
    pass_desc.depthAttachment.clearDepth = action->depth.value;
    pass_desc.stencilAttachment.loadAction =
        yk___sg_mtl_load_action(action->stencil.action);
    pass_desc.stencilAttachment.clearStencil = action->stencil.value;
  }
  /* create a render command encoder, this might return nil if window is minimized */
  yk___sg.mtl.cmd_encoder =
      [yk___sg.mtl.cmd_buffer renderCommandEncoderWithDescriptor:pass_desc];
  if (nil == yk___sg.mtl.cmd_encoder) {
    yk___sg.mtl.pass_valid = false;
    return;
  }
  /* bind the global uniform buffer, this only happens once per pass */
  yk___sg_mtl_bind_uniform_buffers();
}
YK___SOKOL_PRIVATE void yk___sg_mtl_end_pass(void) {
  YK__SOKOL_ASSERT(yk___sg.mtl.in_pass);
  yk___sg.mtl.in_pass = false;
  yk___sg.mtl.pass_valid = false;
  if (nil != yk___sg.mtl.cmd_encoder) {
    [yk___sg.mtl.cmd_encoder endEncoding];
    /* NOTE: MTLRenderCommandEncoder is autoreleased */
    yk___sg.mtl.cmd_encoder = nil;
  }
}
YK___SOKOL_PRIVATE void yk___sg_mtl_commit(void) {
  YK__SOKOL_ASSERT(!yk___sg.mtl.in_pass);
  YK__SOKOL_ASSERT(!yk___sg.mtl.pass_valid);
  YK__SOKOL_ASSERT(yk___sg.mtl.drawable_cb || yk___sg.mtl.drawable_userdata_cb);
  YK__SOKOL_ASSERT(nil == yk___sg.mtl.cmd_encoder);
  YK__SOKOL_ASSERT(nil != yk___sg.mtl.cmd_buffer);
  /* present, commit and signal semaphore when done */
  id<MTLDrawable> cur_drawable = nil;
  if (yk___sg.mtl.drawable_cb) {
    cur_drawable = (__bridge id<MTLDrawable>) yk___sg.mtl.drawable_cb();
  } else {
    cur_drawable = (__bridge id<MTLDrawable>) yk___sg.mtl.drawable_userdata_cb(
        yk___sg.mtl.user_data);
  }
  if (nil != cur_drawable) {
    [yk___sg.mtl.cmd_buffer presentDrawable:cur_drawable];
  }
  [yk___sg.mtl.cmd_buffer commit];
  /* garbage-collect resources pending for release */
  yk___sg_mtl_garbage_collect(yk___sg.mtl.frame_index);
  /* rotate uniform buffer slot */
  if (++yk___sg.mtl.cur_frame_rotate_index >= YK__SG_NUM_INFLIGHT_FRAMES) {
    yk___sg.mtl.cur_frame_rotate_index = 0;
  }
  yk___sg.mtl.frame_index++;
  yk___sg.mtl.cur_ub_offset = 0;
  yk___sg.mtl.cur_ub_base_ptr = 0;
  /* NOTE: MTLCommandBuffer is autoreleased */
  yk___sg.mtl.cmd_buffer = nil;
}
YK___SOKOL_PRIVATE void yk___sg_mtl_apply_viewport(int x, int y, int w, int h,
                                                   bool origin_top_left) {
  YK__SOKOL_ASSERT(yk___sg.mtl.in_pass);
  if (!yk___sg.mtl.pass_valid) { return; }
  YK__SOKOL_ASSERT(nil != yk___sg.mtl.cmd_encoder);
  MTLViewport vp;
  vp.originX = (double) x;
  vp.originY =
      (double) (origin_top_left ? y : (yk___sg.mtl.cur_height - (y + h)));
  vp.width = (double) w;
  vp.height = (double) h;
  vp.znear = 0.0;
  vp.zfar = 1.0;
  [yk___sg.mtl.cmd_encoder setViewport:vp];
}
YK___SOKOL_PRIVATE void yk___sg_mtl_apply_scissor_rect(int x, int y, int w,
                                                       int h,
                                                       bool origin_top_left) {
  YK__SOKOL_ASSERT(yk___sg.mtl.in_pass);
  if (!yk___sg.mtl.pass_valid) { return; }
  YK__SOKOL_ASSERT(nil != yk___sg.mtl.cmd_encoder);
  /* clip against framebuffer rect */
  x = yk___sg_min(yk___sg_max(0, x), yk___sg.mtl.cur_width - 1);
  y = yk___sg_min(yk___sg_max(0, y), yk___sg.mtl.cur_height - 1);
  if ((x + w) > yk___sg.mtl.cur_width) { w = yk___sg.mtl.cur_width - x; }
  if ((y + h) > yk___sg.mtl.cur_height) { h = yk___sg.mtl.cur_height - y; }
  w = yk___sg_max(w, 1);
  h = yk___sg_max(h, 1);
  MTLScissorRect r;
  r.x = (NSUInteger) x;
  r.y = (NSUInteger) (origin_top_left ? y : (yk___sg.mtl.cur_height - (y + h)));
  r.width = (NSUInteger) w;
  r.height = (NSUInteger) h;
  [yk___sg.mtl.cmd_encoder setScissorRect:r];
}
YK___SOKOL_PRIVATE void yk___sg_mtl_apply_pipeline(yk___sg_pipeline_t *pip) {
  YK__SOKOL_ASSERT(pip);
  YK__SOKOL_ASSERT(pip->shader &&
                   (pip->cmn.shader_id.id == pip->shader->slot.id));
  YK__SOKOL_ASSERT(yk___sg.mtl.in_pass);
  if (!yk___sg.mtl.pass_valid) { return; }
  YK__SOKOL_ASSERT(nil != yk___sg.mtl.cmd_encoder);
  if ((yk___sg.mtl.state_cache.cur_pipeline != pip) ||
      (yk___sg.mtl.state_cache.cur_pipeline_id.id != pip->slot.id)) {
    yk___sg.mtl.state_cache.cur_pipeline = pip;
    yk___sg.mtl.state_cache.cur_pipeline_id.id = pip->slot.id;
    yk__sg_color c = pip->cmn.blend_color;
    [yk___sg.mtl.cmd_encoder setBlendColorRed:c.r green:c.g blue:c.b alpha:c.a];
    [yk___sg.mtl.cmd_encoder setCullMode:pip->mtl.cull_mode];
    [yk___sg.mtl.cmd_encoder setFrontFacingWinding:pip->mtl.winding];
    [yk___sg.mtl.cmd_encoder setStencilReferenceValue:pip->mtl.stencil_ref];
    [yk___sg.mtl.cmd_encoder setDepthBias:pip->cmn.depth_bias
                               slopeScale:pip->cmn.depth_bias_slope_scale
                                    clamp:pip->cmn.depth_bias_clamp];
    YK__SOKOL_ASSERT(pip->mtl.rps != YK___SG_MTL_INVALID_SLOT_INDEX);
    [yk___sg.mtl.cmd_encoder
        setRenderPipelineState:yk___sg_mtl_id(pip->mtl.rps)];
    YK__SOKOL_ASSERT(pip->mtl.dss != YK___SG_MTL_INVALID_SLOT_INDEX);
    [yk___sg.mtl.cmd_encoder setDepthStencilState:yk___sg_mtl_id(pip->mtl.dss)];
  }
}
YK___SOKOL_PRIVATE void yk___sg_mtl_apply_bindings(
    yk___sg_pipeline_t *pip, yk___sg_buffer_t **vbs, const int *vb_offsets,
    int num_vbs, yk___sg_buffer_t *ib, int ib_offset, yk___sg_image_t **vs_imgs,
    int num_vs_imgs, yk___sg_image_t **fs_imgs, int num_fs_imgs) {
  YK___SOKOL_UNUSED(pip);
  YK__SOKOL_ASSERT(yk___sg.mtl.in_pass);
  if (!yk___sg.mtl.pass_valid) { return; }
  YK__SOKOL_ASSERT(nil != yk___sg.mtl.cmd_encoder);
  /* store index buffer binding, this will be needed later in yk__sg_draw() */
  yk___sg.mtl.state_cache.cur_indexbuffer = ib;
  yk___sg.mtl.state_cache.cur_indexbuffer_offset = ib_offset;
  if (ib) {
    YK__SOKOL_ASSERT(pip->cmn.index_type != YK__SG_INDEXTYPE_NONE);
    yk___sg.mtl.state_cache.cur_indexbuffer_id.id = ib->slot.id;
  } else {
    YK__SOKOL_ASSERT(pip->cmn.index_type == YK__SG_INDEXTYPE_NONE);
    yk___sg.mtl.state_cache.cur_indexbuffer_id.id = YK__SG_INVALID_ID;
  }
  /* apply vertex buffers */
  NSUInteger slot;
  for (slot = 0; slot < (NSUInteger) num_vbs; slot++) {
    const yk___sg_buffer_t *vb = vbs[slot];
    if ((yk___sg.mtl.state_cache.cur_vertexbuffers[slot] != vb) ||
        (yk___sg.mtl.state_cache.cur_vertexbuffer_offsets[slot] !=
         vb_offsets[slot]) ||
        (yk___sg.mtl.state_cache.cur_vertexbuffer_ids[slot].id !=
         vb->slot.id)) {
      yk___sg.mtl.state_cache.cur_vertexbuffers[slot] = vb;
      yk___sg.mtl.state_cache.cur_vertexbuffer_offsets[slot] = vb_offsets[slot];
      yk___sg.mtl.state_cache.cur_vertexbuffer_ids[slot].id = vb->slot.id;
      const NSUInteger mtl_slot = YK__SG_MAX_SHADERSTAGE_UBS + slot;
      YK__SOKOL_ASSERT(vb->mtl.buf[vb->cmn.active_slot] !=
                       YK___SG_MTL_INVALID_SLOT_INDEX);
      [yk___sg.mtl.cmd_encoder
          setVertexBuffer:yk___sg_mtl_id(vb->mtl.buf[vb->cmn.active_slot])
                   offset:(NSUInteger) vb_offsets[slot]
                  atIndex:mtl_slot];
    }
  }
  /* apply vertex shader images */
  for (slot = 0; slot < (NSUInteger) num_vs_imgs; slot++) {
    const yk___sg_image_t *img = vs_imgs[slot];
    if ((yk___sg.mtl.state_cache.cur_vs_images[slot] != img) ||
        (yk___sg.mtl.state_cache.cur_vs_image_ids[slot].id != img->slot.id)) {
      yk___sg.mtl.state_cache.cur_vs_images[slot] = img;
      yk___sg.mtl.state_cache.cur_vs_image_ids[slot].id = img->slot.id;
      YK__SOKOL_ASSERT(img->mtl.tex[img->cmn.active_slot] !=
                       YK___SG_MTL_INVALID_SLOT_INDEX);
      [yk___sg.mtl.cmd_encoder
          setVertexTexture:yk___sg_mtl_id(img->mtl.tex[img->cmn.active_slot])
                   atIndex:slot];
      YK__SOKOL_ASSERT(img->mtl.sampler_state !=
                       YK___SG_MTL_INVALID_SLOT_INDEX);
      [yk___sg.mtl.cmd_encoder
          setVertexSamplerState:yk___sg_mtl_id(img->mtl.sampler_state)
                        atIndex:slot];
    }
  }
  /* apply fragment shader images */
  for (slot = 0; slot < (NSUInteger) num_fs_imgs; slot++) {
    const yk___sg_image_t *img = fs_imgs[slot];
    if ((yk___sg.mtl.state_cache.cur_fs_images[slot] != img) ||
        (yk___sg.mtl.state_cache.cur_fs_image_ids[slot].id != img->slot.id)) {
      yk___sg.mtl.state_cache.cur_fs_images[slot] = img;
      yk___sg.mtl.state_cache.cur_fs_image_ids[slot].id = img->slot.id;
      YK__SOKOL_ASSERT(img->mtl.tex[img->cmn.active_slot] !=
                       YK___SG_MTL_INVALID_SLOT_INDEX);
      [yk___sg.mtl.cmd_encoder
          setFragmentTexture:yk___sg_mtl_id(img->mtl.tex[img->cmn.active_slot])
                     atIndex:slot];
      YK__SOKOL_ASSERT(img->mtl.sampler_state !=
                       YK___SG_MTL_INVALID_SLOT_INDEX);
      [yk___sg.mtl.cmd_encoder
          setFragmentSamplerState:yk___sg_mtl_id(img->mtl.sampler_state)
                          atIndex:slot];
    }
  }
}
YK___SOKOL_PRIVATE void
yk___sg_mtl_apply_uniforms(yk__sg_shader_stage stage_index, int ub_index,
                           const yk__sg_range *data) {
  YK__SOKOL_ASSERT(yk___sg.mtl.in_pass);
  if (!yk___sg.mtl.pass_valid) { return; }
  YK__SOKOL_ASSERT(nil != yk___sg.mtl.cmd_encoder);
  YK__SOKOL_ASSERT(((size_t) yk___sg.mtl.cur_ub_offset + data->size) <=
                   (size_t) yk___sg.mtl.ub_size);
  YK__SOKOL_ASSERT((yk___sg.mtl.cur_ub_offset & (YK___SG_MTL_UB_ALIGN - 1)) ==
                   0);
  YK__SOKOL_ASSERT(yk___sg.mtl.state_cache.cur_pipeline &&
                   yk___sg.mtl.state_cache.cur_pipeline->shader);
  YK__SOKOL_ASSERT(yk___sg.mtl.state_cache.cur_pipeline->slot.id ==
                   yk___sg.mtl.state_cache.cur_pipeline_id.id);
  YK__SOKOL_ASSERT(yk___sg.mtl.state_cache.cur_pipeline->shader->slot.id ==
                   yk___sg.mtl.state_cache.cur_pipeline->cmn.shader_id.id);
  YK__SOKOL_ASSERT(ub_index < yk___sg.mtl.state_cache.cur_pipeline->shader->cmn
                                  .stage[stage_index]
                                  .num_uniform_blocks);
  YK__SOKOL_ASSERT(
      data->size <=
      yk___sg.mtl.state_cache.cur_pipeline->shader->cmn.stage[stage_index]
          .uniform_blocks[ub_index]
          .size);
  /* copy to global uniform buffer, record offset into cmd encoder, and advance offset */
  uint8_t *dst = &yk___sg.mtl.cur_ub_base_ptr[yk___sg.mtl.cur_ub_offset];
  memcpy(dst, data->ptr, data->size);
  if (stage_index == YK__SG_SHADERSTAGE_VS) {
    [yk___sg.mtl.cmd_encoder
        setVertexBufferOffset:(NSUInteger) yk___sg.mtl.cur_ub_offset
                      atIndex:(NSUInteger) ub_index];
  } else {
    [yk___sg.mtl.cmd_encoder
        setFragmentBufferOffset:(NSUInteger) yk___sg.mtl.cur_ub_offset
                        atIndex:(NSUInteger) ub_index];
  }
  yk___sg.mtl.cur_ub_offset = yk___sg_roundup(
      yk___sg.mtl.cur_ub_offset + (int) data->size, YK___SG_MTL_UB_ALIGN);
}
YK___SOKOL_PRIVATE void yk___sg_mtl_draw(int base_element, int num_elements,
                                         int num_instances) {
  YK__SOKOL_ASSERT(yk___sg.mtl.in_pass);
  if (!yk___sg.mtl.pass_valid) { return; }
  YK__SOKOL_ASSERT(nil != yk___sg.mtl.cmd_encoder);
  YK__SOKOL_ASSERT(yk___sg.mtl.state_cache.cur_pipeline &&
                   (yk___sg.mtl.state_cache.cur_pipeline->slot.id ==
                    yk___sg.mtl.state_cache.cur_pipeline_id.id));
  if (YK__SG_INDEXTYPE_NONE !=
      yk___sg.mtl.state_cache.cur_pipeline->cmn.index_type) {
    /* indexed rendering */
    YK__SOKOL_ASSERT(yk___sg.mtl.state_cache.cur_indexbuffer &&
                     (yk___sg.mtl.state_cache.cur_indexbuffer->slot.id ==
                      yk___sg.mtl.state_cache.cur_indexbuffer_id.id));
    const yk___sg_buffer_t *ib = yk___sg.mtl.state_cache.cur_indexbuffer;
    YK__SOKOL_ASSERT(ib->mtl.buf[ib->cmn.active_slot] !=
                     YK___SG_MTL_INVALID_SLOT_INDEX);
    const NSUInteger index_buffer_offset =
        (NSUInteger) (yk___sg.mtl.state_cache.cur_indexbuffer_offset +
                      base_element *
                          yk___sg.mtl.state_cache.cur_pipeline->mtl.index_size);
    [yk___sg.mtl.cmd_encoder
        drawIndexedPrimitives:yk___sg.mtl.state_cache.cur_pipeline->mtl
                                  .prim_type
                   indexCount:(NSUInteger) num_elements
                    indexType:yk___sg.mtl.state_cache.cur_pipeline->mtl
                                  .index_type
                  indexBuffer:yk___sg_mtl_id(ib->mtl.buf[ib->cmn.active_slot])
            indexBufferOffset:index_buffer_offset
                instanceCount:(NSUInteger) num_instances];
  } else {
    /* non-indexed rendering */
    [yk___sg.mtl.cmd_encoder
        drawPrimitives:yk___sg.mtl.state_cache.cur_pipeline->mtl.prim_type
           vertexStart:(NSUInteger) base_element
           vertexCount:(NSUInteger) num_elements
         instanceCount:(NSUInteger) num_instances];
  }
}
YK___SOKOL_PRIVATE void yk___sg_mtl_update_buffer(yk___sg_buffer_t *buf,
                                                  const yk__sg_range *data) {
  YK__SOKOL_ASSERT(buf && data && data->ptr && (data->size > 0));
  if (++buf->cmn.active_slot >= buf->cmn.num_slots) {
    buf->cmn.active_slot = 0;
  }
  __unsafe_unretained id<MTLBuffer> mtl_buf =
      yk___sg_mtl_id(buf->mtl.buf[buf->cmn.active_slot]);
  void *dst_ptr = [mtl_buf contents];
  memcpy(dst_ptr, data->ptr, data->size);
#if defined(YK___SG_TARGET_MACOS)
  [mtl_buf didModifyRange:NSMakeRange(0, data->size)];
#endif
}
YK___SOKOL_PRIVATE int yk___sg_mtl_append_buffer(yk___sg_buffer_t *buf,
                                                 const yk__sg_range *data,
                                                 bool new_frame) {
  YK__SOKOL_ASSERT(buf && data && data->ptr && (data->size > 0));
  if (new_frame) {
    if (++buf->cmn.active_slot >= buf->cmn.num_slots) {
      buf->cmn.active_slot = 0;
    }
  }
  __unsafe_unretained id<MTLBuffer> mtl_buf =
      yk___sg_mtl_id(buf->mtl.buf[buf->cmn.active_slot]);
  uint8_t *dst_ptr = (uint8_t *) [mtl_buf contents];
  dst_ptr += buf->cmn.append_pos;
  memcpy(dst_ptr, data->ptr, data->size);
#if defined(YK___SG_TARGET_MACOS)
  [mtl_buf didModifyRange:NSMakeRange((NSUInteger) buf->cmn.append_pos,
                                      (NSUInteger) data->size)];
#endif
  /* NOTE: this is a requirement from WebGPU, but we want identical behaviour across all backends */
  return yk___sg_roundup((int) data->size, 4);
}
YK___SOKOL_PRIVATE void
yk___sg_mtl_update_image(yk___sg_image_t *img, const yk__sg_image_data *data) {
  YK__SOKOL_ASSERT(img && data);
  if (++img->cmn.active_slot >= img->cmn.num_slots) {
    img->cmn.active_slot = 0;
  }
  __unsafe_unretained id<MTLTexture> mtl_tex =
      yk___sg_mtl_id(img->mtl.tex[img->cmn.active_slot]);
  yk___sg_mtl_copy_image_data(img, mtl_tex, data);
}
/*== WEBGPU BACKEND IMPLEMENTATION ===========================================*/
#elif defined(YK__SOKOL_WGPU)
YK___SOKOL_PRIVATE WGPUBufferUsageFlags
yk___sg_wgpu_buffer_usage(yk__sg_buffer_type t, yk__sg_usage u) {
  WGPUBufferUsageFlags res = 0;
  if (YK__SG_BUFFERTYPE_VERTEXBUFFER == t) {
    res |= WGPUBufferUsage_Vertex;
  } else {
    res |= WGPUBufferUsage_Index;
  }
  if (YK__SG_USAGE_IMMUTABLE != u) { res |= WGPUBufferUsage_CopyDst; }
  return res;
}
YK___SOKOL_PRIVATE WGPULoadOp yk___sg_wgpu_load_op(yk__sg_action a) {
  switch (a) {
    case YK__SG_ACTION_CLEAR:
    case YK__SG_ACTION_DONTCARE:
      return WGPULoadOp_Clear;
    case YK__SG_ACTION_LOAD:
      return WGPULoadOp_Load;
    default:
      YK__SOKOL_UNREACHABLE;
      return (WGPULoadOp) 0;
  }
}
YK___SOKOL_PRIVATE WGPUTextureViewDimension
yk___sg_wgpu_tex_viewdim(yk__sg_image_type t) {
  switch (t) {
    case YK__SG_IMAGETYPE_2D:
      return WGPUTextureViewDimension_2D;
    case YK__SG_IMAGETYPE_CUBE:
      return WGPUTextureViewDimension_Cube;
    case YK__SG_IMAGETYPE_3D:
      return WGPUTextureViewDimension_3D;
    case YK__SG_IMAGETYPE_ARRAY:
      return WGPUTextureViewDimension_2DArray;
    default:
      YK__SOKOL_UNREACHABLE;
      return WGPUTextureViewDimension_Force32;
  }
}
YK___SOKOL_PRIVATE WGPUTextureComponentType
yk___sg_wgpu_tex_comptype(yk__sg_sampler_type t) {
  switch (t) {
    case YK__SG_SAMPLERTYPE_FLOAT:
      return WGPUTextureComponentType_Float;
    case YK__SG_SAMPLERTYPE_SINT:
      return WGPUTextureComponentType_Sint;
    case YK__SG_SAMPLERTYPE_UINT:
      return WGPUTextureComponentType_Uint;
    default:
      YK__SOKOL_UNREACHABLE;
      return WGPUTextureComponentType_Force32;
  }
}
YK___SOKOL_PRIVATE WGPUTextureDimension
yk___sg_wgpu_tex_dim(yk__sg_image_type t) {
  if (YK__SG_IMAGETYPE_3D == t) {
    return WGPUTextureDimension_3D;
  } else {
    return WGPUTextureDimension_2D;
  }
}
YK___SOKOL_PRIVATE WGPUAddressMode
yk___sg_wgpu_sampler_addrmode(yk__sg_wrap m) {
  switch (m) {
    case YK__SG_WRAP_REPEAT:
      return WGPUAddressMode_Repeat;
    case YK__SG_WRAP_CLAMP_TO_EDGE:
    case YK__SG_WRAP_CLAMP_TO_BORDER:
      return WGPUAddressMode_ClampToEdge;
    case YK__SG_WRAP_MIRRORED_REPEAT:
      return WGPUAddressMode_MirrorRepeat;
    default:
      YK__SOKOL_UNREACHABLE;
      return WGPUAddressMode_Force32;
  }
}
YK___SOKOL_PRIVATE WGPUFilterMode
yk___sg_wgpu_sampler_minmagfilter(yk__sg_filter f) {
  switch (f) {
    case YK__SG_FILTER_NEAREST:
    case YK__SG_FILTER_NEAREST_MIPMAP_NEAREST:
    case YK__SG_FILTER_NEAREST_MIPMAP_LINEAR:
      return WGPUFilterMode_Nearest;
    case YK__SG_FILTER_LINEAR:
    case YK__SG_FILTER_LINEAR_MIPMAP_NEAREST:
    case YK__SG_FILTER_LINEAR_MIPMAP_LINEAR:
      return WGPUFilterMode_Linear;
    default:
      YK__SOKOL_UNREACHABLE;
      return WGPUFilterMode_Force32;
  }
}
YK___SOKOL_PRIVATE WGPUFilterMode
yk___sg_wgpu_sampler_mipfilter(yk__sg_filter f) {
  switch (f) {
    case YK__SG_FILTER_NEAREST:
    case YK__SG_FILTER_LINEAR:
    case YK__SG_FILTER_NEAREST_MIPMAP_NEAREST:
    case YK__SG_FILTER_LINEAR_MIPMAP_NEAREST:
      return WGPUFilterMode_Nearest;
    case YK__SG_FILTER_NEAREST_MIPMAP_LINEAR:
    case YK__SG_FILTER_LINEAR_MIPMAP_LINEAR:
      return WGPUFilterMode_Linear;
    default:
      YK__SOKOL_UNREACHABLE;
      return WGPUFilterMode_Force32;
  }
}
YK___SOKOL_PRIVATE WGPUIndexFormat
yk___sg_wgpu_indexformat(yk__sg_index_type t) {
  /* NOTE: there's no WGPUIndexFormat_None */
  return (t == YK__SG_INDEXTYPE_UINT16) ? WGPUIndexFormat_Uint16
                                        : WGPUIndexFormat_Uint32;
}
YK___SOKOL_PRIVATE WGPUInputStepMode
yk___sg_wgpu_stepmode(yk__sg_vertex_step s) {
  return (s == YK__SG_VERTEXSTEP_PER_VERTEX) ? WGPUInputStepMode_Vertex
                                             : WGPUInputStepMode_Instance;
}
YK___SOKOL_PRIVATE WGPUVertexFormat
yk___sg_wgpu_vertexformat(yk__sg_vertex_format f) {
  switch (f) {
    case YK__SG_VERTEXFORMAT_FLOAT:
      return WGPUVertexFormat_Float;
    case YK__SG_VERTEXFORMAT_FLOAT2:
      return WGPUVertexFormat_Float2;
    case YK__SG_VERTEXFORMAT_FLOAT3:
      return WGPUVertexFormat_Float3;
    case YK__SG_VERTEXFORMAT_FLOAT4:
      return WGPUVertexFormat_Float4;
    case YK__SG_VERTEXFORMAT_BYTE4:
      return WGPUVertexFormat_Char4;
    case YK__SG_VERTEXFORMAT_BYTE4N:
      return WGPUVertexFormat_Char4Norm;
    case YK__SG_VERTEXFORMAT_UBYTE4:
      return WGPUVertexFormat_UChar4;
    case YK__SG_VERTEXFORMAT_UBYTE4N:
      return WGPUVertexFormat_UChar4Norm;
    case YK__SG_VERTEXFORMAT_SHORT2:
      return WGPUVertexFormat_Short2;
    case YK__SG_VERTEXFORMAT_SHORT2N:
      return WGPUVertexFormat_Short2Norm;
    case YK__SG_VERTEXFORMAT_USHORT2N:
      return WGPUVertexFormat_UShort2Norm;
    case YK__SG_VERTEXFORMAT_SHORT4:
      return WGPUVertexFormat_Short4;
    case YK__SG_VERTEXFORMAT_SHORT4N:
      return WGPUVertexFormat_Short4Norm;
    case YK__SG_VERTEXFORMAT_USHORT4N:
      return WGPUVertexFormat_UShort4Norm;
    /* FIXME! UINT10_N2 */
    case YK__SG_VERTEXFORMAT_UINT10_N2:
    default:
      YK__SOKOL_UNREACHABLE;
      return WGPUVertexFormat_Force32;
  }
}
YK___SOKOL_PRIVATE WGPUPrimitiveTopology
yk___sg_wgpu_topology(yk__sg_primitive_type t) {
  switch (t) {
    case YK__SG_PRIMITIVETYPE_POINTS:
      return WGPUPrimitiveTopology_PointList;
    case YK__SG_PRIMITIVETYPE_LINES:
      return WGPUPrimitiveTopology_LineList;
    case YK__SG_PRIMITIVETYPE_LINE_STRIP:
      return WGPUPrimitiveTopology_LineStrip;
    case YK__SG_PRIMITIVETYPE_TRIANGLES:
      return WGPUPrimitiveTopology_TriangleList;
    case YK__SG_PRIMITIVETYPE_TRIANGLE_STRIP:
      return WGPUPrimitiveTopology_TriangleStrip;
    default:
      YK__SOKOL_UNREACHABLE;
      return WGPUPrimitiveTopology_Force32;
  }
}
YK___SOKOL_PRIVATE WGPUFrontFace
yk___sg_wgpu_frontface(yk__sg_face_winding fw) {
  return (fw == YK__SG_FACEWINDING_CCW) ? WGPUFrontFace_CCW : WGPUFrontFace_CW;
}
YK___SOKOL_PRIVATE WGPUCullMode yk___sg_wgpu_cullmode(yk__sg_cull_mode cm) {
  switch (cm) {
    case YK__SG_CULLMODE_NONE:
      return WGPUCullMode_None;
    case YK__SG_CULLMODE_FRONT:
      return WGPUCullMode_Front;
    case YK__SG_CULLMODE_BACK:
      return WGPUCullMode_Back;
    default:
      YK__SOKOL_UNREACHABLE;
      return WGPUCullMode_Force32;
  }
}
YK___SOKOL_PRIVATE WGPUTextureFormat
yk___sg_wgpu_textureformat(yk__sg_pixel_format p) {
  switch (p) {
    case YK__SG_PIXELFORMAT_NONE:
      return WGPUTextureFormat_Undefined;
    case YK__SG_PIXELFORMAT_R8:
      return WGPUTextureFormat_R8Unorm;
    case YK__SG_PIXELFORMAT_R8SN:
      return WGPUTextureFormat_R8Snorm;
    case YK__SG_PIXELFORMAT_R8UI:
      return WGPUTextureFormat_R8Uint;
    case YK__SG_PIXELFORMAT_R8SI:
      return WGPUTextureFormat_R8Sint;
    case YK__SG_PIXELFORMAT_R16UI:
      return WGPUTextureFormat_R16Uint;
    case YK__SG_PIXELFORMAT_R16SI:
      return WGPUTextureFormat_R16Sint;
    case YK__SG_PIXELFORMAT_R16F:
      return WGPUTextureFormat_R16Float;
    case YK__SG_PIXELFORMAT_RG8:
      return WGPUTextureFormat_RG8Unorm;
    case YK__SG_PIXELFORMAT_RG8SN:
      return WGPUTextureFormat_RG8Snorm;
    case YK__SG_PIXELFORMAT_RG8UI:
      return WGPUTextureFormat_RG8Uint;
    case YK__SG_PIXELFORMAT_RG8SI:
      return WGPUTextureFormat_RG8Sint;
    case YK__SG_PIXELFORMAT_R32UI:
      return WGPUTextureFormat_R32Uint;
    case YK__SG_PIXELFORMAT_R32SI:
      return WGPUTextureFormat_R32Sint;
    case YK__SG_PIXELFORMAT_R32F:
      return WGPUTextureFormat_R32Float;
    case YK__SG_PIXELFORMAT_RG16UI:
      return WGPUTextureFormat_RG16Uint;
    case YK__SG_PIXELFORMAT_RG16SI:
      return WGPUTextureFormat_RG16Sint;
    case YK__SG_PIXELFORMAT_RG16F:
      return WGPUTextureFormat_RG16Float;
    case YK__SG_PIXELFORMAT_RGBA8:
      return WGPUTextureFormat_RGBA8Unorm;
    case YK__SG_PIXELFORMAT_RGBA8SN:
      return WGPUTextureFormat_RGBA8Snorm;
    case YK__SG_PIXELFORMAT_RGBA8UI:
      return WGPUTextureFormat_RGBA8Uint;
    case YK__SG_PIXELFORMAT_RGBA8SI:
      return WGPUTextureFormat_RGBA8Sint;
    case YK__SG_PIXELFORMAT_BGRA8:
      return WGPUTextureFormat_BGRA8Unorm;
    case YK__SG_PIXELFORMAT_RGB10A2:
      return WGPUTextureFormat_RGB10A2Unorm;
    case YK__SG_PIXELFORMAT_RG11B10F:
      return WGPUTextureFormat_RG11B10Float;
    case YK__SG_PIXELFORMAT_RG32UI:
      return WGPUTextureFormat_RG32Uint;
    case YK__SG_PIXELFORMAT_RG32SI:
      return WGPUTextureFormat_RG32Sint;
    case YK__SG_PIXELFORMAT_RG32F:
      return WGPUTextureFormat_RG32Float;
    case YK__SG_PIXELFORMAT_RGBA16UI:
      return WGPUTextureFormat_RGBA16Uint;
    case YK__SG_PIXELFORMAT_RGBA16SI:
      return WGPUTextureFormat_RGBA16Sint;
    case YK__SG_PIXELFORMAT_RGBA16F:
      return WGPUTextureFormat_RGBA16Float;
    case YK__SG_PIXELFORMAT_RGBA32UI:
      return WGPUTextureFormat_RGBA32Uint;
    case YK__SG_PIXELFORMAT_RGBA32SI:
      return WGPUTextureFormat_RGBA32Sint;
    case YK__SG_PIXELFORMAT_RGBA32F:
      return WGPUTextureFormat_RGBA32Float;
    case YK__SG_PIXELFORMAT_DEPTH:
      return WGPUTextureFormat_Depth24Plus;
    case YK__SG_PIXELFORMAT_DEPTH_STENCIL:
      return WGPUTextureFormat_Depth24PlusStencil8;
    case YK__SG_PIXELFORMAT_BC1_RGBA:
      return WGPUTextureFormat_BC1RGBAUnorm;
    case YK__SG_PIXELFORMAT_BC2_RGBA:
      return WGPUTextureFormat_BC2RGBAUnorm;
    case YK__SG_PIXELFORMAT_BC3_RGBA:
      return WGPUTextureFormat_BC3RGBAUnorm;
    case YK__SG_PIXELFORMAT_BC4_R:
      return WGPUTextureFormat_BC4RUnorm;
    case YK__SG_PIXELFORMAT_BC4_RSN:
      return WGPUTextureFormat_BC4RSnorm;
    case YK__SG_PIXELFORMAT_BC5_RG:
      return WGPUTextureFormat_BC5RGUnorm;
    case YK__SG_PIXELFORMAT_BC5_RGSN:
      return WGPUTextureFormat_BC5RGSnorm;
    case YK__SG_PIXELFORMAT_BC6H_RGBF:
      return WGPUTextureFormat_BC6HRGBSfloat;
    case YK__SG_PIXELFORMAT_BC6H_RGBUF:
      return WGPUTextureFormat_BC6HRGBUfloat;
    case YK__SG_PIXELFORMAT_BC7_RGBA:
      return WGPUTextureFormat_BC7RGBAUnorm;
    /* NOT SUPPORTED */
    case YK__SG_PIXELFORMAT_R16:
    case YK__SG_PIXELFORMAT_R16SN:
    case YK__SG_PIXELFORMAT_RG16:
    case YK__SG_PIXELFORMAT_RG16SN:
    case YK__SG_PIXELFORMAT_RGBA16:
    case YK__SG_PIXELFORMAT_RGBA16SN:
    case YK__SG_PIXELFORMAT_PVRTC_RGB_2BPP:
    case YK__SG_PIXELFORMAT_PVRTC_RGB_4BPP:
    case YK__SG_PIXELFORMAT_PVRTC_RGBA_2BPP:
    case YK__SG_PIXELFORMAT_PVRTC_RGBA_4BPP:
    case YK__SG_PIXELFORMAT_ETC2_RGB8:
    case YK__SG_PIXELFORMAT_ETC2_RGB8A1:
    case YK__SG_PIXELFORMAT_ETC2_RGBA8:
    case YK__SG_PIXELFORMAT_ETC2_RG11:
    case YK__SG_PIXELFORMAT_ETC2_RG11SN:
    default:
      YK__SOKOL_UNREACHABLE;
      return WGPUTextureFormat_Force32;
  }
}
/*
FIXME ??? this isn't needed anywhere?
YK___SOKOL_PRIVATE WGPUTextureAspect _sg_wgpu_texture_aspect(yk__sg_pixel_format fmt) {
    if (yk___sg_is_valid_rendertarget_depth_format(fmt)) {
        if (!yk___sg_is_depth_stencil_format(fmt)) {
            return WGPUTextureAspect_DepthOnly;
        }
    }
    return WGPUTextureAspect_All;
}
*/
YK___SOKOL_PRIVATE WGPUCompareFunction
yk___sg_wgpu_comparefunc(yk__sg_compare_func f) {
  switch (f) {
    case YK__SG_COMPAREFUNC_NEVER:
      return WGPUCompareFunction_Never;
    case YK__SG_COMPAREFUNC_LESS:
      return WGPUCompareFunction_Less;
    case YK__SG_COMPAREFUNC_EQUAL:
      return WGPUCompareFunction_Equal;
    case YK__SG_COMPAREFUNC_LESS_EQUAL:
      return WGPUCompareFunction_LessEqual;
    case YK__SG_COMPAREFUNC_GREATER:
      return WGPUCompareFunction_Greater;
    case YK__SG_COMPAREFUNC_NOT_EQUAL:
      return WGPUCompareFunction_NotEqual;
    case YK__SG_COMPAREFUNC_GREATER_EQUAL:
      return WGPUCompareFunction_GreaterEqual;
    case YK__SG_COMPAREFUNC_ALWAYS:
      return WGPUCompareFunction_Always;
    default:
      YK__SOKOL_UNREACHABLE;
      return WGPUCompareFunction_Force32;
  }
}
YK___SOKOL_PRIVATE WGPUStencilOperation
yk___sg_wgpu_stencilop(yk__sg_stencil_op op) {
  switch (op) {
    case YK__SG_STENCILOP_KEEP:
      return WGPUStencilOperation_Keep;
    case YK__SG_STENCILOP_ZERO:
      return WGPUStencilOperation_Zero;
    case YK__SG_STENCILOP_REPLACE:
      return WGPUStencilOperation_Replace;
    case YK__SG_STENCILOP_INCR_CLAMP:
      return WGPUStencilOperation_IncrementClamp;
    case YK__SG_STENCILOP_DECR_CLAMP:
      return WGPUStencilOperation_DecrementClamp;
    case YK__SG_STENCILOP_INVERT:
      return WGPUStencilOperation_Invert;
    case YK__SG_STENCILOP_INCR_WRAP:
      return WGPUStencilOperation_IncrementWrap;
    case YK__SG_STENCILOP_DECR_WRAP:
      return WGPUStencilOperation_DecrementWrap;
    default:
      YK__SOKOL_UNREACHABLE;
      return WGPUStencilOperation_Force32;
  }
}
YK___SOKOL_PRIVATE WGPUBlendOperation yk___sg_wgpu_blendop(yk__sg_blend_op op) {
  switch (op) {
    case YK__SG_BLENDOP_ADD:
      return WGPUBlendOperation_Add;
    case YK__SG_BLENDOP_SUBTRACT:
      return WGPUBlendOperation_Subtract;
    case YK__SG_BLENDOP_REVERSE_SUBTRACT:
      return WGPUBlendOperation_ReverseSubtract;
    default:
      YK__SOKOL_UNREACHABLE;
      return WGPUBlendOperation_Force32;
  }
}
YK___SOKOL_PRIVATE WGPUBlendFactor
yk___sg_wgpu_blendfactor(yk__sg_blend_factor f) {
  switch (f) {
    case YK__SG_BLENDFACTOR_ZERO:
      return WGPUBlendFactor_Zero;
    case YK__SG_BLENDFACTOR_ONE:
      return WGPUBlendFactor_One;
    case YK__SG_BLENDFACTOR_SRC_COLOR:
      return WGPUBlendFactor_SrcColor;
    case YK__SG_BLENDFACTOR_ONE_MINUS_SRC_COLOR:
      return WGPUBlendFactor_OneMinusSrcColor;
    case YK__SG_BLENDFACTOR_SRC_ALPHA:
      return WGPUBlendFactor_SrcAlpha;
    case YK__SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA:
      return WGPUBlendFactor_OneMinusSrcAlpha;
    case YK__SG_BLENDFACTOR_DST_COLOR:
      return WGPUBlendFactor_DstColor;
    case YK__SG_BLENDFACTOR_ONE_MINUS_DST_COLOR:
      return WGPUBlendFactor_OneMinusDstColor;
    case YK__SG_BLENDFACTOR_DST_ALPHA:
      return WGPUBlendFactor_DstAlpha;
    case YK__SG_BLENDFACTOR_ONE_MINUS_DST_ALPHA:
      return WGPUBlendFactor_OneMinusDstAlpha;
    case YK__SG_BLENDFACTOR_SRC_ALPHA_SATURATED:
      return WGPUBlendFactor_SrcAlphaSaturated;
    case YK__SG_BLENDFACTOR_BLEND_COLOR:
      return WGPUBlendFactor_BlendColor;
    case YK__SG_BLENDFACTOR_ONE_MINUS_BLEND_COLOR:
      return WGPUBlendFactor_OneMinusBlendColor;
    /* FIXME: separate blend alpha value not supported? */
    case YK__SG_BLENDFACTOR_BLEND_ALPHA:
      return WGPUBlendFactor_BlendColor;
    case YK__SG_BLENDFACTOR_ONE_MINUS_BLEND_ALPHA:
      return WGPUBlendFactor_OneMinusBlendColor;
    default:
      YK__SOKOL_UNREACHABLE;
      return WGPUBlendFactor_Force32;
  }
}
YK___SOKOL_PRIVATE WGPUColorWriteMaskFlags
yk___sg_wgpu_colorwritemask(uint8_t m) {
  WGPUColorWriteMaskFlags res = 0;
  if (0 != (m & YK__SG_COLORMASK_R)) { res |= WGPUColorWriteMask_Red; }
  if (0 != (m & YK__SG_COLORMASK_G)) { res |= WGPUColorWriteMask_Green; }
  if (0 != (m & YK__SG_COLORMASK_B)) { res |= WGPUColorWriteMask_Blue; }
  if (0 != (m & YK__SG_COLORMASK_A)) { res |= WGPUColorWriteMask_Alpha; }
  return res;
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_init_caps(void) {
  yk___sg.backend = YK__SG_BACKEND_WGPU;
  yk___sg.features.instancing = true;
  yk___sg.features.origin_top_left = true;
  yk___sg.features.multiple_render_targets = true;
  yk___sg.features.msaa_render_targets = true;
  yk___sg.features.imagetype_3d = true;
  yk___sg.features.imagetype_array = true;
  yk___sg.features.image_clamp_to_border = false;
  yk___sg.features.mrt_independent_blend_state = true;
  yk___sg.features.mrt_independent_write_mask = true;
  /* FIXME: max images size??? */
  yk___sg.limits.max_image_size_2d = 8 * 1024;
  yk___sg.limits.max_image_size_cube = 8 * 1024;
  yk___sg.limits.max_image_size_3d = 2 * 1024;
  yk___sg.limits.max_image_size_array = 8 * 1024;
  yk___sg.limits.max_image_array_layers = 2 * 1024;
  yk___sg.limits.max_vertex_attrs = YK__SG_MAX_VERTEX_ATTRIBUTES;
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_R8]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_R8SN]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_R8UI]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_R8SI]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_R16UI]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_R16SI]);
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_R16F]);
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RG8]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_RG8SN]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RG8UI]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RG8SI]);
  yk___sg_pixelformat_sr(&yk___sg.formats[YK__SG_PIXELFORMAT_R32UI]);
  yk___sg_pixelformat_sr(&yk___sg.formats[YK__SG_PIXELFORMAT_R32SI]);
  yk___sg_pixelformat_sbr(&yk___sg.formats[YK__SG_PIXELFORMAT_R32F]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RG16UI]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RG16SI]);
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RG16F]);
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA8]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA8SN]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA8UI]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA8SI]);
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_BGRA8]);
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RGB10A2]);
  yk___sg_pixelformat_sr(&yk___sg.formats[YK__SG_PIXELFORMAT_RG32UI]);
  yk___sg_pixelformat_sr(&yk___sg.formats[YK__SG_PIXELFORMAT_RG32SI]);
  yk___sg_pixelformat_sbr(&yk___sg.formats[YK__SG_PIXELFORMAT_RG32F]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA16UI]);
  yk___sg_pixelformat_srm(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA16SI]);
  yk___sg_pixelformat_all(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA16F]);
  yk___sg_pixelformat_sr(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA32UI]);
  yk___sg_pixelformat_sr(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA32SI]);
  yk___sg_pixelformat_sr(&yk___sg.formats[YK__SG_PIXELFORMAT_RGBA32F]);
  yk___sg_pixelformat_srmd(&yk___sg.formats[YK__SG_PIXELFORMAT_DEPTH]);
  yk___sg_pixelformat_srmd(&yk___sg.formats[YK__SG_PIXELFORMAT_DEPTH_STENCIL]);
  /* FIXME FIXME FIXME: need to check if BC texture compression is
        actually supported, currently the WebGPU C-API doesn't allow this
    */
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC1_RGBA]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC2_RGBA]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC3_RGBA]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC4_R]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC4_RSN]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC5_RG]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC5_RGSN]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC6H_RGBF]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC6H_RGBUF]);
  yk___sg_pixelformat_sf(&yk___sg.formats[YK__SG_PIXELFORMAT_BC7_RGBA]);
}
/*
    WGPU uniform buffer pool implementation:

    At start of frame, a mapped buffer is grabbed from the pool,
    or a new buffer is created if there is no mapped buffer available.

    At end of frame, the current buffer is unmapped before queue submit,
    and async-mapped immediately again.

    UNIFORM BUFFER FIXME:

    - As per WebGPU spec, it should be possible to create a Uniform|MapWrite
      buffer, but this isn't currently allowed in Dawn.
*/
YK___SOKOL_PRIVATE void yk___sg_wgpu_ubpool_init(const yk__sg_desc *desc) {
  /* Add the max-uniform-update size (64 KB) to the requested buffer size,
       this is to prevent validation errors in the WebGPU implementation
       if the entire buffer size is used per frame. 64 KB is the allowed
       max uniform update size on NVIDIA
    */
  yk___sg.wgpu.ub.num_bytes =
      desc->uniform_buffer_size + YK___SG_WGPU_MAX_UNIFORM_UPDATE_SIZE;
  WGPUBufferDescriptor ub_desc;
  memset(&ub_desc, 0, sizeof(ub_desc));
  ub_desc.size = yk___sg.wgpu.ub.num_bytes;
  ub_desc.usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;
  yk___sg.wgpu.ub.buf = wgpuDeviceCreateBuffer(yk___sg.wgpu.dev, &ub_desc);
  YK__SOKOL_ASSERT(yk___sg.wgpu.ub.buf);
  WGPUBindGroupLayoutBinding ub_bglb_desc[YK__SG_NUM_SHADER_STAGES]
                                         [YK__SG_MAX_SHADERSTAGE_UBS];
  memset(ub_bglb_desc, 0, sizeof(ub_bglb_desc));
  for (int stage_index = 0; stage_index < YK__SG_NUM_SHADER_STAGES;
       stage_index++) {
    WGPUShaderStage vis = (stage_index == YK__SG_SHADERSTAGE_VS)
                              ? WGPUShaderStage_Vertex
                              : WGPUShaderStage_Fragment;
    for (int ub_index = 0; ub_index < YK__SG_MAX_SHADERSTAGE_UBS; ub_index++) {
      int bind_index = stage_index * YK__SG_MAX_SHADERSTAGE_UBS + ub_index;
      ub_bglb_desc[stage_index][ub_index].binding = bind_index;
      ub_bglb_desc[stage_index][ub_index].visibility = vis;
      ub_bglb_desc[stage_index][ub_index].type = WGPUBindingType_UniformBuffer;
      ub_bglb_desc[stage_index][ub_index].hasDynamicOffset = true;
    }
  }
  WGPUBindGroupLayoutDescriptor ub_bgl_desc;
  memset(&ub_bgl_desc, 0, sizeof(ub_bgl_desc));
  ub_bgl_desc.bindingCount =
      YK__SG_NUM_SHADER_STAGES * YK__SG_MAX_SHADERSTAGE_UBS;
  ub_bgl_desc.bindings = &ub_bglb_desc[0][0];
  yk___sg.wgpu.ub.bindgroup_layout =
      wgpuDeviceCreateBindGroupLayout(yk___sg.wgpu.dev, &ub_bgl_desc);
  YK__SOKOL_ASSERT(yk___sg.wgpu.ub.bindgroup_layout);
  WGPUBindGroupBinding ub_bgb[YK__SG_NUM_SHADER_STAGES]
                             [YK__SG_MAX_SHADERSTAGE_UBS];
  memset(ub_bgb, 0, sizeof(ub_bgb));
  for (int stage_index = 0; stage_index < YK__SG_NUM_SHADER_STAGES;
       stage_index++) {
    for (int ub_index = 0; ub_index < YK__SG_MAX_SHADERSTAGE_UBS; ub_index++) {
      int bind_index = stage_index * YK__SG_MAX_SHADERSTAGE_UBS + ub_index;
      ub_bgb[stage_index][ub_index].binding = bind_index;
      ub_bgb[stage_index][ub_index].buffer = yk___sg.wgpu.ub.buf;
      // FIXME FIXME FIXME FIXME: HACK FOR VALIDATION BUG IN DAWN
      ub_bgb[stage_index][ub_index].size = (1 << 16);
    }
  }
  WGPUBindGroupDescriptor bg_desc;
  memset(&bg_desc, 0, sizeof(bg_desc));
  bg_desc.layout = yk___sg.wgpu.ub.bindgroup_layout;
  bg_desc.bindingCount = YK__SG_NUM_SHADER_STAGES * YK__SG_MAX_SHADERSTAGE_UBS;
  bg_desc.bindings = &ub_bgb[0][0];
  yk___sg.wgpu.ub.bindgroup =
      wgpuDeviceCreateBindGroup(yk___sg.wgpu.dev, &bg_desc);
  YK__SOKOL_ASSERT(yk___sg.wgpu.ub.bindgroup);
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_ubpool_discard(void) {
  if (yk___sg.wgpu.ub.buf) {
    wgpuBufferRelease(yk___sg.wgpu.ub.buf);
    yk___sg.wgpu.ub.buf = 0;
  }
  if (yk___sg.wgpu.ub.bindgroup) {
    wgpuBindGroupRelease(yk___sg.wgpu.ub.bindgroup);
    yk___sg.wgpu.ub.bindgroup = 0;
  }
  if (yk___sg.wgpu.ub.bindgroup_layout) {
    wgpuBindGroupLayoutRelease(yk___sg.wgpu.ub.bindgroup_layout);
    yk___sg.wgpu.ub.bindgroup_layout = 0;
  }
  for (int i = 0; i < yk___sg.wgpu.ub.stage.num; i++) {
    if (yk___sg.wgpu.ub.stage.buf[i]) {
      wgpuBufferRelease(yk___sg.wgpu.ub.stage.buf[i]);
      yk___sg.wgpu.ub.stage.buf[i] = 0;
      yk___sg.wgpu.ub.stage.ptr[i] = 0;
    }
  }
}
YK___SOKOL_PRIVATE void
yk___sg_wgpu_ubpool_mapped_callback(WGPUBufferMapAsyncStatus status, void *data,
                                    uint64_t data_len, void *user_data) {
  if (!yk___sg.wgpu.valid) { return; }
  /* FIXME: better handling for this */
  if (WGPUBufferMapAsyncStatus_Success != status) {
    YK__SOKOL_LOG("Mapping uniform buffer failed!\n");
    YK__SOKOL_ASSERT(false);
  }
  YK__SOKOL_ASSERT(data && (data_len == yk___sg.wgpu.ub.num_bytes));
  int index = (int) (intptr_t) user_data;
  YK__SOKOL_ASSERT(index < yk___sg.wgpu.ub.stage.num);
  YK__SOKOL_ASSERT(0 == yk___sg.wgpu.ub.stage.ptr[index]);
  yk___sg.wgpu.ub.stage.ptr[index] = (uint8_t *) data;
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_ubpool_next_frame(bool first_frame) {
  /* immediately request a new mapping for the last frame's current staging buffer */
  if (!first_frame) {
    WGPUBuffer ub_src = yk___sg.wgpu.ub.stage.buf[yk___sg.wgpu.ub.stage.cur];
    wgpuBufferMapWriteAsync(ub_src, yk___sg_wgpu_ubpool_mapped_callback,
                            (void *) (intptr_t) yk___sg.wgpu.ub.stage.cur);
  }
  /* rewind per-frame offsets */
  yk___sg.wgpu.ub.offset = 0;
  memset(&yk___sg.wgpu.ub.bind_offsets, 0,
         sizeof(yk___sg.wgpu.ub.bind_offsets));
  /* check if a mapped staging buffer is available, otherwise create one */
  for (int i = 0; i < yk___sg.wgpu.ub.stage.num; i++) {
    if (yk___sg.wgpu.ub.stage.ptr[i]) {
      yk___sg.wgpu.ub.stage.cur = i;
      return;
    }
  }
  /* no mapped uniform buffer available, create one */
  YK__SOKOL_ASSERT(yk___sg.wgpu.ub.stage.num <
                   YK___SG_WGPU_STAGING_PIPELINE_SIZE);
  yk___sg.wgpu.ub.stage.cur = yk___sg.wgpu.ub.stage.num++;
  const int cur = yk___sg.wgpu.ub.stage.cur;
  WGPUBufferDescriptor desc;
  memset(&desc, 0, sizeof(desc));
  desc.size = yk___sg.wgpu.ub.num_bytes;
  desc.usage = WGPUBufferUsage_CopySrc | WGPUBufferUsage_MapWrite;
  WGPUCreateBufferMappedResult res =
      wgpuDeviceCreateBufferMapped(yk___sg.wgpu.dev, &desc);
  yk___sg.wgpu.ub.stage.buf[cur] = res.buffer;
  yk___sg.wgpu.ub.stage.ptr[cur] = (uint8_t *) res.data;
  YK__SOKOL_ASSERT(yk___sg.wgpu.ub.stage.buf[cur]);
  YK__SOKOL_ASSERT(yk___sg.wgpu.ub.stage.ptr[cur]);
  YK__SOKOL_ASSERT(res.dataLength == yk___sg.wgpu.ub.num_bytes);
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_ubpool_flush(void) {
  /* unmap staging buffer and copy to uniform buffer */
  const int cur = yk___sg.wgpu.ub.stage.cur;
  YK__SOKOL_ASSERT(yk___sg.wgpu.ub.stage.ptr[cur]);
  yk___sg.wgpu.ub.stage.ptr[cur] = 0;
  WGPUBuffer src_buf = yk___sg.wgpu.ub.stage.buf[cur];
  wgpuBufferUnmap(src_buf);
  if (yk___sg.wgpu.ub.offset > 0) {
    WGPUBuffer dst_buf = yk___sg.wgpu.ub.buf;
    wgpuCommandEncoderCopyBufferToBuffer(yk___sg.wgpu.render_cmd_enc, src_buf,
                                         0, dst_buf, 0, yk___sg.wgpu.ub.offset);
  }
}
/* helper function to compute number of bytes needed in staging buffer to copy image data */
YK___SOKOL_PRIVATE uint32_t
yk___sg_wgpu_image_data_buffer_size(const yk___sg_image_t *img) {
  uint32_t num_bytes = 0;
  const uint32_t num_faces = (img->cmn.type == YK__SG_IMAGETYPE_CUBE) ? 6 : 1;
  const uint32_t num_slices =
      (img->cmn.type == YK__SG_IMAGETYPE_ARRAY) ? img->cmn.num_slices : 1;
  for (int mip_index = 0; mip_index < img->cmn.num_mipmaps; mip_index++) {
    const uint32_t mip_width = yk___sg_max(img->cmn.width >> mip_index, 1);
    const uint32_t mip_height = yk___sg_max(img->cmn.height >> mip_index, 1);
    /* row-pitch must be 256-aligend */
    const uint32_t bytes_per_slice =
        yk___sg_surface_pitch(img->cmn.pixel_format, mip_width, mip_height,
                              YK___SG_WGPU_ROWPITCH_ALIGN);
    num_bytes += bytes_per_slice * num_slices * num_faces;
  }
  return num_bytes;
}
/* helper function to copy image data into a texture via a staging buffer, returns number of
   bytes copied
*/
YK___SOKOL_PRIVATE uint32_t yk___sg_wgpu_copy_image_data(
    WGPUBuffer stg_buf, uint8_t *stg_base_ptr, uint32_t stg_base_offset,
    yk___sg_image_t *img, const yk__sg_image_data *data) {
  YK__SOKOL_ASSERT(yk___sg.wgpu.staging_cmd_enc);
  YK__SOKOL_ASSERT(stg_buf && stg_base_ptr);
  YK__SOKOL_ASSERT(img);
  YK__SOKOL_ASSERT(data);
  uint32_t stg_offset = stg_base_offset;
  const uint32_t num_faces = (img->cmn.type == YK__SG_IMAGETYPE_CUBE) ? 6 : 1;
  const uint32_t num_slices =
      (img->cmn.type == YK__SG_IMAGETYPE_ARRAY) ? img->cmn.num_slices : 1;
  const yk__sg_pixel_format fmt = img->cmn.pixel_format;
  WGPUBufferCopyView src_view;
  memset(&src_view, 0, sizeof(src_view));
  src_view.buffer = stg_buf;
  WGPUTextureCopyView dst_view;
  memset(&dst_view, 0, sizeof(dst_view));
  dst_view.texture = img->wgpu.tex;
  WGPUExtent3D extent;
  memset(&extent, 0, sizeof(extent));
  for (uint32_t face_index = 0; face_index < num_faces; face_index++) {
    for (uint32_t mip_index = 0; mip_index < (uint32_t) img->cmn.num_mipmaps;
         mip_index++) {
      YK__SOKOL_ASSERT(data->subimage[face_index][mip_index].ptr);
      YK__SOKOL_ASSERT(data->subimage[face_index][mip_index].size > 0);
      const uint8_t *src_base_ptr =
          (const uint8_t *) data->subimage[face_index][mip_index].ptr;
      YK__SOKOL_ASSERT(src_base_ptr);
      uint8_t *dst_base_ptr = stg_base_ptr + stg_offset;
      const uint32_t mip_width = yk___sg_max(img->cmn.width >> mip_index, 1);
      const uint32_t mip_height = yk___sg_max(img->cmn.height >> mip_index, 1);
      const uint32_t mip_depth =
          (img->cmn.type == YK__SG_IMAGETYPE_3D)
              ? yk___sg_max(img->cmn.num_slices >> mip_index, 1)
              : 1;
      const uint32_t num_rows = yk___sg_num_rows(fmt, mip_height);
      const uint32_t src_bytes_per_row = yk___sg_row_pitch(fmt, mip_width, 1);
      const uint32_t dst_bytes_per_row =
          yk___sg_row_pitch(fmt, mip_width, YK___SG_WGPU_ROWPITCH_ALIGN);
      const uint32_t src_bytes_per_slice =
          yk___sg_surface_pitch(fmt, mip_width, mip_height, 1);
      const uint32_t dst_bytes_per_slice = yk___sg_surface_pitch(
          fmt, mip_width, mip_height, YK___SG_WGPU_ROWPITCH_ALIGN);
      YK__SOKOL_ASSERT((uint32_t) data->subimage[face_index][mip_index].size ==
                       (src_bytes_per_slice * num_slices));
      YK__SOKOL_ASSERT(src_bytes_per_row <= dst_bytes_per_row);
      YK__SOKOL_ASSERT(src_bytes_per_slice == (src_bytes_per_row * num_rows));
      YK__SOKOL_ASSERT(dst_bytes_per_slice == (dst_bytes_per_row * num_rows));
      YK___SOKOL_UNUSED(src_bytes_per_slice);
      /* copy data into mapped staging buffer */
      if (src_bytes_per_row == dst_bytes_per_row) {
        /* can do a single memcpy */
        uint32_t num_bytes = data->subimage[face_index][mip_index].size;
        memcpy(dst_base_ptr, src_base_ptr, num_bytes);
      } else {
        /* src/dst pitch doesn't match, need to copy row by row */
        uint8_t *dst_ptr = dst_base_ptr;
        const uint8_t *src_ptr = src_base_ptr;
        for (uint32_t slice_index = 0; slice_index < num_slices;
             slice_index++) {
          YK__SOKOL_ASSERT(dst_ptr ==
                           dst_base_ptr + slice_index * dst_bytes_per_slice);
          for (uint32_t row_index = 0; row_index < num_rows; row_index++) {
            memcpy(dst_ptr, src_ptr, src_bytes_per_row);
            src_ptr += src_bytes_per_row;
            dst_ptr += dst_bytes_per_row;
          }
        }
      }
      /* record the staging copy operation into command encoder */
      src_view.imageHeight = mip_height;
      src_view.rowPitch = dst_bytes_per_row;
      dst_view.mipLevel = mip_index;
      extent.width = mip_width;
      extent.height = mip_height;
      extent.depth = mip_depth;
      YK__SOKOL_ASSERT((img->cmn.type != YK__SG_IMAGETYPE_CUBE) ||
                       (num_slices == 1));
      for (uint32_t slice_index = 0; slice_index < num_slices; slice_index++) {
        const uint32_t layer_index = (img->cmn.type == YK__SG_IMAGETYPE_ARRAY)
                                         ? slice_index
                                         : face_index;
        src_view.offset = stg_offset;
        dst_view.arrayLayer = layer_index;
        wgpuCommandEncoderCopyBufferToTexture(yk___sg.wgpu.staging_cmd_enc,
                                              &src_view, &dst_view, &extent);
        stg_offset += dst_bytes_per_slice;
        YK__SOKOL_ASSERT(stg_offset <= yk___sg.wgpu.staging.num_bytes);
      }
    }
  }
  YK__SOKOL_ASSERT(stg_offset >= stg_base_offset);
  return (stg_offset - stg_base_offset);
}
/*
    The WGPU staging buffer implementation:

    Very similar to the uniform buffer pool, there's a pool of big
    per-frame staging buffers, each must be big enough to hold
    all data uploaded to dynamic resources for one frame.

    Staging buffers are created on demand and reused, because the
    'frame pipeline depth' of WGPU isn't predictable.

    The difference to the uniform buffer system is that there isn't
    a 1:1 relationship for source- and destination for the
    data-copy operation. There's always one staging buffer as copy-source
    per frame, but many copy-destinations (regular vertex/index buffers
    or images). Instead of one big copy-operation at the end of the frame,
    multiple copy-operations will be written throughout the frame.
*/
YK___SOKOL_PRIVATE void yk___sg_wgpu_staging_init(const yk__sg_desc *desc) {
  YK__SOKOL_ASSERT(desc && (desc->staging_buffer_size > 0));
  yk___sg.wgpu.staging.num_bytes = desc->staging_buffer_size;
  /* there's actually nothing more to do here */
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_staging_discard(void) {
  for (int i = 0; i < yk___sg.wgpu.staging.num; i++) {
    if (yk___sg.wgpu.staging.buf[i]) {
      wgpuBufferRelease(yk___sg.wgpu.staging.buf[i]);
      yk___sg.wgpu.staging.buf[i] = 0;
      yk___sg.wgpu.staging.ptr[i] = 0;
    }
  }
}
YK___SOKOL_PRIVATE void
yk___sg_wgpu_staging_mapped_callback(WGPUBufferMapAsyncStatus status,
                                     void *data, uint64_t data_len,
                                     void *user_data) {
  if (!yk___sg.wgpu.valid) { return; }
  /* FIXME: better handling for this */
  if (WGPUBufferMapAsyncStatus_Success != status) {
    YK__SOKOL_ASSERT("Mapping staging buffer failed!\n");
    YK__SOKOL_ASSERT(false);
  }
  YK__SOKOL_ASSERT(data && (data_len == yk___sg.wgpu.staging.num_bytes));
  int index = (int) (intptr_t) user_data;
  YK__SOKOL_ASSERT(index < yk___sg.wgpu.staging.num);
  YK__SOKOL_ASSERT(0 == yk___sg.wgpu.staging.ptr[index]);
  yk___sg.wgpu.staging.ptr[index] = (uint8_t *) data;
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_staging_next_frame(bool first_frame) {
  /* immediately request a new mapping for the last frame's current staging buffer */
  if (!first_frame) {
    WGPUBuffer cur_buf = yk___sg.wgpu.staging.buf[yk___sg.wgpu.staging.cur];
    wgpuBufferMapWriteAsync(cur_buf, yk___sg_wgpu_staging_mapped_callback,
                            (void *) (intptr_t) yk___sg.wgpu.staging.cur);
  }
  /* rewind staging-buffer offset */
  yk___sg.wgpu.staging.offset = 0;
  /* check if mapped staging buffer is available, otherwise create one */
  for (int i = 0; i < yk___sg.wgpu.staging.num; i++) {
    if (yk___sg.wgpu.staging.ptr[i]) {
      yk___sg.wgpu.staging.cur = i;
      return;
    }
  }
  /* no mapped buffer available, create one */
  YK__SOKOL_ASSERT(yk___sg.wgpu.staging.num <
                   YK___SG_WGPU_STAGING_PIPELINE_SIZE);
  yk___sg.wgpu.staging.cur = yk___sg.wgpu.staging.num++;
  const int cur = yk___sg.wgpu.staging.cur;
  WGPUBufferDescriptor desc;
  memset(&desc, 0, sizeof(desc));
  desc.size = yk___sg.wgpu.staging.num_bytes;
  desc.usage = WGPUBufferUsage_CopySrc | WGPUBufferUsage_MapWrite;
  WGPUCreateBufferMappedResult res =
      wgpuDeviceCreateBufferMapped(yk___sg.wgpu.dev, &desc);
  yk___sg.wgpu.staging.buf[cur] = res.buffer;
  yk___sg.wgpu.staging.ptr[cur] = (uint8_t *) res.data;
  YK__SOKOL_ASSERT(yk___sg.wgpu.staging.buf[cur]);
  YK__SOKOL_ASSERT(yk___sg.wgpu.staging.ptr[cur]);
  YK__SOKOL_ASSERT(res.dataLength == yk___sg.wgpu.staging.num_bytes);
}
YK___SOKOL_PRIVATE uint32_t
yk___sg_wgpu_staging_copy_to_buffer(WGPUBuffer dst_buf, uint32_t dst_buf_offset,
                                    const void *data, uint32_t data_num_bytes) {
  /* Copy a chunk of data into the staging buffer, and record a blit-operation into
        the command encoder, bump the offset for the next data chunk, return 0 if there
        was not enough room in the staging buffer, return the number of actually
        copied bytes on success.

        NOTE: that the number of staging bytes to be copied must be a multiple of 4.

    */
  YK__SOKOL_ASSERT(yk___sg.wgpu.staging_cmd_enc);
  YK__SOKOL_ASSERT((dst_buf_offset & 3) == 0);
  YK__SOKOL_ASSERT(data_num_bytes > 0);
  uint32_t copy_num_bytes = yk___sg_roundup(data_num_bytes, 4);
  if ((yk___sg.wgpu.staging.offset + copy_num_bytes) >=
      yk___sg.wgpu.staging.num_bytes) {
    YK__SOKOL_LOG("WGPU: Per frame staging buffer full (in "
                  "yk___sg_wgpu_staging_copy_to_buffer())!\n");
    return false;
  }
  const int cur = yk___sg.wgpu.staging.cur;
  YK__SOKOL_ASSERT(yk___sg.wgpu.staging.ptr[cur]);
  uint32_t stg_buf_offset = yk___sg.wgpu.staging.offset;
  uint8_t *stg_ptr = yk___sg.wgpu.staging.ptr[cur] + stg_buf_offset;
  memcpy(stg_ptr, data, data_num_bytes);
  WGPUBuffer stg_buf = yk___sg.wgpu.staging.buf[cur];
  wgpuCommandEncoderCopyBufferToBuffer(yk___sg.wgpu.staging_cmd_enc, stg_buf,
                                       stg_buf_offset, dst_buf, dst_buf_offset,
                                       copy_num_bytes);
  yk___sg.wgpu.staging.offset = stg_buf_offset + copy_num_bytes;
  return copy_num_bytes;
}
YK___SOKOL_PRIVATE bool
yk___sg_wgpu_staging_copy_to_texture(yk___sg_image_t *img,
                                     const yk__sg_image_data *data) {
  /* similar to yk___sg_wgpu_staging_copy_to_buffer(), but with image data instead */
  YK__SOKOL_ASSERT(yk___sg.wgpu.staging_cmd_enc);
  uint32_t num_bytes = yk___sg_wgpu_image_data_buffer_size(img);
  if ((yk___sg.wgpu.staging.offset + num_bytes) >=
      yk___sg.wgpu.staging.num_bytes) {
    YK__SOKOL_LOG("WGPU: Per frame staging buffer full (in "
                  "yk___sg_wgpu_staging_copy_to_texture)!\n");
    return false;
  }
  const int cur = yk___sg.wgpu.staging.cur;
  YK__SOKOL_ASSERT(yk___sg.wgpu.staging.ptr[cur]);
  uint32_t stg_offset = yk___sg.wgpu.staging.offset;
  uint8_t *stg_ptr = yk___sg.wgpu.staging.ptr[cur];
  WGPUBuffer stg_buf = yk___sg.wgpu.staging.buf[cur];
  uint32_t bytes_copied =
      yk___sg_wgpu_copy_image_data(stg_buf, stg_ptr, stg_offset, img, data);
  YK___SOKOL_UNUSED(bytes_copied);
  YK__SOKOL_ASSERT(bytes_copied == num_bytes);
  yk___sg.wgpu.staging.offset =
      yk___sg_roundup(stg_offset + num_bytes, YK___SG_WGPU_STAGING_ALIGN);
  return true;
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_staging_unmap(void) {
  /* called at end of frame before queue-submit */
  const int cur = yk___sg.wgpu.staging.cur;
  YK__SOKOL_ASSERT(yk___sg.wgpu.staging.ptr[cur]);
  yk___sg.wgpu.staging.ptr[cur] = 0;
  wgpuBufferUnmap(yk___sg.wgpu.staging.buf[cur]);
}
/*--- WGPU sampler cache functions ---*/
YK___SOKOL_PRIVATE void
yk___sg_wgpu_init_sampler_cache(const yk__sg_desc *desc) {
  YK__SOKOL_ASSERT(desc->sampler_cache_size > 0);
  yk___sg_smpcache_init(&yk___sg.wgpu.sampler_cache, desc->sampler_cache_size);
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_destroy_sampler_cache(void) {
  YK__SOKOL_ASSERT(yk___sg.wgpu.sampler_cache.items);
  YK__SOKOL_ASSERT(yk___sg.wgpu.sampler_cache.num_items <=
                   yk___sg.wgpu.sampler_cache.capacity);
  for (int i = 0; i < yk___sg.wgpu.sampler_cache.num_items; i++) {
    wgpuSamplerRelease(
        (WGPUSampler) yk___sg_smpcache_sampler(&yk___sg.wgpu.sampler_cache, i));
  }
  yk___sg_smpcache_discard(&yk___sg.wgpu.sampler_cache);
}
YK___SOKOL_PRIVATE WGPUSampler
yk___sg_wgpu_create_sampler(const yk__sg_image_desc *img_desc) {
  YK__SOKOL_ASSERT(img_desc);
  int index = yk___sg_smpcache_find_item(&yk___sg.wgpu.sampler_cache, img_desc);
  if (index >= 0) {
    /* reuse existing sampler */
    return (WGPUSampler) yk___sg_smpcache_sampler(&yk___sg.wgpu.sampler_cache,
                                                  index);
  } else {
    /* create a new WGPU sampler and add to sampler cache */
    /* FIXME: anisotropic filtering not supported? */
    WGPUSamplerDescriptor smp_desc;
    memset(&smp_desc, 0, sizeof(smp_desc));
    smp_desc.addressModeU = yk___sg_wgpu_sampler_addrmode(img_desc->wrap_u);
    smp_desc.addressModeV = yk___sg_wgpu_sampler_addrmode(img_desc->wrap_v);
    smp_desc.addressModeW = yk___sg_wgpu_sampler_addrmode(img_desc->wrap_w);
    smp_desc.magFilter =
        yk___sg_wgpu_sampler_minmagfilter(img_desc->mag_filter);
    smp_desc.minFilter =
        yk___sg_wgpu_sampler_minmagfilter(img_desc->min_filter);
    smp_desc.mipmapFilter =
        yk___sg_wgpu_sampler_mipfilter(img_desc->min_filter);
    smp_desc.lodMinClamp = img_desc->min_lod;
    smp_desc.lodMaxClamp = img_desc->max_lod;
    WGPUSampler smp = wgpuDeviceCreateSampler(yk___sg.wgpu.dev, &smp_desc);
    YK__SOKOL_ASSERT(smp);
    yk___sg_smpcache_add_item(&yk___sg.wgpu.sampler_cache, img_desc,
                              (uintptr_t) smp);
    return smp;
  }
}
/*--- WGPU backend API functions ---*/
YK___SOKOL_PRIVATE void yk___sg_wgpu_setup_backend(const yk__sg_desc *desc) {
  YK__SOKOL_ASSERT(desc);
  YK__SOKOL_ASSERT(desc->context.wgpu.device);
  YK__SOKOL_ASSERT(desc->context.wgpu.render_view_cb ||
                   desc->context.wgpu.render_view_userdata_cb);
  YK__SOKOL_ASSERT(desc->context.wgpu.resolve_view_cb ||
                   desc->context.wgpu.resolve_view_userdata_cb);
  YK__SOKOL_ASSERT(desc->context.wgpu.depth_stencil_view_cb ||
                   desc->context.wgpu.depth_stencil_view_userdata_cb);
  YK__SOKOL_ASSERT(desc->uniform_buffer_size > 0);
  YK__SOKOL_ASSERT(desc->staging_buffer_size > 0);
  yk___sg.backend = YK__SG_BACKEND_WGPU;
  yk___sg.wgpu.valid = true;
  yk___sg.wgpu.dev = (WGPUDevice) desc->context.wgpu.device;
  yk___sg.wgpu.render_view_cb =
      (WGPUTextureView(*)(void)) desc->context.wgpu.render_view_cb;
  yk___sg.wgpu.render_view_userdata_cb =
      (WGPUTextureView(*)(void *)) desc->context.wgpu.render_view_userdata_cb;
  yk___sg.wgpu.resolve_view_cb =
      (WGPUTextureView(*)(void)) desc->context.wgpu.resolve_view_cb;
  yk___sg.wgpu.resolve_view_userdata_cb =
      (WGPUTextureView(*)(void *)) desc->context.wgpu.resolve_view_userdata_cb;
  yk___sg.wgpu.depth_stencil_view_cb =
      (WGPUTextureView(*)(void)) desc->context.wgpu.depth_stencil_view_cb;
  yk___sg.wgpu.depth_stencil_view_userdata_cb = (WGPUTextureView(*)(
      void *)) desc->context.wgpu.depth_stencil_view_userdata_cb;
  yk___sg.wgpu.user_data = desc->context.wgpu.user_data;
  yk___sg.wgpu.queue = wgpuDeviceCreateQueue(yk___sg.wgpu.dev);
  YK__SOKOL_ASSERT(yk___sg.wgpu.queue);
  /* setup WebGPU features and limits */
  yk___sg_wgpu_init_caps();
  /* setup the sampler cache, uniform and staging buffer pools */
  yk___sg_wgpu_init_sampler_cache(&yk___sg.desc);
  yk___sg_wgpu_ubpool_init(desc);
  yk___sg_wgpu_ubpool_next_frame(true);
  yk___sg_wgpu_staging_init(desc);
  yk___sg_wgpu_staging_next_frame(true);
  /* create an empty bind group for shader stages without bound images */
  WGPUBindGroupLayoutDescriptor bgl_desc;
  memset(&bgl_desc, 0, sizeof(bgl_desc));
  WGPUBindGroupLayout empty_bgl =
      wgpuDeviceCreateBindGroupLayout(yk___sg.wgpu.dev, &bgl_desc);
  YK__SOKOL_ASSERT(empty_bgl);
  WGPUBindGroupDescriptor bg_desc;
  memset(&bg_desc, 0, sizeof(bg_desc));
  bg_desc.layout = empty_bgl;
  yk___sg.wgpu.empty_bind_group =
      wgpuDeviceCreateBindGroup(yk___sg.wgpu.dev, &bg_desc);
  YK__SOKOL_ASSERT(yk___sg.wgpu.empty_bind_group);
  wgpuBindGroupLayoutRelease(empty_bgl);
  /* create initial per-frame command encoders */
  WGPUCommandEncoderDescriptor cmd_enc_desc;
  memset(&cmd_enc_desc, 0, sizeof(cmd_enc_desc));
  yk___sg.wgpu.render_cmd_enc =
      wgpuDeviceCreateCommandEncoder(yk___sg.wgpu.dev, &cmd_enc_desc);
  YK__SOKOL_ASSERT(yk___sg.wgpu.render_cmd_enc);
  yk___sg.wgpu.staging_cmd_enc =
      wgpuDeviceCreateCommandEncoder(yk___sg.wgpu.dev, &cmd_enc_desc);
  YK__SOKOL_ASSERT(yk___sg.wgpu.staging_cmd_enc);
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_discard_backend(void) {
  YK__SOKOL_ASSERT(yk___sg.wgpu.valid);
  YK__SOKOL_ASSERT(yk___sg.wgpu.render_cmd_enc);
  YK__SOKOL_ASSERT(yk___sg.wgpu.staging_cmd_enc);
  yk___sg.wgpu.valid = false;
  yk___sg_wgpu_ubpool_discard();
  yk___sg_wgpu_staging_discard();
  yk___sg_wgpu_destroy_sampler_cache();
  wgpuBindGroupRelease(yk___sg.wgpu.empty_bind_group);
  wgpuCommandEncoderRelease(yk___sg.wgpu.render_cmd_enc);
  yk___sg.wgpu.render_cmd_enc = 0;
  wgpuCommandEncoderRelease(yk___sg.wgpu.staging_cmd_enc);
  yk___sg.wgpu.staging_cmd_enc = 0;
  if (yk___sg.wgpu.queue) {
    wgpuQueueRelease(yk___sg.wgpu.queue);
    yk___sg.wgpu.queue = 0;
  }
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_reset_state_cache(void) {
  YK__SOKOL_LOG("yk___sg_wgpu_reset_state_cache: FIXME\n");
}
YK___SOKOL_PRIVATE yk__sg_resource_state
yk___sg_wgpu_create_context(yk___sg_context_t *ctx) {
  YK__SOKOL_ASSERT(ctx);
  YK___SOKOL_UNUSED(ctx);
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_destroy_context(yk___sg_context_t *ctx) {
  YK__SOKOL_ASSERT(ctx);
  YK___SOKOL_UNUSED(ctx);
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_activate_context(yk___sg_context_t *ctx) {
  (void) ctx;
  YK__SOKOL_LOG("yk___sg_wgpu_activate_context: FIXME\n");
}
YK___SOKOL_PRIVATE yk__sg_resource_state yk___sg_wgpu_create_buffer(
    yk___sg_buffer_t *buf, const yk__sg_buffer_desc *desc) {
  YK__SOKOL_ASSERT(buf && desc);
  const bool injected = (0 != desc->wgpu_buffer);
  yk___sg_buffer_common_init(&buf->cmn, desc);
  if (injected) {
    buf->wgpu.buf = (WGPUBuffer) desc->wgpu_buffer;
    wgpuBufferReference(buf->wgpu.buf);
  } else {
    WGPUBufferDescriptor wgpu_buf_desc;
    memset(&wgpu_buf_desc, 0, sizeof(wgpu_buf_desc));
    wgpu_buf_desc.usage =
        yk___sg_wgpu_buffer_usage(buf->cmn.type, buf->cmn.usage);
    wgpu_buf_desc.size = buf->cmn.size;
    if (YK__SG_USAGE_IMMUTABLE == buf->cmn.usage) {
      YK__SOKOL_ASSERT(desc->data.ptr);
      WGPUCreateBufferMappedResult res =
          wgpuDeviceCreateBufferMapped(yk___sg.wgpu.dev, &wgpu_buf_desc);
      buf->wgpu.buf = res.buffer;
      YK__SOKOL_ASSERT(res.data && (res.dataLength == buf->cmn.size));
      memcpy(res.data, desc->data.ptr, buf->cmn.size);
      wgpuBufferUnmap(res.buffer);
    } else {
      buf->wgpu.buf = wgpuDeviceCreateBuffer(yk___sg.wgpu.dev, &wgpu_buf_desc);
    }
  }
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_destroy_buffer(yk___sg_buffer_t *buf) {
  YK__SOKOL_ASSERT(buf);
  WGPUBuffer wgpu_buf = buf->wgpu.buf;
  if (0 != wgpu_buf) { wgpuBufferRelease(wgpu_buf); }
}
YK___SOKOL_PRIVATE void
yk___sg_wgpu_init_texdesc_common(WGPUTextureDescriptor *wgpu_tex_desc,
                                 const yk__sg_image_desc *desc) {
  wgpu_tex_desc->usage = WGPUTextureUsage_Sampled | WGPUTextureUsage_CopyDst;
  wgpu_tex_desc->dimension = yk___sg_wgpu_tex_dim(desc->type);
  wgpu_tex_desc->size.width = desc->width;
  wgpu_tex_desc->size.height = desc->height;
  if (desc->type == YK__SG_IMAGETYPE_3D) {
    wgpu_tex_desc->size.depth = desc->num_slices;
    wgpu_tex_desc->arrayLayerCount = 1;
  } else if (desc->type == YK__SG_IMAGETYPE_CUBE) {
    wgpu_tex_desc->size.depth = 1;
    wgpu_tex_desc->arrayLayerCount = 6;
  } else {
    wgpu_tex_desc->size.depth = 1;
    wgpu_tex_desc->arrayLayerCount = desc->num_slices;
  }
  wgpu_tex_desc->format = yk___sg_wgpu_textureformat(desc->pixel_format);
  wgpu_tex_desc->mipLevelCount = desc->num_mipmaps;
  wgpu_tex_desc->sampleCount = 1;
}
YK___SOKOL_PRIVATE yk__sg_resource_state
yk___sg_wgpu_create_image(yk___sg_image_t *img, const yk__sg_image_desc *desc) {
  YK__SOKOL_ASSERT(img && desc);
  YK__SOKOL_ASSERT(yk___sg.wgpu.dev);
  YK__SOKOL_ASSERT(yk___sg.wgpu.staging_cmd_enc);
  yk___sg_image_common_init(&img->cmn, desc);
  const bool injected = (0 != desc->wgpu_texture);
  const bool is_msaa = desc->sample_count > 1;
  WGPUTextureDescriptor wgpu_tex_desc;
  memset(&wgpu_tex_desc, 0, sizeof(wgpu_tex_desc));
  yk___sg_wgpu_init_texdesc_common(&wgpu_tex_desc, desc);
  if (yk___sg_is_valid_rendertarget_depth_format(img->cmn.pixel_format)) {
    YK__SOKOL_ASSERT(img->cmn.render_target);
    YK__SOKOL_ASSERT(img->cmn.type == YK__SG_IMAGETYPE_2D);
    YK__SOKOL_ASSERT(img->cmn.num_mipmaps == 1);
    YK__SOKOL_ASSERT(!injected);
    /* NOTE: a depth-stencil texture will never be MSAA-resolved, so there
           won't be a separate MSAA- and resolve-texture
        */
    wgpu_tex_desc.usage = WGPUTextureUsage_OutputAttachment;
    wgpu_tex_desc.sampleCount = desc->sample_count;
    img->wgpu.tex = wgpuDeviceCreateTexture(yk___sg.wgpu.dev, &wgpu_tex_desc);
    YK__SOKOL_ASSERT(img->wgpu.tex);
  } else {
    if (injected) {
      img->wgpu.tex = (WGPUTexture) desc->wgpu_texture;
      wgpuTextureReference(img->wgpu.tex);
    } else {
      /* NOTE: in the MSAA-rendertarget case, both the MSAA texture *and*
               the resolve texture need OutputAttachment usage
            */
      if (img->cmn.render_target) {
        wgpu_tex_desc.usage =
            WGPUTextureUsage_Sampled | WGPUTextureUsage_OutputAttachment;
      }
      img->wgpu.tex = wgpuDeviceCreateTexture(yk___sg.wgpu.dev, &wgpu_tex_desc);
      YK__SOKOL_ASSERT(img->wgpu.tex);
      /* copy content into texture via a throw-away staging buffer */
      if (desc->usage == YK__SG_USAGE_IMMUTABLE && !desc->render_target) {
        WGPUBufferDescriptor wgpu_buf_desc;
        memset(&wgpu_buf_desc, 0, sizeof(wgpu_buf_desc));
        wgpu_buf_desc.size = yk___sg_wgpu_image_data_buffer_size(img);
        wgpu_buf_desc.usage = WGPUBufferUsage_CopySrc | WGPUBufferUsage_CopyDst;
        WGPUCreateBufferMappedResult map =
            wgpuDeviceCreateBufferMapped(yk___sg.wgpu.dev, &wgpu_buf_desc);
        YK__SOKOL_ASSERT(map.buffer && map.data);
        uint32_t num_bytes = yk___sg_wgpu_copy_image_data(
            map.buffer, (uint8_t *) map.data, 0, img, &desc->data);
        YK___SOKOL_UNUSED(num_bytes);
        YK__SOKOL_ASSERT(num_bytes == wgpu_buf_desc.size);
        wgpuBufferUnmap(map.buffer);
        wgpuBufferRelease(map.buffer);
      }
    }
    /* create texture view object */
    WGPUTextureViewDescriptor wgpu_view_desc;
    memset(&wgpu_view_desc, 0, sizeof(wgpu_view_desc));
    wgpu_view_desc.dimension = yk___sg_wgpu_tex_viewdim(desc->type);
    img->wgpu.tex_view = wgpuTextureCreateView(img->wgpu.tex, &wgpu_view_desc);
    /* if render target and MSAA, then a separate texture in MSAA format is needed
           which will be resolved into the regular texture at the end of the
           offscreen-render pass
        */
    if (desc->render_target && is_msaa) {
      wgpu_tex_desc.dimension = WGPUTextureDimension_2D;
      wgpu_tex_desc.size.depth = 1;
      wgpu_tex_desc.arrayLayerCount = 1;
      wgpu_tex_desc.mipLevelCount = 1;
      wgpu_tex_desc.usage = WGPUTextureUsage_OutputAttachment;
      wgpu_tex_desc.sampleCount = desc->sample_count;
      img->wgpu.msaa_tex =
          wgpuDeviceCreateTexture(yk___sg.wgpu.dev, &wgpu_tex_desc);
      YK__SOKOL_ASSERT(img->wgpu.msaa_tex);
    }
    /* create sampler via shared-sampler-cache */
    img->wgpu.sampler = yk___sg_wgpu_create_sampler(desc);
    YK__SOKOL_ASSERT(img->wgpu.sampler);
  }
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_destroy_image(yk___sg_image_t *img) {
  YK__SOKOL_ASSERT(img);
  if (img->wgpu.tex) {
    wgpuTextureRelease(img->wgpu.tex);
    img->wgpu.tex = 0;
  }
  if (img->wgpu.tex_view) {
    wgpuTextureViewRelease(img->wgpu.tex_view);
    img->wgpu.tex_view = 0;
  }
  if (img->wgpu.msaa_tex) {
    wgpuTextureRelease(img->wgpu.msaa_tex);
    img->wgpu.msaa_tex = 0;
  }
  /* NOTE: do *not* destroy the sampler from the shared-sampler-cache */
  img->wgpu.sampler = 0;
}
/*
    How BindGroups work in WebGPU:

    - up to 4 bind groups can be bound simultaneously
    - up to 16 bindings per bind group
    - 'binding' slots are local per bind group
    - in the shader:
        layout(set=0, binding=1) corresponds to bind group 0, binding 1

    Now how to map this to yk__sokol-gfx's bind model:

    Reduce YK__SG_MAX_SHADERSTAGE_IMAGES to 8, then:

        1 bind group for all 8 uniform buffers
        1 bind group for vertex shader textures + samplers
        1 bind group for fragment shader textures + samples

    Alternatively:

        1 bind group for 8 uniform buffer slots
        1 bind group for 8 vs images + 8 vs samplers
        1 bind group for 12 fs images
        1 bind group for 12 fs samplers

    I guess this means that we need to create BindGroups on the
    fly during yk__sg_apply_bindings() :/
*/
YK___SOKOL_PRIVATE yk__sg_resource_state yk___sg_wgpu_create_shader(
    yk___sg_shader_t *shd, const yk__sg_shader_desc *desc) {
  YK__SOKOL_ASSERT(shd && desc);
  YK__SOKOL_ASSERT(desc->vs.bytecode.ptr && desc->fs.bytecode.ptr);
  yk___sg_shader_common_init(&shd->cmn, desc);
  bool success = true;
  for (int stage_index = 0; stage_index < YK__SG_NUM_SHADER_STAGES;
       stage_index++) {
    const yk__sg_shader_stage_desc *stage_desc =
        (stage_index == YK__SG_SHADERSTAGE_VS) ? &desc->vs : &desc->fs;
    YK__SOKOL_ASSERT((stage_desc->bytecode.size & 3) == 0);
    yk___sg_shader_stage_t *cmn_stage = &shd->cmn.stage[stage_index];
    yk___sg_wgpu_shader_stage_t *wgpu_stage = &shd->wgpu.stage[stage_index];
    yk___sg_strcpy(&wgpu_stage->entry, stage_desc->entry);
    WGPUShaderModuleDescriptor wgpu_shdmod_desc;
    memset(&wgpu_shdmod_desc, 0, sizeof(wgpu_shdmod_desc));
    wgpu_shdmod_desc.codeSize = stage_desc->bytecode.size >> 2;
    wgpu_shdmod_desc.code = (const uint32_t *) stage_desc->bytecode.ptr;
    wgpu_stage->module =
        wgpuDeviceCreateShaderModule(yk___sg.wgpu.dev, &wgpu_shdmod_desc);
    if (0 == wgpu_stage->module) { success = false; }
    /* create image/sampler bind group for the shader stage */
    WGPUShaderStage vis = (stage_index == YK__SG_SHADERSTAGE_VS)
                              ? WGPUShaderStage_Vertex
                              : WGPUShaderStage_Fragment;
    int num_imgs = cmn_stage->num_images;
    if (num_imgs > YK___SG_WGPU_MAX_SHADERSTAGE_IMAGES) {
      num_imgs = YK___SG_WGPU_MAX_SHADERSTAGE_IMAGES;
    }
    WGPUBindGroupLayoutBinding
        bglb_desc[YK___SG_WGPU_MAX_SHADERSTAGE_IMAGES * 2];
    memset(bglb_desc, 0, sizeof(bglb_desc));
    for (int img_index = 0; img_index < num_imgs; img_index++) {
      /* texture- and sampler-bindings */
      WGPUBindGroupLayoutBinding *tex_desc = &bglb_desc[img_index * 2 + 0];
      WGPUBindGroupLayoutBinding *smp_desc = &bglb_desc[img_index * 2 + 1];
      tex_desc->binding = img_index;
      tex_desc->visibility = vis;
      tex_desc->type = WGPUBindingType_SampledTexture;
      tex_desc->textureDimension =
          yk___sg_wgpu_tex_viewdim(cmn_stage->images[img_index].image_type);
      tex_desc->textureComponentType =
          yk___sg_wgpu_tex_comptype(cmn_stage->images[img_index].sampler_type);
      smp_desc->binding = img_index + YK___SG_WGPU_MAX_SHADERSTAGE_IMAGES;
      smp_desc->visibility = vis;
      smp_desc->type = WGPUBindingType_Sampler;
    }
    WGPUBindGroupLayoutDescriptor img_bgl_desc;
    memset(&img_bgl_desc, 0, sizeof(img_bgl_desc));
    img_bgl_desc.bindingCount = num_imgs * 2;
    img_bgl_desc.bindings = &bglb_desc[0];
    wgpu_stage->bind_group_layout =
        wgpuDeviceCreateBindGroupLayout(yk___sg.wgpu.dev, &img_bgl_desc);
    YK__SOKOL_ASSERT(wgpu_stage->bind_group_layout);
  }
  return success ? YK__SG_RESOURCESTATE_VALID : YK__SG_RESOURCESTATE_FAILED;
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_destroy_shader(yk___sg_shader_t *shd) {
  YK__SOKOL_ASSERT(shd);
  for (int stage_index = 0; stage_index < YK__SG_NUM_SHADER_STAGES;
       stage_index++) {
    yk___sg_wgpu_shader_stage_t *wgpu_stage = &shd->wgpu.stage[stage_index];
    if (wgpu_stage->module) {
      wgpuShaderModuleRelease(wgpu_stage->module);
      wgpu_stage->module = 0;
    }
    if (wgpu_stage->bind_group_layout) {
      wgpuBindGroupLayoutRelease(wgpu_stage->bind_group_layout);
      wgpu_stage->bind_group_layout = 0;
    }
  }
}
YK___SOKOL_PRIVATE yk__sg_resource_state
yk___sg_wgpu_create_pipeline(yk___sg_pipeline_t *pip, yk___sg_shader_t *shd,
                             const yk__sg_pipeline_desc *desc) {
  YK__SOKOL_ASSERT(pip && shd && desc);
  YK__SOKOL_ASSERT(desc->shader.id == shd->slot.id);
  YK__SOKOL_ASSERT(shd->wgpu.stage[YK__SG_SHADERSTAGE_VS].bind_group_layout);
  YK__SOKOL_ASSERT(shd->wgpu.stage[YK__SG_SHADERSTAGE_FS].bind_group_layout);
  pip->shader = shd;
  yk___sg_pipeline_common_init(&pip->cmn, desc);
  pip->wgpu.stencil_ref = (uint32_t) desc->stencil.ref;
  WGPUBindGroupLayout pip_bgl[3] = {
      yk___sg.wgpu.ub.bindgroup_layout,
      shd->wgpu.stage[YK__SG_SHADERSTAGE_VS].bind_group_layout,
      shd->wgpu.stage[YK__SG_SHADERSTAGE_FS].bind_group_layout};
  WGPUPipelineLayoutDescriptor pl_desc;
  memset(&pl_desc, 0, sizeof(pl_desc));
  pl_desc.bindGroupLayoutCount = 3;
  pl_desc.bindGroupLayouts = &pip_bgl[0];
  WGPUPipelineLayout pip_layout =
      wgpuDeviceCreatePipelineLayout(yk___sg.wgpu.dev, &pl_desc);
  WGPUVertexBufferLayoutDescriptor vb_desc[YK__SG_MAX_SHADERSTAGE_BUFFERS];
  memset(&vb_desc, 0, sizeof(vb_desc));
  WGPUVertexAttributeDescriptor va_desc[YK__SG_MAX_SHADERSTAGE_BUFFERS]
                                       [YK__SG_MAX_VERTEX_ATTRIBUTES];
  memset(&va_desc, 0, sizeof(va_desc));
  int vb_idx = 0;
  for (; vb_idx < YK__SG_MAX_SHADERSTAGE_BUFFERS; vb_idx++) {
    const yk__sg_buffer_layout_desc *src_vb_desc =
        &desc->layout.buffers[vb_idx];
    if (0 == src_vb_desc->stride) { break; }
    vb_desc[vb_idx].arrayStride = src_vb_desc->stride;
    vb_desc[vb_idx].stepMode = yk___sg_wgpu_stepmode(src_vb_desc->step_func);
    /* NOTE: WebGPU has no support for vertex step rate (because that's
           not supported by Core Vulkan
        */
    int va_idx = 0;
    for (int va_loc = 0; va_loc < YK__SG_MAX_VERTEX_ATTRIBUTES; va_loc++) {
      const yk__sg_vertex_attr_desc *src_va_desc = &desc->layout.attrs[va_loc];
      if (YK__SG_VERTEXFORMAT_INVALID == src_va_desc->format) { break; }
      pip->cmn.vertex_layout_valid[src_va_desc->buffer_index] = true;
      if (vb_idx == src_va_desc->buffer_index) {
        va_desc[vb_idx][va_idx].format =
            yk___sg_wgpu_vertexformat(src_va_desc->format);
        va_desc[vb_idx][va_idx].offset = src_va_desc->offset;
        va_desc[vb_idx][va_idx].shaderLocation = va_loc;
        va_idx++;
      }
    }
    vb_desc[vb_idx].attributeCount = va_idx;
    vb_desc[vb_idx].attributes = &va_desc[vb_idx][0];
  }
  WGPUVertexStateDescriptor vx_state_desc;
  memset(&vx_state_desc, 0, sizeof(vx_state_desc));
  vx_state_desc.indexFormat = yk___sg_wgpu_indexformat(desc->index_type);
  vx_state_desc.vertexBufferCount = vb_idx;
  vx_state_desc.vertexBuffers = vb_desc;
  WGPURasterizationStateDescriptor rs_desc;
  memset(&rs_desc, 0, sizeof(rs_desc));
  rs_desc.frontFace = yk___sg_wgpu_frontface(desc->face_winding);
  rs_desc.cullMode = yk___sg_wgpu_cullmode(desc->cull_mode);
  rs_desc.depthBias = (int32_t) desc->depth.bias;
  rs_desc.depthBiasClamp = desc->depth.bias_clamp;
  rs_desc.depthBiasSlopeScale = desc->depth.bias_slope_scale;
  WGPUDepthStencilStateDescriptor ds_desc;
  memset(&ds_desc, 0, sizeof(ds_desc));
  ds_desc.format = yk___sg_wgpu_textureformat(desc->depth.pixel_format);
  ds_desc.depthWriteEnabled = desc->depth.write_enabled;
  ds_desc.depthCompare = yk___sg_wgpu_comparefunc(desc->depth.compare);
  ds_desc.stencilReadMask = desc->stencil.read_mask;
  ds_desc.stencilWriteMask = desc->stencil.write_mask;
  ds_desc.stencilFront.compare =
      yk___sg_wgpu_comparefunc(desc->stencil.front.compare);
  ds_desc.stencilFront.failOp =
      yk___sg_wgpu_stencilop(desc->stencil.front.fail_op);
  ds_desc.stencilFront.depthFailOp =
      yk___sg_wgpu_stencilop(desc->stencil.front.depth_fail_op);
  ds_desc.stencilFront.passOp =
      yk___sg_wgpu_stencilop(desc->stencil.front.pass_op);
  ds_desc.stencilBack.compare =
      yk___sg_wgpu_comparefunc(desc->stencil.back.compare);
  ds_desc.stencilBack.failOp =
      yk___sg_wgpu_stencilop(desc->stencil.back.fail_op);
  ds_desc.stencilBack.depthFailOp =
      yk___sg_wgpu_stencilop(desc->stencil.back.depth_fail_op);
  ds_desc.stencilBack.passOp =
      yk___sg_wgpu_stencilop(desc->stencil.back.pass_op);
  WGPUProgrammableStageDescriptor fs_desc;
  memset(&fs_desc, 0, sizeof(fs_desc));
  fs_desc.module = shd->wgpu.stage[YK__SG_SHADERSTAGE_FS].module;
  fs_desc.entryPoint = shd->wgpu.stage[YK__SG_SHADERSTAGE_VS].entry.buf;
  WGPUColorStateDescriptor cs_desc[YK__SG_MAX_COLOR_ATTACHMENTS];
  memset(cs_desc, 0, sizeof(cs_desc));
  for (uint32_t i = 0; i < desc->color_count; i++) {
    YK__SOKOL_ASSERT(i < YK__SG_MAX_COLOR_ATTACHMENTS);
    cs_desc[i].format =
        yk___sg_wgpu_textureformat(desc->colors[i].pixel_format);
    cs_desc[i].colorBlend.operation =
        yk___sg_wgpu_blendop(desc->colors[i].blend.op_rgb);
    cs_desc[i].colorBlend.srcFactor =
        yk___sg_wgpu_blendfactor(desc->colors[i].blend.src_factor_rgb);
    cs_desc[i].colorBlend.dstFactor =
        yk___sg_wgpu_blendfactor(desc->colors[i].blend.dst_factor_rgb);
    cs_desc[i].alphaBlend.operation =
        yk___sg_wgpu_blendop(desc->colors[i].blend.op_alpha);
    cs_desc[i].alphaBlend.srcFactor =
        yk___sg_wgpu_blendfactor(desc->colors[i].blend.src_factor_alpha);
    cs_desc[i].alphaBlend.dstFactor =
        yk___sg_wgpu_blendfactor(desc->colors[i].blend.dst_factor_alpha);
    cs_desc[i].writeMask =
        yk___sg_wgpu_colorwritemask(desc->colors[i].write_mask);
  }
  WGPURenderPipelineDescriptor pip_desc;
  memset(&pip_desc, 0, sizeof(pip_desc));
  pip_desc.layout = pip_layout;
  pip_desc.vertexStage.module = shd->wgpu.stage[YK__SG_SHADERSTAGE_VS].module;
  pip_desc.vertexStage.entryPoint =
      shd->wgpu.stage[YK__SG_SHADERSTAGE_VS].entry.buf;
  pip_desc.fragmentStage = &fs_desc;
  pip_desc.vertexState = &vx_state_desc;
  pip_desc.primitiveTopology = yk___sg_wgpu_topology(desc->primitive_type);
  pip_desc.rasterizationState = &rs_desc;
  pip_desc.sampleCount = desc->sample_count;
  if (YK__SG_PIXELFORMAT_NONE != desc->depth.pixel_format) {
    pip_desc.depthStencilState = &ds_desc;
  }
  pip_desc.colorStateCount = desc->color_count;
  pip_desc.colorStates = cs_desc;
  pip_desc.sampleMask = 0xFFFFFFFF; /* FIXME: ??? */
  pip->wgpu.pip = wgpuDeviceCreateRenderPipeline(yk___sg.wgpu.dev, &pip_desc);
  YK__SOKOL_ASSERT(0 != pip->wgpu.pip);
  wgpuPipelineLayoutRelease(pip_layout);
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_destroy_pipeline(yk___sg_pipeline_t *pip) {
  YK__SOKOL_ASSERT(pip);
  if (pip == yk___sg.wgpu.cur_pipeline) {
    yk___sg.wgpu.cur_pipeline = 0;
    YK___Sg.wgpu.cur_pipeline_id.id = YK__SG_INVALID_ID;
  }
  if (pip->wgpu.pip) {
    wgpuRenderPipelineRelease(pip->wgpu.pip);
    pip->wgpu.pip = 0;
  }
}
YK___SOKOL_PRIVATE yk__sg_resource_state
yk___sg_wgpu_create_pass(yk___sg_pass_t *pass, yk___sg_image_t **att_images,
                         const yk__sg_pass_desc *desc) {
  YK__SOKOL_ASSERT(pass && desc);
  YK__SOKOL_ASSERT(att_images && att_images[0]);
  yk___sg_pass_common_init(&pass->cmn, desc);
  /* copy image pointers and create render-texture views */
  const yk__sg_pass_attachment_desc *att_desc;
  for (uint32_t i = 0; i < pass->cmn.num_color_atts; i++) {
    att_desc = &desc->color_attachments[i];
    if (att_desc->image.id != YK__SG_INVALID_ID) {
      YK__SOKOL_ASSERT(att_desc->image.id != YK__SG_INVALID_ID);
      YK__SOKOL_ASSERT(0 == pass->wgpu.color_atts[i].image);
      yk___sg_image_t *img = att_images[i];
      YK__SOKOL_ASSERT(img && (img->slot.id == att_desc->image.id));
      YK__SOKOL_ASSERT(
          yk___sg_is_valid_rendertarget_color_format(img->cmn.pixel_format));
      pass->wgpu.color_atts[i].image = img;
      /* create a render-texture-view to render into the right sub-surface */
      const bool is_msaa = img->cmn.sample_count > 1;
      WGPUTextureViewDescriptor view_desc;
      memset(&view_desc, 0, sizeof(view_desc));
      view_desc.baseMipLevel = is_msaa ? 0 : att_desc->mip_level;
      view_desc.mipLevelCount = 1;
      view_desc.baseArrayLayer = is_msaa ? 0 : att_desc->slice;
      view_desc.arrayLayerCount = 1;
      WGPUTexture wgpu_tex = is_msaa ? img->wgpu.msaa_tex : img->wgpu.tex;
      YK__SOKOL_ASSERT(wgpu_tex);
      pass->wgpu.color_atts[i].render_tex_view =
          wgpuTextureCreateView(wgpu_tex, &view_desc);
      YK__SOKOL_ASSERT(pass->wgpu.color_atts[i].render_tex_view);
      /* ... and if needed a separate resolve texture view */
      if (is_msaa) {
        view_desc.baseMipLevel = att_desc->mip_level;
        view_desc.baseArrayLayer = att_desc->slice;
        WGPUTexture wgpu_tex = img->wgpu.tex;
        pass->wgpu.color_atts[i].resolve_tex_view =
            wgpuTextureCreateView(wgpu_tex, &view_desc);
        YK__SOKOL_ASSERT(pass->wgpu.color_atts[i].resolve_tex_view);
      }
    }
  }
  YK__SOKOL_ASSERT(0 == pass->wgpu.ds_att.image);
  att_desc = &desc->depth_stencil_attachment;
  if (att_desc->image.id != YK__SG_INVALID_ID) {
    const int ds_img_index = YK__SG_MAX_COLOR_ATTACHMENTS;
    YK__SOKOL_ASSERT(att_images[ds_img_index] &&
                     (att_images[ds_img_index]->slot.id == att_desc->image.id));
    YK__SOKOL_ASSERT(yk___sg_is_valid_rendertarget_depth_format(
        att_images[ds_img_index]->cmn.pixel_format));
    yk___sg_image_t *ds_img = att_images[ds_img_index];
    pass->wgpu.ds_att.image = ds_img;
    /* create a render-texture view */
    YK__SOKOL_ASSERT(0 == att_desc->mip_level);
    YK__SOKOL_ASSERT(0 == att_desc->slice);
    WGPUTextureViewDescriptor view_desc;
    memset(&view_desc, 0, sizeof(view_desc));
    WGPUTexture wgpu_tex = ds_img->wgpu.tex;
    YK__SOKOL_ASSERT(wgpu_tex);
    pass->wgpu.ds_att.render_tex_view =
        wgpuTextureCreateView(wgpu_tex, &view_desc);
    YK__SOKOL_ASSERT(pass->wgpu.ds_att.render_tex_view);
  }
  return YK__SG_RESOURCESTATE_VALID;
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_destroy_pass(yk___sg_pass_t *pass) {
  YK__SOKOL_ASSERT(pass);
  for (uint32_t i = 0; i < pass->cmn.num_color_atts; i++) {
    if (pass->wgpu.color_atts[i].render_tex_view) {
      wgpuTextureViewRelease(pass->wgpu.color_atts[i].render_tex_view);
      pass->wgpu.color_atts[i].render_tex_view = 0;
    }
    if (pass->wgpu.color_atts[i].resolve_tex_view) {
      wgpuTextureViewRelease(pass->wgpu.color_atts[i].resolve_tex_view);
      pass->wgpu.color_atts[i].resolve_tex_view = 0;
    }
  }
  if (pass->wgpu.ds_att.render_tex_view) {
    wgpuTextureViewRelease(pass->wgpu.ds_att.render_tex_view);
    pass->wgpu.ds_att.render_tex_view = 0;
  }
}
YK___SOKOL_PRIVATE yk___sg_image_t *
yk___sg_wgpu_pass_color_image(const yk___sg_pass_t *pass, int index) {
  YK__SOKOL_ASSERT(pass && (index >= 0) &&
                   (index < YK__SG_MAX_COLOR_ATTACHMENTS));
  /* NOTE: may return null */
  return pass->wgpu.color_atts[index].image;
}
YK___SOKOL_PRIVATE yk___sg_image_t *
yk___sg_wgpu_pass_ds_image(const yk___sg_pass_t *pass) {
  /* NOTE: may return null */
  YK__SOKOL_ASSERT(pass);
  return pass->wgpu.ds_att.image;
}
YK___SOKOL_PRIVATE void
yk___sg_wgpu_begin_pass(yk___sg_pass_t *pass, const yk__sg_pass_action *action,
                        int w, int h) {
  YK__SOKOL_ASSERT(action);
  YK__SOKOL_ASSERT(!yk___sg.wgpu.in_pass);
  YK__SOKOL_ASSERT(yk___sg.wgpu.render_cmd_enc);
  YK__SOKOL_ASSERT(yk___sg.wgpu.dev);
  YK__SOKOL_ASSERT(yk___sg.wgpu.render_view_cb ||
                   yk___sg.wgpu.render_view_userdata_cb);
  YK__SOKOL_ASSERT(yk___sg.wgpu.resolve_view_cb ||
                   yk___sg.wgpu.resolve_view_userdata_cb);
  YK__SOKOL_ASSERT(yk___sg.wgpu.depth_stencil_view_cb ||
                   yk___sg.wgpu.depth_stencil_view_userdata_cb);
  yk___sg.wgpu.in_pass = true;
  yk___sg.wgpu.cur_width = w;
  yk___sg.wgpu.cur_height = h;
  yk___sg.wgpu.cur_pipeline = 0;
  yk___sg.wgpu.cur_pipeline_id.id = YK__SG_INVALID_ID;
  YK__SOKOL_ASSERT(yk___sg.wgpu.render_cmd_enc);
  if (pass) {
    WGPURenderPassDescriptor wgpu_pass_desc;
    memset(&wgpu_pass_desc, 0, sizeof(wgpu_pass_desc));
    WGPURenderPassColorAttachmentDescriptor
        wgpu_color_att_desc[YK__SG_MAX_COLOR_ATTACHMENTS];
    memset(&wgpu_color_att_desc, 0, sizeof(wgpu_color_att_desc));
    YK__SOKOL_ASSERT(pass->slot.state == YK__SG_RESOURCESTATE_VALID);
    for (uint32_t i = 0; i < pass->cmn.num_color_atts; i++) {
      const yk___sg_wgpu_attachment_t *wgpu_att = &pass->wgpu.color_atts[i];
      wgpu_color_att_desc[i].loadOp =
          yk___sg_wgpu_load_op(action->colors[i].action);
      wgpu_color_att_desc[i].storeOp = WGPUStoreOp_Store;
      wgpu_color_att_desc[i].clearColor.r = action->colors[i].value.r;
      wgpu_color_att_desc[i].clearColor.g = action->colors[i].value.g;
      wgpu_color_att_desc[i].clearColor.b = action->colors[i].value.b;
      wgpu_color_att_desc[i].clearColor.a = action->colors[i].value.a;
      wgpu_color_att_desc[i].attachment = wgpu_att->render_tex_view;
      if (wgpu_att->image->cmn.sample_count > 1) {
        wgpu_color_att_desc[i].resolveTarget = wgpu_att->resolve_tex_view;
      }
    }
    wgpu_pass_desc.colorAttachmentCount = pass->cmn.num_color_atts;
    wgpu_pass_desc.colorAttachments = &wgpu_color_att_desc[0];
    if (pass->wgpu.ds_att.image) {
      WGPURenderPassDepthStencilAttachmentDescriptor wgpu_ds_att_desc;
      memset(&wgpu_ds_att_desc, 0, sizeof(wgpu_ds_att_desc));
      wgpu_ds_att_desc.depthLoadOp = yk___sg_wgpu_load_op(action->depth.action);
      wgpu_ds_att_desc.clearDepth = action->depth.value;
      wgpu_ds_att_desc.stencilLoadOp =
          yk___sg_wgpu_load_op(action->stencil.action);
      wgpu_ds_att_desc.clearStencil = action->stencil.value;
      wgpu_ds_att_desc.attachment = pass->wgpu.ds_att.render_tex_view;
      wgpu_pass_desc.depthStencilAttachment = &wgpu_ds_att_desc;
      yk___sg.wgpu.pass_enc = wgpuCommandEncoderBeginRenderPass(
          yk___sg.wgpu.render_cmd_enc, &wgpu_pass_desc);
    }
  } else {
    /* default render pass */
    WGPUTextureView wgpu_render_view =
        yk___sg.wgpu.render_view_cb
            ? yk___sg.wgpu.render_view_cb()
            : yk___sg.wgpu.render_view_userdata_cb(yk___sg.wgpu.user_data);
    WGPUTextureView wgpu_resolve_view =
        yk___sg.wgpu.resolve_view_cb
            ? yk___sg.wgpu.resolve_view_cb()
            : yk___sg.wgpu.resolve_view_userdata_cb(yk___sg.wgpu.user_data);
    WGPUTextureView wgpu_depth_stencil_view =
        yk___sg.wgpu.depth_stencil_view_cb
            ? yk___sg.wgpu.depth_stencil_view_cb()
            : yk___sg.wgpu.depth_stencil_view_userdata_cb(
                  yk___sg.wgpu.user_data);
    WGPURenderPassDescriptor pass_desc;
    memset(&pass_desc, 0, sizeof(pass_desc));
    WGPURenderPassColorAttachmentDescriptor color_att_desc;
    memset(&color_att_desc, 0, sizeof(color_att_desc));
    color_att_desc.loadOp = yk___sg_wgpu_load_op(action->colors[0].action);
    color_att_desc.clearColor.r = action->colors[0].value.r;
    color_att_desc.clearColor.g = action->colors[0].value.g;
    color_att_desc.clearColor.b = action->colors[0].value.b;
    color_att_desc.clearColor.a = action->colors[0].value.a;
    color_att_desc.attachment = wgpu_render_view;
    color_att_desc.resolveTarget =
        wgpu_resolve_view; /* null if no MSAA rendering */
    pass_desc.colorAttachmentCount = 1;
    pass_desc.colorAttachments = &color_att_desc;
    WGPURenderPassDepthStencilAttachmentDescriptor ds_att_desc;
    memset(&ds_att_desc, 0, sizeof(ds_att_desc));
    ds_att_desc.attachment = wgpu_depth_stencil_view;
    YK__SOKOL_ASSERT(0 != ds_att_desc.attachment);
    ds_att_desc.depthLoadOp = yk___sg_wgpu_load_op(action->depth.action);
    ds_att_desc.clearDepth = action->depth.value;
    ds_att_desc.stencilLoadOp = yk___sg_wgpu_load_op(action->stencil.action);
    ds_att_desc.clearStencil = action->stencil.value;
    pass_desc.depthStencilAttachment = &ds_att_desc;
    yk___sg.wgpu.pass_enc = wgpuCommandEncoderBeginRenderPass(
        yk___sg.wgpu.render_cmd_enc, &pass_desc);
  }
  YK__SOKOL_ASSERT(yk___sg.wgpu.pass_enc);
  /* initial uniform buffer binding (required even if no uniforms are set in the frame) */
  wgpuRenderPassEncoderSetBindGroup(
      yk___sg.wgpu.pass_enc,
      0, /* groupIndex 0 is reserved for uniform buffers */
      yk___sg.wgpu.ub.bindgroup,
      YK__SG_NUM_SHADER_STAGES * YK__SG_MAX_SHADERSTAGE_UBS,
      &yk___sg.wgpu.ub.bind_offsets[0][0]);
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_end_pass(void) {
  YK__SOKOL_ASSERT(yk___sg.wgpu.in_pass);
  YK__SOKOL_ASSERT(yk___sg.wgpu.pass_enc);
  yk___sg.wgpu.in_pass = false;
  wgpuRenderPassEncoderEndPass(yk___sg.wgpu.pass_enc);
  wgpuRenderPassEncoderRelease(yk___sg.wgpu.pass_enc);
  yk___sg.wgpu.pass_enc = 0;
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_commit(void) {
  YK__SOKOL_ASSERT(!yk___sg.wgpu.in_pass);
  YK__SOKOL_ASSERT(yk___sg.wgpu.queue);
  YK__SOKOL_ASSERT(yk___sg.wgpu.render_cmd_enc);
  YK__SOKOL_ASSERT(yk___sg.wgpu.staging_cmd_enc);
  /* finish and submit this frame's work */
  yk___sg_wgpu_ubpool_flush();
  yk___sg_wgpu_staging_unmap();
  WGPUCommandBuffer cmd_bufs[2];
  WGPUCommandBufferDescriptor cmd_buf_desc;
  memset(&cmd_buf_desc, 0, sizeof(cmd_buf_desc));
  cmd_bufs[0] =
      wgpuCommandEncoderFinish(yk___sg.wgpu.staging_cmd_enc, &cmd_buf_desc);
  YK__SOKOL_ASSERT(cmd_bufs[0]);
  wgpuCommandEncoderRelease(yk___sg.wgpu.staging_cmd_enc);
  yk___sg.wgpu.staging_cmd_enc = 0;
  cmd_bufs[1] =
      wgpuCommandEncoderFinish(yk___sg.wgpu.render_cmd_enc, &cmd_buf_desc);
  YK__SOKOL_ASSERT(cmd_bufs[1]);
  wgpuCommandEncoderRelease(yk___sg.wgpu.render_cmd_enc);
  yk___sg.wgpu.render_cmd_enc = 0;
  wgpuQueueSubmit(yk___sg.wgpu.queue, 2, &cmd_bufs[0]);
  wgpuCommandBufferRelease(cmd_bufs[0]);
  wgpuCommandBufferRelease(cmd_bufs[1]);
  /* create a new render- and staging-command-encoders for next frame */
  WGPUCommandEncoderDescriptor cmd_enc_desc;
  memset(&cmd_enc_desc, 0, sizeof(cmd_enc_desc));
  yk___sg.wgpu.staging_cmd_enc =
      wgpuDeviceCreateCommandEncoder(yk___sg.wgpu.dev, &cmd_enc_desc);
  yk___sg.wgpu.render_cmd_enc =
      wgpuDeviceCreateCommandEncoder(yk___sg.wgpu.dev, &cmd_enc_desc);
  /* grab new staging buffers for uniform- and vertex/image-updates */
  yk___sg_wgpu_ubpool_next_frame(false);
  yk___sg_wgpu_staging_next_frame(false);
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_apply_viewport(int x, int y, int w, int h,
                                                    bool origin_top_left) {
  YK__SOKOL_ASSERT(yk___sg.wgpu.in_pass);
  YK__SOKOL_ASSERT(yk___sg.wgpu.pass_enc);
  float xf = (float) x;
  float yf =
      (float) (origin_top_left ? y : (yk___sg.wgpu.cur_height - (y + h)));
  float wf = (float) w;
  float hf = (float) h;
  wgpuRenderPassEncoderSetViewport(yk___sg.wgpu.pass_enc, xf, yf, wf, hf, 0.0f,
                                   1.0f);
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_apply_scissor_rect(int x, int y, int w,
                                                        int h,
                                                        bool origin_top_left) {
  YK__SOKOL_ASSERT(yk___sg.wgpu.in_pass);
  YK__SOKOL_ASSERT(yk___sg.wgpu.pass_enc);
  YK__SOKOL_ASSERT(yk___sg.wgpu.in_pass);
  YK__SOKOL_ASSERT(yk___sg.wgpu.pass_enc);
  /* clip against framebuffer rect */
  x = yk___sg_min(yk___sg_max(0, x), yk___sg.wgpu.cur_width - 1);
  y = yk___sg_min(yk___sg_max(0, y), yk___sg.wgpu.cur_height - 1);
  if ((x + w) > yk___sg.wgpu.cur_width) { w = yk___sg.wgpu.cur_width - x; }
  if ((y + h) > yk___sg.wgpu.cur_height) { h = yk___sg.wgpu.cur_height - y; }
  w = yk___sg_max(w, 1);
  h = yk___sg_max(h, 1);
  uint32_t sx = (uint32_t) x;
  uint32_t sy = origin_top_left ? y : (yk___sg.wgpu.cur_height - (y + h));
  uint32_t sw = w;
  uint32_t sh = h;
  wgpuRenderPassEncoderSetScissorRect(yk___sg.wgpu.pass_enc, sx, sy, sw, sh);
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_apply_pipeline(yk___sg_pipeline_t *pip) {
  YK__SOKOL_ASSERT(pip);
  YK__SOKOL_ASSERT(pip->wgpu.pip);
  YK__SOKOL_ASSERT(yk___sg.wgpu.in_pass);
  YK__SOKOL_ASSERT(yk___sg.wgpu.pass_enc);
  yk___sg.wgpu.draw_indexed = (pip->cmn.index_type != YK__SG_INDEXTYPE_NONE);
  yk___sg.wgpu.cur_pipeline = pip;
  yk___sg.wgpu.cur_pipeline_id.id = pip->slot.id;
  wgpuRenderPassEncoderSetPipeline(yk___sg.wgpu.pass_enc, pip->wgpu.pip);
  wgpuRenderPassEncoderSetBlendColor(yk___sg.wgpu.pass_enc,
                                     (WGPUColor *) &pip->cmn.blend_color);
  wgpuRenderPassEncoderSetStencilReference(yk___sg.wgpu.pass_enc,
                                           pip->wgpu.stencil_ref);
}
YK___SOKOL_PRIVATE WGPUBindGroup yk___sg_wgpu_create_images_bindgroup(
    WGPUBindGroupLayout bgl, yk___sg_image_t **imgs, int num_imgs) {
  YK__SOKOL_ASSERT(yk___sg.wgpu.dev);
  YK__SOKOL_ASSERT(num_imgs <= YK___SG_WGPU_MAX_SHADERSTAGE_IMAGES);
  WGPUBindGroupBinding img_bgb[YK___SG_WGPU_MAX_SHADERSTAGE_IMAGES * 2];
  memset(&img_bgb, 0, sizeof(img_bgb));
  for (int img_index = 0; img_index < num_imgs; img_index++) {
    WGPUBindGroupBinding *tex_bdg = &img_bgb[img_index * 2 + 0];
    WGPUBindGroupBinding *smp_bdg = &img_bgb[img_index * 2 + 1];
    tex_bdg->binding = img_index;
    tex_bdg->textureView = imgs[img_index]->wgpu.tex_view;
    smp_bdg->binding = img_index + YK___SG_WGPU_MAX_SHADERSTAGE_IMAGES;
    smp_bdg->sampler = imgs[img_index]->wgpu.sampler;
  }
  WGPUBindGroupDescriptor bg_desc;
  memset(&bg_desc, 0, sizeof(bg_desc));
  bg_desc.layout = bgl;
  bg_desc.bindingCount = 2 * num_imgs;
  bg_desc.bindings = &img_bgb[0];
  WGPUBindGroup bg = wgpuDeviceCreateBindGroup(yk___sg.wgpu.dev, &bg_desc);
  YK__SOKOL_ASSERT(bg);
  return bg;
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_apply_bindings(
    yk___sg_pipeline_t *pip, yk___sg_buffer_t **vbs, const int *vb_offsets,
    int num_vbs, yk___sg_buffer_t *ib, int ib_offset, yk___sg_image_t **vs_imgs,
    int num_vs_imgs, yk___sg_image_t **fs_imgs, int num_fs_imgs) {
  YK__SOKOL_ASSERT(yk___sg.wgpu.in_pass);
  YK__SOKOL_ASSERT(yk___sg.wgpu.pass_enc);
  YK__SOKOL_ASSERT(pip->shader &&
                   (pip->cmn.shader_id.id == pip->shader->slot.id));
  /* index buffer */
  if (ib) {
    wgpuRenderPassEncoderSetIndexBuffer(yk___sg.wgpu.pass_enc, ib->wgpu.buf,
                                        ib_offset);
  }
  /* vertex buffers */
  for (uint32_t slot = 0; slot < (uint32_t) num_vbs; slot++) {
    wgpuRenderPassEncoderSetVertexBuffer(yk___sg.wgpu.pass_enc, slot,
                                         vbs[slot]->wgpu.buf,
                                         (uint64_t) vb_offsets[slot]);
  }
  /* need to create throw-away bind groups for images */
  if (num_vs_imgs > 0) {
    if (num_vs_imgs > YK___SG_WGPU_MAX_SHADERSTAGE_IMAGES) {
      num_vs_imgs = YK___SG_WGPU_MAX_SHADERSTAGE_IMAGES;
    }
    WGPUBindGroupLayout vs_bgl =
        pip->shader->wgpu.stage[YK__SG_SHADERSTAGE_VS].bind_group_layout;
    YK__SOKOL_ASSERT(vs_bgl);
    WGPUBindGroup vs_img_bg =
        yk___sg_wgpu_create_images_bindgroup(vs_bgl, vs_imgs, num_vs_imgs);
    wgpuRenderPassEncoderSetBindGroup(yk___sg.wgpu.pass_enc, 1, vs_img_bg, 0,
                                      0);
    wgpuBindGroupRelease(vs_img_bg);
  } else {
    wgpuRenderPassEncoderSetBindGroup(yk___sg.wgpu.pass_enc, 1,
                                      yk___sg.wgpu.empty_bind_group, 0, 0);
  }
  if (num_fs_imgs > 0) {
    if (num_fs_imgs > YK___SG_WGPU_MAX_SHADERSTAGE_IMAGES) {
      num_fs_imgs = YK___SG_WGPU_MAX_SHADERSTAGE_IMAGES;
    }
    WGPUBindGroupLayout fs_bgl =
        pip->shader->wgpu.stage[YK__SG_SHADERSTAGE_FS].bind_group_layout;
    YK__SOKOL_ASSERT(fs_bgl);
    WGPUBindGroup fs_img_bg =
        yk___sg_wgpu_create_images_bindgroup(fs_bgl, fs_imgs, num_fs_imgs);
    wgpuRenderPassEncoderSetBindGroup(yk___sg.wgpu.pass_enc, 2, fs_img_bg, 0,
                                      0);
    wgpuBindGroupRelease(fs_img_bg);
  } else {
    wgpuRenderPassEncoderSetBindGroup(yk___sg.wgpu.pass_enc, 2,
                                      yk___sg.wgpu.empty_bind_group, 0, 0);
  }
}
YK___SOKOL_PRIVATE void
yk___sg_wgpu_apply_uniforms(yk__sg_shader_stage stage_index, int ub_index,
                            const yk__sg_range *data) {
  YK__SOKOL_ASSERT(yk___sg.wgpu.in_pass);
  YK__SOKOL_ASSERT(yk___sg.wgpu.pass_enc);
  YK__SOKOL_ASSERT((yk___sg.wgpu.ub.offset + data->size) <=
                   yk___sg.wgpu.ub.num_bytes);
  YK__SOKOL_ASSERT(
      (yk___sg.wgpu.ub.offset & (YK___SG_WGPU_STAGING_ALIGN - 1)) == 0);
  YK__SOKOL_ASSERT(yk___sg.wgpu.cur_pipeline &&
                   yk___sg.wgpu.cur_pipeline->shader);
  YK__SOKOL_ASSERT(yk___sg.wgpu.cur_pipeline->slot.id ==
                   yk___sg.wgpu.cur_pipeline_id.id);
  YK__SOKOL_ASSERT(yk___sg.wgpu.cur_pipeline->shader->slot.id ==
                   yk___sg.wgpu.cur_pipeline->cmn.shader_id.id);
  YK__SOKOL_ASSERT(ub_index <
                   yk___sg.wgpu.cur_pipeline->shader->cmn.stage[stage_index]
                       .num_uniform_blocks);
  YK__SOKOL_ASSERT(data->size <=
                   yk___sg.wgpu.cur_pipeline->shader->cmn.stage[stage_index]
                       .uniform_blocks[ub_index]
                       .size);
  YK__SOKOL_ASSERT(data->size <= YK___SG_WGPU_MAX_UNIFORM_UPDATE_SIZE);
  YK__SOKOL_ASSERT(0 != yk___sg.wgpu.ub.stage.ptr[yk___sg.wgpu.ub.stage.cur]);
  uint8_t *dst_ptr = yk___sg.wgpu.ub.stage.ptr[yk___sg.wgpu.ub.stage.cur] +
                     yk___sg.wgpu.ub.offset;
  memcpy(dst_ptr, data->ptr, data->size);
  yk___sg.wgpu.ub.bind_offsets[stage_index][ub_index] = yk___sg.wgpu.ub.offset;
  wgpuRenderPassEncoderSetBindGroup(
      yk___sg.wgpu.pass_enc,
      0, /* groupIndex 0 is reserved for uniform buffers */
      yk___sg.wgpu.ub.bindgroup,
      YK__SG_NUM_SHADER_STAGES * YK__SG_MAX_SHADERSTAGE_UBS,
      &yk___sg.wgpu.ub.bind_offsets[0][0]);
  yk___sg.wgpu.ub.offset = yk___sg_roundup(yk___sg.wgpu.ub.offset + data->size,
                                           YK___SG_WGPU_STAGING_ALIGN);
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_draw(int base_element, int num_elements,
                                          int num_instances) {
  YK__SOKOL_ASSERT(yk___sg.wgpu.in_pass);
  YK__SOKOL_ASSERT(yk___sg.wgpu.pass_enc);
  if (yk___sg.wgpu.draw_indexed) {
    wgpuRenderPassEncoderDrawIndexed(yk___sg.wgpu.pass_enc, num_elements,
                                     num_instances, base_element, 0, 0);
  } else {
    wgpuRenderPassEncoderDraw(yk___sg.wgpu.pass_enc, num_elements,
                              num_instances, base_element, 0);
  }
}
YK___SOKOL_PRIVATE void yk___sg_wgpu_update_buffer(yk___sg_buffer_t *buf,
                                                   const yk__sg_range *data) {
  YK__SOKOL_ASSERT(buf && data && data->ptr && (data->size > 0));
  uint32_t copied_num_bytes = yk___sg_wgpu_staging_copy_to_buffer(
      buf->wgpu.buf, 0, data->ptr, data->size);
  YK__SOKOL_ASSERT(copied_num_bytes > 0);
  YK___SOKOL_UNUSED(copied_num_bytes);
}
YK___SOKOL_PRIVATE int yk___sg_wgpu_append_buffer(yk___sg_buffer_t *buf,
                                                  const yk__sg_range *data,
                                                  bool new_frame) {
  YK__SOKOL_ASSERT(buf && data && data->ptr && (data->size > 0));
  YK___SOKOL_UNUSED(new_frame);
  uint32_t copied_num_bytes = yk___sg_wgpu_staging_copy_to_buffer(
      buf->wgpu.buf, buf->cmn.append_pos, data->ptr, data->size);
  YK__SOKOL_ASSERT(copied_num_bytes > 0);
  YK___SOKOL_UNUSED(copied_num_bytes);
  return (int) copied_num_bytes;
}
YK___SOKOL_PRIVATE void
yk___sg_wgpu_update_image(yk___sg_image_t *img, const yk__sg_image_data *data) {
  YK__SOKOL_ASSERT(img && data);
  bool success = yk___sg_wgpu_staging_copy_to_texture(img, data);
  YK__SOKOL_ASSERT(success);
  YK___SOKOL_UNUSED(success);
}
#endif
/*== BACKEND API WRAPPERS ====================================================*/
static inline void yk___sg_setup_backend(const yk__sg_desc *desc) {
#if defined(YK___SOKOL_ANY_GL)
  yk___sg_gl_setup_backend(desc);
#elif defined(YK__SOKOL_METAL)
  yk___sg_mtl_setup_backend(desc);
#elif defined(YK__SOKOL_D3D11)
  yk___sg_d3d11_setup_backend(desc);
#elif defined(YK__SOKOL_WGPU)
  yk___sg_wgpu_setup_backend(desc);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  yk___sg_dummy_setup_backend(desc);
#else
#error("INVALID BACKEND");
#endif
}
static inline void yk___sg_discard_backend(void) {
#if defined(YK___SOKOL_ANY_GL)
  yk___sg_gl_discard_backend();
#elif defined(YK__SOKOL_METAL)
  yk___sg_mtl_discard_backend();
#elif defined(YK__SOKOL_D3D11)
  yk___sg_d3d11_discard_backend();
#elif defined(YK__SOKOL_WGPU)
  yk___sg_wgpu_discard_backend();
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  yk___sg_dummy_discard_backend();
#else
#error("INVALID BACKEND");
#endif
}
static inline void yk___sg_reset_state_cache(void) {
#if defined(YK___SOKOL_ANY_GL)
  yk___sg_gl_reset_state_cache();
#elif defined(YK__SOKOL_METAL)
  yk___sg_mtl_reset_state_cache();
#elif defined(YK__SOKOL_D3D11)
  yk___sg_d3d11_reset_state_cache();
#elif defined(YK__SOKOL_WGPU)
  yk___sg_wgpu_reset_state_cache();
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  yk___sg_dummy_reset_state_cache();
#else
#error("INVALID BACKEND");
#endif
}
static inline void yk___sg_activate_context(yk___sg_context_t *ctx) {
#if defined(YK___SOKOL_ANY_GL)
  yk___sg_gl_activate_context(ctx);
#elif defined(YK__SOKOL_METAL)
  yk___sg_mtl_activate_context(ctx);
#elif defined(YK__SOKOL_D3D11)
  yk___sg_d3d11_activate_context(ctx);
#elif defined(YK__SOKOL_WGPU)
  yk___sg_wgpu_activate_context(ctx);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  yk___sg_dummy_activate_context(ctx);
#else
#error("INVALID BACKEND");
#endif
}
static inline yk__sg_resource_state
yk___sg_create_context(yk___sg_context_t *ctx) {
#if defined(YK___SOKOL_ANY_GL)
  return yk___sg_gl_create_context(ctx);
#elif defined(YK__SOKOL_METAL)
  return yk___sg_mtl_create_context(ctx);
#elif defined(YK__SOKOL_D3D11)
  return yk___sg_d3d11_create_context(ctx);
#elif defined(YK__SOKOL_WGPU)
  return yk___sg_wgpu_create_context(ctx);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  return yk___sg_dummy_create_context(ctx);
#else
#error("INVALID BACKEND");
#endif
}
static inline void yk___sg_destroy_context(yk___sg_context_t *ctx) {
#if defined(YK___SOKOL_ANY_GL)
  yk___sg_gl_destroy_context(ctx);
#elif defined(YK__SOKOL_METAL)
  yk___sg_mtl_destroy_context(ctx);
#elif defined(YK__SOKOL_D3D11)
  yk___sg_d3d11_destroy_context(ctx);
#elif defined(YK__SOKOL_WGPU)
  yk___sg_wgpu_destroy_context(ctx);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  yk___sg_dummy_destroy_context(ctx);
#else
#error("INVALID BACKEND");
#endif
}
static inline yk__sg_resource_state
yk___sg_create_buffer(yk___sg_buffer_t *buf, const yk__sg_buffer_desc *desc) {
#if defined(YK___SOKOL_ANY_GL)
  return yk___sg_gl_create_buffer(buf, desc);
#elif defined(YK__SOKOL_METAL)
  return yk___sg_mtl_create_buffer(buf, desc);
#elif defined(YK__SOKOL_D3D11)
  return yk___sg_d3d11_create_buffer(buf, desc);
#elif defined(YK__SOKOL_WGPU)
  return yk___sg_wgpu_create_buffer(buf, desc);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  return yk___sg_dummy_create_buffer(buf, desc);
#else
#error("INVALID BACKEND");
#endif
}
static inline void yk___sg_destroy_buffer(yk___sg_buffer_t *buf) {
#if defined(YK___SOKOL_ANY_GL)
  yk___sg_gl_destroy_buffer(buf);
#elif defined(YK__SOKOL_METAL)
  yk___sg_mtl_destroy_buffer(buf);
#elif defined(YK__SOKOL_D3D11)
  yk___sg_d3d11_destroy_buffer(buf);
#elif defined(YK__SOKOL_WGPU)
  yk___sg_wgpu_destroy_buffer(buf);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  yk___sg_dummy_destroy_buffer(buf);
#else
#error("INVALID BACKEND");
#endif
}
static inline yk__sg_resource_state
yk___sg_create_image(yk___sg_image_t *img, const yk__sg_image_desc *desc) {
#if defined(YK___SOKOL_ANY_GL)
  return yk___sg_gl_create_image(img, desc);
#elif defined(YK__SOKOL_METAL)
  return yk___sg_mtl_create_image(img, desc);
#elif defined(YK__SOKOL_D3D11)
  return yk___sg_d3d11_create_image(img, desc);
#elif defined(YK__SOKOL_WGPU)
  return yk___sg_wgpu_create_image(img, desc);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  return yk___sg_dummy_create_image(img, desc);
#else
#error("INVALID BACKEND");
#endif
}
static inline void yk___sg_destroy_image(yk___sg_image_t *img) {
#if defined(YK___SOKOL_ANY_GL)
  yk___sg_gl_destroy_image(img);
#elif defined(YK__SOKOL_METAL)
  yk___sg_mtl_destroy_image(img);
#elif defined(YK__SOKOL_D3D11)
  yk___sg_d3d11_destroy_image(img);
#elif defined(YK__SOKOL_WGPU)
  yk___sg_wgpu_destroy_image(img);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  yk___sg_dummy_destroy_image(img);
#else
#error("INVALID BACKEND");
#endif
}
static inline yk__sg_resource_state
yk___sg_create_shader(yk___sg_shader_t *shd, const yk__sg_shader_desc *desc) {
#if defined(YK___SOKOL_ANY_GL)
  return yk___sg_gl_create_shader(shd, desc);
#elif defined(YK__SOKOL_METAL)
  return yk___sg_mtl_create_shader(shd, desc);
#elif defined(YK__SOKOL_D3D11)
  return yk___sg_d3d11_create_shader(shd, desc);
#elif defined(YK__SOKOL_WGPU)
  return yk___sg_wgpu_create_shader(shd, desc);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  return yk___sg_dummy_create_shader(shd, desc);
#else
#error("INVALID BACKEND");
#endif
}
static inline void yk___sg_destroy_shader(yk___sg_shader_t *shd) {
#if defined(YK___SOKOL_ANY_GL)
  yk___sg_gl_destroy_shader(shd);
#elif defined(YK__SOKOL_METAL)
  yk___sg_mtl_destroy_shader(shd);
#elif defined(YK__SOKOL_D3D11)
  yk___sg_d3d11_destroy_shader(shd);
#elif defined(YK__SOKOL_WGPU)
  yk___sg_wgpu_destroy_shader(shd);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  yk___sg_dummy_destroy_shader(shd);
#else
#error("INVALID BACKEND");
#endif
}
static inline yk__sg_resource_state
yk___sg_create_pipeline(yk___sg_pipeline_t *pip, yk___sg_shader_t *shd,
                        const yk__sg_pipeline_desc *desc) {
#if defined(YK___SOKOL_ANY_GL)
  return yk___sg_gl_create_pipeline(pip, shd, desc);
#elif defined(YK__SOKOL_METAL)
  return yk___sg_mtl_create_pipeline(pip, shd, desc);
#elif defined(YK__SOKOL_D3D11)
  return yk___sg_d3d11_create_pipeline(pip, shd, desc);
#elif defined(YK__SOKOL_WGPU)
  return yk___sg_wgpu_create_pipeline(pip, shd, desc);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  return yk___sg_dummy_create_pipeline(pip, shd, desc);
#else
#error("INVALID BACKEND");
#endif
}
static inline void yk___sg_destroy_pipeline(yk___sg_pipeline_t *pip) {
#if defined(YK___SOKOL_ANY_GL)
  yk___sg_gl_destroy_pipeline(pip);
#elif defined(YK__SOKOL_METAL)
  yk___sg_mtl_destroy_pipeline(pip);
#elif defined(YK__SOKOL_D3D11)
  yk___sg_d3d11_destroy_pipeline(pip);
#elif defined(YK__SOKOL_WGPU)
  yk___sg_wgpu_destroy_pipeline(pip);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  yk___sg_dummy_destroy_pipeline(pip);
#else
#error("INVALID BACKEND");
#endif
}
static inline yk__sg_resource_state
yk___sg_create_pass(yk___sg_pass_t *pass, yk___sg_image_t **att_images,
                    const yk__sg_pass_desc *desc) {
#if defined(YK___SOKOL_ANY_GL)
  return yk___sg_gl_create_pass(pass, att_images, desc);
#elif defined(YK__SOKOL_METAL)
  return yk___sg_mtl_create_pass(pass, att_images, desc);
#elif defined(YK__SOKOL_D3D11)
  return yk___sg_d3d11_create_pass(pass, att_images, desc);
#elif defined(YK__SOKOL_WGPU)
  return yk___sg_wgpu_create_pass(pass, att_images, desc);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  return yk___sg_dummy_create_pass(pass, att_images, desc);
#else
#error("INVALID BACKEND");
#endif
}
static inline void yk___sg_destroy_pass(yk___sg_pass_t *pass) {
#if defined(YK___SOKOL_ANY_GL)
  yk___sg_gl_destroy_pass(pass);
#elif defined(YK__SOKOL_METAL)
  yk___sg_mtl_destroy_pass(pass);
#elif defined(YK__SOKOL_D3D11)
  yk___sg_d3d11_destroy_pass(pass);
#elif defined(YK__SOKOL_WGPU)
  return yk___sg_wgpu_destroy_pass(pass);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  yk___sg_dummy_destroy_pass(pass);
#else
#error("INVALID BACKEND");
#endif
}
static inline yk___sg_image_t *
yk___sg_pass_color_image(const yk___sg_pass_t *pass, int index) {
#if defined(YK___SOKOL_ANY_GL)
  return yk___sg_gl_pass_color_image(pass, index);
#elif defined(YK__SOKOL_METAL)
  return yk___sg_mtl_pass_color_image(pass, index);
#elif defined(YK__SOKOL_D3D11)
  return yk___sg_d3d11_pass_color_image(pass, index);
#elif defined(YK__SOKOL_WGPU)
  return yk___sg_wgpu_pass_color_image(pass, index);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  return yk___sg_dummy_pass_color_image(pass, index);
#else
#error("INVALID BACKEND");
#endif
}
static inline yk___sg_image_t *
yk___sg_pass_ds_image(const yk___sg_pass_t *pass) {
#if defined(YK___SOKOL_ANY_GL)
  return yk___sg_gl_pass_ds_image(pass);
#elif defined(YK__SOKOL_METAL)
  return yk___sg_mtl_pass_ds_image(pass);
#elif defined(YK__SOKOL_D3D11)
  return yk___sg_d3d11_pass_ds_image(pass);
#elif defined(YK__SOKOL_WGPU)
  return yk___sg_wgpu_pass_ds_image(pass);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  return yk___sg_dummy_pass_ds_image(pass);
#else
#error("INVALID BACKEND");
#endif
}
static inline void yk___sg_begin_pass(yk___sg_pass_t *pass,
                                      const yk__sg_pass_action *action, int w,
                                      int h) {
#if defined(YK___SOKOL_ANY_GL)
  yk___sg_gl_begin_pass(pass, action, w, h);
#elif defined(YK__SOKOL_METAL)
  yk___sg_mtl_begin_pass(pass, action, w, h);
#elif defined(YK__SOKOL_D3D11)
  yk___sg_d3d11_begin_pass(pass, action, w, h);
#elif defined(YK__SOKOL_WGPU)
  yk___sg_wgpu_begin_pass(pass, action, w, h);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  yk___sg_dummy_begin_pass(pass, action, w, h);
#else
#error("INVALID BACKEND");
#endif
}
static inline void yk___sg_end_pass(void) {
#if defined(YK___SOKOL_ANY_GL)
  yk___sg_gl_end_pass();
#elif defined(YK__SOKOL_METAL)
  yk___sg_mtl_end_pass();
#elif defined(YK__SOKOL_D3D11)
  yk___sg_d3d11_end_pass();
#elif defined(YK__SOKOL_WGPU)
  yk___sg_wgpu_end_pass();
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  yk___sg_dummy_end_pass();
#else
#error("INVALID BACKEND");
#endif
}
static inline void yk___sg_apply_viewport(int x, int y, int w, int h,
                                          bool origin_top_left) {
#if defined(YK___SOKOL_ANY_GL)
  yk___sg_gl_apply_viewport(x, y, w, h, origin_top_left);
#elif defined(YK__SOKOL_METAL)
  yk___sg_mtl_apply_viewport(x, y, w, h, origin_top_left);
#elif defined(YK__SOKOL_D3D11)
  yk___sg_d3d11_apply_viewport(x, y, w, h, origin_top_left);
#elif defined(YK__SOKOL_WGPU)
  yk___sg_wgpu_apply_viewport(x, y, w, h, origin_top_left);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  yk___sg_dummy_apply_viewport(x, y, w, h, origin_top_left);
#else
#error("INVALID BACKEND");
#endif
}
static inline void yk___sg_apply_scissor_rect(int x, int y, int w, int h,
                                              bool origin_top_left) {
#if defined(YK___SOKOL_ANY_GL)
  yk___sg_gl_apply_scissor_rect(x, y, w, h, origin_top_left);
#elif defined(YK__SOKOL_METAL)
  yk___sg_mtl_apply_scissor_rect(x, y, w, h, origin_top_left);
#elif defined(YK__SOKOL_D3D11)
  yk___sg_d3d11_apply_scissor_rect(x, y, w, h, origin_top_left);
#elif defined(YK__SOKOL_WGPU)
  yk___sg_wgpu_apply_scissor_rect(x, y, w, h, origin_top_left);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  yk___sg_dummy_apply_scissor_rect(x, y, w, h, origin_top_left);
#else
#error("INVALID BACKEND");
#endif
}
static inline void yk___sg_apply_pipeline(yk___sg_pipeline_t *pip) {
#if defined(YK___SOKOL_ANY_GL)
  yk___sg_gl_apply_pipeline(pip);
#elif defined(YK__SOKOL_METAL)
  yk___sg_mtl_apply_pipeline(pip);
#elif defined(YK__SOKOL_D3D11)
  yk___sg_d3d11_apply_pipeline(pip);
#elif defined(YK__SOKOL_WGPU)
  yk___sg_wgpu_apply_pipeline(pip);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  yk___sg_dummy_apply_pipeline(pip);
#else
#error("INVALID BACKEND");
#endif
}
static inline void yk___sg_apply_bindings(
    yk___sg_pipeline_t *pip, yk___sg_buffer_t **vbs, const int *vb_offsets,
    int num_vbs, yk___sg_buffer_t *ib, int ib_offset, yk___sg_image_t **vs_imgs,
    int num_vs_imgs, yk___sg_image_t **fs_imgs, int num_fs_imgs) {
#if defined(YK___SOKOL_ANY_GL)
  yk___sg_gl_apply_bindings(pip, vbs, vb_offsets, num_vbs, ib, ib_offset,
                            vs_imgs, num_vs_imgs, fs_imgs, num_fs_imgs);
#elif defined(YK__SOKOL_METAL)
  yk___sg_mtl_apply_bindings(pip, vbs, vb_offsets, num_vbs, ib, ib_offset,
                             vs_imgs, num_vs_imgs, fs_imgs, num_fs_imgs);
#elif defined(YK__SOKOL_D3D11)
  yk___sg_d3d11_apply_bindings(pip, vbs, vb_offsets, num_vbs, ib, ib_offset,
                               vs_imgs, num_vs_imgs, fs_imgs, num_fs_imgs);
#elif defined(YK__SOKOL_WGPU)
  yk___sg_wgpu_apply_bindings(pip, vbs, vb_offsets, num_vbs, ib, ib_offset,
                              vs_imgs, num_vs_imgs, fs_imgs, num_fs_imgs);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  yk___sg_dummy_apply_bindings(pip, vbs, vb_offsets, num_vbs, ib, ib_offset,
                               vs_imgs, num_vs_imgs, fs_imgs, num_fs_imgs);
#else
#error("INVALID BACKEND");
#endif
}
static inline void yk___sg_apply_uniforms(yk__sg_shader_stage stage_index,
                                          int ub_index,
                                          const yk__sg_range *data) {
#if defined(YK___SOKOL_ANY_GL)
  yk___sg_gl_apply_uniforms(stage_index, ub_index, data);
#elif defined(YK__SOKOL_METAL)
  yk___sg_mtl_apply_uniforms(stage_index, ub_index, data);
#elif defined(YK__SOKOL_D3D11)
  yk___sg_d3d11_apply_uniforms(stage_index, ub_index, data);
#elif defined(YK__SOKOL_WGPU)
  yk___sg_wgpu_apply_uniforms(stage_index, ub_index, data);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  yk___sg_dummy_apply_uniforms(stage_index, ub_index, data);
#else
#error("INVALID BACKEND");
#endif
}
static inline void yk___sg_draw(int base_element, int num_elements,
                                int num_instances) {
#if defined(YK___SOKOL_ANY_GL)
  yk___sg_gl_draw(base_element, num_elements, num_instances);
#elif defined(YK__SOKOL_METAL)
  yk___sg_mtl_draw(base_element, num_elements, num_instances);
#elif defined(YK__SOKOL_D3D11)
  yk___sg_d3d11_draw(base_element, num_elements, num_instances);
#elif defined(YK__SOKOL_WGPU)
  yk___sg_wgpu_draw(base_element, num_elements, num_instances);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  yk___sg_dummy_draw(base_element, num_elements, num_instances);
#else
#error("INVALID BACKEND");
#endif
}
static inline void yk___sg_commit(void) {
#if defined(YK___SOKOL_ANY_GL)
  yk___sg_gl_commit();
#elif defined(YK__SOKOL_METAL)
  yk___sg_mtl_commit();
#elif defined(YK__SOKOL_D3D11)
  yk___sg_d3d11_commit();
#elif defined(YK__SOKOL_WGPU)
  yk___sg_wgpu_commit();
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  yk___sg_dummy_commit();
#else
#error("INVALID BACKEND");
#endif
}
static inline void yk___sg_update_buffer(yk___sg_buffer_t *buf,
                                         const yk__sg_range *data) {
#if defined(YK___SOKOL_ANY_GL)
  yk___sg_gl_update_buffer(buf, data);
#elif defined(YK__SOKOL_METAL)
  yk___sg_mtl_update_buffer(buf, data);
#elif defined(YK__SOKOL_D3D11)
  yk___sg_d3d11_update_buffer(buf, data);
#elif defined(YK__SOKOL_WGPU)
  yk___sg_wgpu_update_buffer(buf, data);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  yk___sg_dummy_update_buffer(buf, data);
#else
#error("INVALID BACKEND");
#endif
}
static inline int yk___sg_append_buffer(yk___sg_buffer_t *buf,
                                        const yk__sg_range *data,
                                        bool new_frame) {
#if defined(YK___SOKOL_ANY_GL)
  return yk___sg_gl_append_buffer(buf, data, new_frame);
#elif defined(YK__SOKOL_METAL)
  return yk___sg_mtl_append_buffer(buf, data, new_frame);
#elif defined(YK__SOKOL_D3D11)
  return yk___sg_d3d11_append_buffer(buf, data, new_frame);
#elif defined(YK__SOKOL_WGPU)
  return yk___sg_wgpu_append_buffer(buf, data, new_frame);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  return yk___sg_dummy_append_buffer(buf, data, new_frame);
#else
#error("INVALID BACKEND");
#endif
}
static inline void yk___sg_update_image(yk___sg_image_t *img,
                                        const yk__sg_image_data *data) {
#if defined(YK___SOKOL_ANY_GL)
  yk___sg_gl_update_image(img, data);
#elif defined(YK__SOKOL_METAL)
  yk___sg_mtl_update_image(img, data);
#elif defined(YK__SOKOL_D3D11)
  yk___sg_d3d11_update_image(img, data);
#elif defined(YK__SOKOL_WGPU)
  yk___sg_wgpu_update_image(img, data);
#elif defined(YK__SOKOL_DUMMY_BACKEND)
  yk___sg_dummy_update_image(img, data);
#else
#error("INVALID BACKEND");
#endif
}
/*== RESOURCE POOLS ==========================================================*/
YK___SOKOL_PRIVATE void yk___sg_init_pool(yk___sg_pool_t *pool, int num) {
  YK__SOKOL_ASSERT(pool && (num >= 1));
  /* slot 0 is reserved for the 'invalid id', so bump the pool size by 1 */
  pool->size = num + 1;
  pool->queue_top = 0;
  /* generation counters indexable by pool slot index, slot 0 is reserved */
  size_t gen_ctrs_size = sizeof(uint32_t) * (size_t) pool->size;
  pool->gen_ctrs = (uint32_t *) YK__SOKOL_MALLOC(gen_ctrs_size);
  YK__SOKOL_ASSERT(pool->gen_ctrs);
  memset(pool->gen_ctrs, 0, gen_ctrs_size);
  /* it's not a bug to only reserve 'num' here */
  pool->free_queue = (int *) YK__SOKOL_MALLOC(sizeof(int) * (size_t) num);
  YK__SOKOL_ASSERT(pool->free_queue);
  /* never allocate the zero-th pool item since the invalid id is 0 */
  for (int i = pool->size - 1; i >= 1; i--) {
    pool->free_queue[pool->queue_top++] = i;
  }
}
YK___SOKOL_PRIVATE void yk___sg_discard_pool(yk___sg_pool_t *pool) {
  YK__SOKOL_ASSERT(pool);
  YK__SOKOL_ASSERT(pool->free_queue);
  YK__SOKOL_FREE(pool->free_queue);
  pool->free_queue = 0;
  YK__SOKOL_ASSERT(pool->gen_ctrs);
  YK__SOKOL_FREE(pool->gen_ctrs);
  pool->gen_ctrs = 0;
  pool->size = 0;
  pool->queue_top = 0;
}
YK___SOKOL_PRIVATE int yk___sg_pool_alloc_index(yk___sg_pool_t *pool) {
  YK__SOKOL_ASSERT(pool);
  YK__SOKOL_ASSERT(pool->free_queue);
  if (pool->queue_top > 0) {
    int slot_index = pool->free_queue[--pool->queue_top];
    YK__SOKOL_ASSERT((slot_index > 0) && (slot_index < pool->size));
    return slot_index;
  } else {
    /* pool exhausted */
    return YK___SG_INVALID_SLOT_INDEX;
  }
}
YK___SOKOL_PRIVATE void yk___sg_pool_free_index(yk___sg_pool_t *pool,
                                                int slot_index) {
  YK__SOKOL_ASSERT((slot_index > YK___SG_INVALID_SLOT_INDEX) &&
                   (slot_index < pool->size));
  YK__SOKOL_ASSERT(pool);
  YK__SOKOL_ASSERT(pool->free_queue);
  YK__SOKOL_ASSERT(pool->queue_top < pool->size);
#ifdef YK__SOKOL_DEBUG
  /* debug check against double-free */
  for (int i = 0; i < pool->queue_top; i++) {
    YK__SOKOL_ASSERT(pool->free_queue[i] != slot_index);
  }
#endif
  pool->free_queue[pool->queue_top++] = slot_index;
  YK__SOKOL_ASSERT(pool->queue_top <= (pool->size - 1));
}
YK___SOKOL_PRIVATE void yk___sg_reset_slot(yk___sg_slot_t *slot) {
  YK__SOKOL_ASSERT(slot);
  memset(slot, 0, sizeof(yk___sg_slot_t));
}
YK___SOKOL_PRIVATE void yk___sg_reset_buffer(yk___sg_buffer_t *buf) {
  YK__SOKOL_ASSERT(buf);
  yk___sg_slot_t slot = buf->slot;
  memset(buf, 0, sizeof(yk___sg_buffer_t));
  buf->slot = slot;
  buf->slot.state = YK__SG_RESOURCESTATE_ALLOC;
}
YK___SOKOL_PRIVATE void yk___sg_reset_image(yk___sg_image_t *img) {
  YK__SOKOL_ASSERT(img);
  yk___sg_slot_t slot = img->slot;
  memset(img, 0, sizeof(yk___sg_image_t));
  img->slot = slot;
  img->slot.state = YK__SG_RESOURCESTATE_ALLOC;
}
YK___SOKOL_PRIVATE void yk___sg_reset_shader(yk___sg_shader_t *shd) {
  YK__SOKOL_ASSERT(shd);
  yk___sg_slot_t slot = shd->slot;
  memset(shd, 0, sizeof(yk___sg_shader_t));
  shd->slot = slot;
  shd->slot.state = YK__SG_RESOURCESTATE_ALLOC;
}
YK___SOKOL_PRIVATE void yk___sg_reset_pipeline(yk___sg_pipeline_t *pip) {
  YK__SOKOL_ASSERT(pip);
  yk___sg_slot_t slot = pip->slot;
  memset(pip, 0, sizeof(yk___sg_pipeline_t));
  pip->slot = slot;
  pip->slot.state = YK__SG_RESOURCESTATE_ALLOC;
}
YK___SOKOL_PRIVATE void yk___sg_reset_pass(yk___sg_pass_t *pass) {
  YK__SOKOL_ASSERT(pass);
  yk___sg_slot_t slot = pass->slot;
  memset(pass, 0, sizeof(yk___sg_pass_t));
  pass->slot = slot;
  pass->slot.state = YK__SG_RESOURCESTATE_ALLOC;
}
YK___SOKOL_PRIVATE void yk___sg_reset_context(yk___sg_context_t *ctx) {
  YK__SOKOL_ASSERT(ctx);
  yk___sg_slot_t slot = ctx->slot;
  memset(ctx, 0, sizeof(yk___sg_context_t));
  ctx->slot = slot;
  ctx->slot.state = YK__SG_RESOURCESTATE_ALLOC;
}
YK___SOKOL_PRIVATE void yk___sg_setup_pools(yk___sg_pools_t *p,
                                            const yk__sg_desc *desc) {
  YK__SOKOL_ASSERT(p);
  YK__SOKOL_ASSERT(desc);
  /* note: the pools here will have an additional item, since slot 0 is reserved */
  YK__SOKOL_ASSERT((desc->buffer_pool_size > 0) &&
                   (desc->buffer_pool_size < YK___SG_MAX_POOL_SIZE));
  yk___sg_init_pool(&p->buffer_pool, desc->buffer_pool_size);
  size_t buffer_pool_byte_size =
      sizeof(yk___sg_buffer_t) * (size_t) p->buffer_pool.size;
  p->buffers = (yk___sg_buffer_t *) YK__SOKOL_MALLOC(buffer_pool_byte_size);
  YK__SOKOL_ASSERT(p->buffers);
  memset(p->buffers, 0, buffer_pool_byte_size);
  YK__SOKOL_ASSERT((desc->image_pool_size > 0) &&
                   (desc->image_pool_size < YK___SG_MAX_POOL_SIZE));
  yk___sg_init_pool(&p->image_pool, desc->image_pool_size);
  size_t image_pool_byte_size =
      sizeof(yk___sg_image_t) * (size_t) p->image_pool.size;
  p->images = (yk___sg_image_t *) YK__SOKOL_MALLOC(image_pool_byte_size);
  YK__SOKOL_ASSERT(p->images);
  memset(p->images, 0, image_pool_byte_size);
  YK__SOKOL_ASSERT((desc->shader_pool_size > 0) &&
                   (desc->shader_pool_size < YK___SG_MAX_POOL_SIZE));
  yk___sg_init_pool(&p->shader_pool, desc->shader_pool_size);
  size_t shader_pool_byte_size =
      sizeof(yk___sg_shader_t) * (size_t) p->shader_pool.size;
  p->shaders = (yk___sg_shader_t *) YK__SOKOL_MALLOC(shader_pool_byte_size);
  YK__SOKOL_ASSERT(p->shaders);
  memset(p->shaders, 0, shader_pool_byte_size);
  YK__SOKOL_ASSERT((desc->pipeline_pool_size > 0) &&
                   (desc->pipeline_pool_size < YK___SG_MAX_POOL_SIZE));
  yk___sg_init_pool(&p->pipeline_pool, desc->pipeline_pool_size);
  size_t pipeline_pool_byte_size =
      sizeof(yk___sg_pipeline_t) * (size_t) p->pipeline_pool.size;
  p->pipelines =
      (yk___sg_pipeline_t *) YK__SOKOL_MALLOC(pipeline_pool_byte_size);
  YK__SOKOL_ASSERT(p->pipelines);
  memset(p->pipelines, 0, pipeline_pool_byte_size);
  YK__SOKOL_ASSERT((desc->pass_pool_size > 0) &&
                   (desc->pass_pool_size < YK___SG_MAX_POOL_SIZE));
  yk___sg_init_pool(&p->pass_pool, desc->pass_pool_size);
  size_t pass_pool_byte_size =
      sizeof(yk___sg_pass_t) * (size_t) p->pass_pool.size;
  p->passes = (yk___sg_pass_t *) YK__SOKOL_MALLOC(pass_pool_byte_size);
  YK__SOKOL_ASSERT(p->passes);
  memset(p->passes, 0, pass_pool_byte_size);
  YK__SOKOL_ASSERT((desc->context_pool_size > 0) &&
                   (desc->context_pool_size < YK___SG_MAX_POOL_SIZE));
  yk___sg_init_pool(&p->context_pool, desc->context_pool_size);
  size_t context_pool_byte_size =
      sizeof(yk___sg_context_t) * (size_t) p->context_pool.size;
  p->contexts = (yk___sg_context_t *) YK__SOKOL_MALLOC(context_pool_byte_size);
  YK__SOKOL_ASSERT(p->contexts);
  memset(p->contexts, 0, context_pool_byte_size);
}
YK___SOKOL_PRIVATE void yk___sg_discard_pools(yk___sg_pools_t *p) {
  YK__SOKOL_ASSERT(p);
  YK__SOKOL_FREE(p->contexts);
  p->contexts = 0;
  YK__SOKOL_FREE(p->passes);
  p->passes = 0;
  YK__SOKOL_FREE(p->pipelines);
  p->pipelines = 0;
  YK__SOKOL_FREE(p->shaders);
  p->shaders = 0;
  YK__SOKOL_FREE(p->images);
  p->images = 0;
  YK__SOKOL_FREE(p->buffers);
  p->buffers = 0;
  yk___sg_discard_pool(&p->context_pool);
  yk___sg_discard_pool(&p->pass_pool);
  yk___sg_discard_pool(&p->pipeline_pool);
  yk___sg_discard_pool(&p->shader_pool);
  yk___sg_discard_pool(&p->image_pool);
  yk___sg_discard_pool(&p->buffer_pool);
}
/* allocate the slot at slot_index:
    - bump the slot's generation counter
    - create a resource id from the generation counter and slot index
    - set the slot's id to this id
    - set the slot's state to ALLOC
    - return the resource id
*/
YK___SOKOL_PRIVATE uint32_t yk___sg_slot_alloc(yk___sg_pool_t *pool,
                                               yk___sg_slot_t *slot,
                                               int slot_index) {
  /* FIXME: add handling for an overflowing generation counter,
       for now, just overflow (another option is to disable
       the slot)
    */
  YK__SOKOL_ASSERT(pool && pool->gen_ctrs);
  YK__SOKOL_ASSERT((slot_index > YK___SG_INVALID_SLOT_INDEX) &&
                   (slot_index < pool->size));
  YK__SOKOL_ASSERT((slot->state == YK__SG_RESOURCESTATE_INITIAL) &&
                   (slot->id == YK__SG_INVALID_ID));
  uint32_t ctr = ++pool->gen_ctrs[slot_index];
  slot->id = (ctr << YK___SG_SLOT_SHIFT) | (slot_index & YK___SG_SLOT_MASK);
  slot->state = YK__SG_RESOURCESTATE_ALLOC;
  return slot->id;
}
/* extract slot index from id */
YK___SOKOL_PRIVATE int yk___sg_slot_index(uint32_t id) {
  int slot_index = (int) (id & YK___SG_SLOT_MASK);
  YK__SOKOL_ASSERT(YK___SG_INVALID_SLOT_INDEX != slot_index);
  return slot_index;
}
/* returns pointer to resource by id without matching id check */
YK___SOKOL_PRIVATE yk___sg_buffer_t *yk___sg_buffer_at(const yk___sg_pools_t *p,
                                                       uint32_t buf_id) {
  YK__SOKOL_ASSERT(p && (YK__SG_INVALID_ID != buf_id));
  int slot_index = yk___sg_slot_index(buf_id);
  YK__SOKOL_ASSERT((slot_index > YK___SG_INVALID_SLOT_INDEX) &&
                   (slot_index < p->buffer_pool.size));
  return &p->buffers[slot_index];
}
YK___SOKOL_PRIVATE yk___sg_image_t *yk___sg_image_at(const yk___sg_pools_t *p,
                                                     uint32_t img_id) {
  YK__SOKOL_ASSERT(p && (YK__SG_INVALID_ID != img_id));
  int slot_index = yk___sg_slot_index(img_id);
  YK__SOKOL_ASSERT((slot_index > YK___SG_INVALID_SLOT_INDEX) &&
                   (slot_index < p->image_pool.size));
  return &p->images[slot_index];
}
YK___SOKOL_PRIVATE yk___sg_shader_t *yk___sg_shader_at(const yk___sg_pools_t *p,
                                                       uint32_t shd_id) {
  YK__SOKOL_ASSERT(p && (YK__SG_INVALID_ID != shd_id));
  int slot_index = yk___sg_slot_index(shd_id);
  YK__SOKOL_ASSERT((slot_index > YK___SG_INVALID_SLOT_INDEX) &&
                   (slot_index < p->shader_pool.size));
  return &p->shaders[slot_index];
}
YK___SOKOL_PRIVATE yk___sg_pipeline_t *
yk___sg_pipeline_at(const yk___sg_pools_t *p, uint32_t pip_id) {
  YK__SOKOL_ASSERT(p && (YK__SG_INVALID_ID != pip_id));
  int slot_index = yk___sg_slot_index(pip_id);
  YK__SOKOL_ASSERT((slot_index > YK___SG_INVALID_SLOT_INDEX) &&
                   (slot_index < p->pipeline_pool.size));
  return &p->pipelines[slot_index];
}
YK___SOKOL_PRIVATE yk___sg_pass_t *yk___sg_pass_at(const yk___sg_pools_t *p,
                                                   uint32_t pass_id) {
  YK__SOKOL_ASSERT(p && (YK__SG_INVALID_ID != pass_id));
  int slot_index = yk___sg_slot_index(pass_id);
  YK__SOKOL_ASSERT((slot_index > YK___SG_INVALID_SLOT_INDEX) &&
                   (slot_index < p->pass_pool.size));
  return &p->passes[slot_index];
}
YK___SOKOL_PRIVATE yk___sg_context_t *
yk___sg_context_at(const yk___sg_pools_t *p, uint32_t context_id) {
  YK__SOKOL_ASSERT(p && (YK__SG_INVALID_ID != context_id));
  int slot_index = yk___sg_slot_index(context_id);
  YK__SOKOL_ASSERT((slot_index > YK___SG_INVALID_SLOT_INDEX) &&
                   (slot_index < p->context_pool.size));
  return &p->contexts[slot_index];
}
/* returns pointer to resource with matching id check, may return 0 */
YK___SOKOL_PRIVATE yk___sg_buffer_t *
yk___sg_lookup_buffer(const yk___sg_pools_t *p, uint32_t buf_id) {
  if (YK__SG_INVALID_ID != buf_id) {
    yk___sg_buffer_t *buf = yk___sg_buffer_at(p, buf_id);
    if (buf->slot.id == buf_id) { return buf; }
  }
  return 0;
}
YK___SOKOL_PRIVATE yk___sg_image_t *
yk___sg_lookup_image(const yk___sg_pools_t *p, uint32_t img_id) {
  if (YK__SG_INVALID_ID != img_id) {
    yk___sg_image_t *img = yk___sg_image_at(p, img_id);
    if (img->slot.id == img_id) { return img; }
  }
  return 0;
}
YK___SOKOL_PRIVATE yk___sg_shader_t *
yk___sg_lookup_shader(const yk___sg_pools_t *p, uint32_t shd_id) {
  YK__SOKOL_ASSERT(p);
  if (YK__SG_INVALID_ID != shd_id) {
    yk___sg_shader_t *shd = yk___sg_shader_at(p, shd_id);
    if (shd->slot.id == shd_id) { return shd; }
  }
  return 0;
}
YK___SOKOL_PRIVATE yk___sg_pipeline_t *
yk___sg_lookup_pipeline(const yk___sg_pools_t *p, uint32_t pip_id) {
  YK__SOKOL_ASSERT(p);
  if (YK__SG_INVALID_ID != pip_id) {
    yk___sg_pipeline_t *pip = yk___sg_pipeline_at(p, pip_id);
    if (pip->slot.id == pip_id) { return pip; }
  }
  return 0;
}
YK___SOKOL_PRIVATE yk___sg_pass_t *yk___sg_lookup_pass(const yk___sg_pools_t *p,
                                                       uint32_t pass_id) {
  YK__SOKOL_ASSERT(p);
  if (YK__SG_INVALID_ID != pass_id) {
    yk___sg_pass_t *pass = yk___sg_pass_at(p, pass_id);
    if (pass->slot.id == pass_id) { return pass; }
  }
  return 0;
}
YK___SOKOL_PRIVATE yk___sg_context_t *
yk___sg_lookup_context(const yk___sg_pools_t *p, uint32_t ctx_id) {
  YK__SOKOL_ASSERT(p);
  if (YK__SG_INVALID_ID != ctx_id) {
    yk___sg_context_t *ctx = yk___sg_context_at(p, ctx_id);
    if (ctx->slot.id == ctx_id) { return ctx; }
  }
  return 0;
}
YK___SOKOL_PRIVATE void yk___sg_destroy_all_resources(yk___sg_pools_t *p,
                                                      uint32_t ctx_id) {
  /*  this is a bit dumb since it loops over all pool slots to
        find the occupied slots, on the other hand it is only ever
        executed at shutdown
        NOTE: ONLY EXECUTE THIS AT SHUTDOWN
              ...because the free queues will not be reset
              and the resource slots not be cleared!
    */
  for (int i = 1; i < p->buffer_pool.size; i++) {
    if (p->buffers[i].slot.ctx_id == ctx_id) {
      yk__sg_resource_state state = p->buffers[i].slot.state;
      if ((state == YK__SG_RESOURCESTATE_VALID) ||
          (state == YK__SG_RESOURCESTATE_FAILED)) {
        yk___sg_destroy_buffer(&p->buffers[i]);
      }
    }
  }
  for (int i = 1; i < p->image_pool.size; i++) {
    if (p->images[i].slot.ctx_id == ctx_id) {
      yk__sg_resource_state state = p->images[i].slot.state;
      if ((state == YK__SG_RESOURCESTATE_VALID) ||
          (state == YK__SG_RESOURCESTATE_FAILED)) {
        yk___sg_destroy_image(&p->images[i]);
      }
    }
  }
  for (int i = 1; i < p->shader_pool.size; i++) {
    if (p->shaders[i].slot.ctx_id == ctx_id) {
      yk__sg_resource_state state = p->shaders[i].slot.state;
      if ((state == YK__SG_RESOURCESTATE_VALID) ||
          (state == YK__SG_RESOURCESTATE_FAILED)) {
        yk___sg_destroy_shader(&p->shaders[i]);
      }
    }
  }
  for (int i = 1; i < p->pipeline_pool.size; i++) {
    if (p->pipelines[i].slot.ctx_id == ctx_id) {
      yk__sg_resource_state state = p->pipelines[i].slot.state;
      if ((state == YK__SG_RESOURCESTATE_VALID) ||
          (state == YK__SG_RESOURCESTATE_FAILED)) {
        yk___sg_destroy_pipeline(&p->pipelines[i]);
      }
    }
  }
  for (int i = 1; i < p->pass_pool.size; i++) {
    if (p->passes[i].slot.ctx_id == ctx_id) {
      yk__sg_resource_state state = p->passes[i].slot.state;
      if ((state == YK__SG_RESOURCESTATE_VALID) ||
          (state == YK__SG_RESOURCESTATE_FAILED)) {
        yk___sg_destroy_pass(&p->passes[i]);
      }
    }
  }
}
/*== VALIDATION LAYER ========================================================*/
#if defined(YK__SOKOL_DEBUG)
/* return a human readable string for an _sg_validate_error */
YK___SOKOL_PRIVATE const char *
yk___sg_validate_string(yk___sg_validate_error_t err) {
  switch (err) {
    /* buffer creation validation errors */
    case YK___SG_VALIDATE_BUFFERDESC_CANARY:
      return "yk__sg_buffer_desc not initialized";
    case YK___SG_VALIDATE_BUFFERDESC_SIZE:
      return "yk__sg_buffer_desc.size cannot be 0";
    case YK___SG_VALIDATE_BUFFERDESC_DATA:
      return "immutable buffers must be initialized with data "
             "(yk__sg_buffer_desc.data.ptr and yk__sg_buffer_desc.data.size)";
    case YK___SG_VALIDATE_BUFFERDESC_DATA_SIZE:
      return "immutable buffer data size differs from buffer size";
    case YK___SG_VALIDATE_BUFFERDESC_NO_DATA:
      return "dynamic/stream usage buffers cannot be initialized with data";
    /* image data (in image creation and updating) */
    case YK___SG_VALIDATE_IMAGEDATA_NODATA:
      return "yk__sg_image_data: no data (.ptr and/or .size is zero)";
    case YK___SG_VALIDATE_IMAGEDATA_DATA_SIZE:
      return "yk__sg_image_data: data size doesn't match expected surface size";
    /* image creation validation errros */
    case YK___SG_VALIDATE_IMAGEDESC_CANARY:
      return "yk__sg_image_desc not initialized";
    case YK___SG_VALIDATE_IMAGEDESC_WIDTH:
      return "yk__sg_image_desc.width must be > 0";
    case YK___SG_VALIDATE_IMAGEDESC_HEIGHT:
      return "yk__sg_image_desc.height must be > 0";
    case YK___SG_VALIDATE_IMAGEDESC_RT_PIXELFORMAT:
      return "invalid pixel format for render-target image";
    case YK___SG_VALIDATE_IMAGEDESC_NONRT_PIXELFORMAT:
      return "invalid pixel format for non-render-target image";
    case YK___SG_VALIDATE_IMAGEDESC_MSAA_BUT_NO_RT:
      return "non-render-target images cannot be multisampled";
    case YK___SG_VALIDATE_IMAGEDESC_NO_MSAA_RT_SUPPORT:
      return "MSAA not supported for this pixel format";
    case YK___SG_VALIDATE_IMAGEDESC_RT_IMMUTABLE:
      return "render target images must be YK__SG_USAGE_IMMUTABLE";
    case YK___SG_VALIDATE_IMAGEDESC_RT_NO_DATA:
      return "render target images cannot be initialized with data";
    case YK___SG_VALIDATE_IMAGEDESC_INJECTED_NO_DATA:
      return "images with injected textures cannot be initialized with data";
    case YK___SG_VALIDATE_IMAGEDESC_DYNAMIC_NO_DATA:
      return "dynamic/stream images cannot be initialized with data";
    case YK___SG_VALIDATE_IMAGEDESC_COMPRESSED_IMMUTABLE:
      return "compressed images must be immutable";
    /* shader creation */
    case YK___SG_VALIDATE_SHADERDESC_CANARY:
      return "yk__sg_shader_desc not initialized";
    case YK___SG_VALIDATE_SHADERDESC_SOURCE:
      return "shader source code required";
    case YK___SG_VALIDATE_SHADERDESC_BYTECODE:
      return "shader byte code required";
    case YK___SG_VALIDATE_SHADERDESC_SOURCE_OR_BYTECODE:
      return "shader source or byte code required";
    case YK___SG_VALIDATE_SHADERDESC_NO_BYTECODE_SIZE:
      return "shader byte code length (in bytes) required";
    case YK___SG_VALIDATE_SHADERDESC_NO_CONT_UBS:
      return "shader uniform blocks must occupy continuous slots";
    case YK___SG_VALIDATE_SHADERDESC_NO_CONT_UB_MEMBERS:
      return "uniform block members must occupy continuous slots";
    case YK___SG_VALIDATE_SHADERDESC_NO_UB_MEMBERS:
      return "GL backend requires uniform block member declarations";
    case YK___SG_VALIDATE_SHADERDESC_UB_MEMBER_NAME:
      return "uniform block member name missing";
    case YK___SG_VALIDATE_SHADERDESC_UB_SIZE_MISMATCH:
      return "size of uniform block members doesn't match uniform block size";
    case YK___SG_VALIDATE_SHADERDESC_NO_CONT_IMGS:
      return "shader images must occupy continuous slots";
    case YK___SG_VALIDATE_SHADERDESC_IMG_NAME:
      return "GL backend requires uniform block member names";
    case YK___SG_VALIDATE_SHADERDESC_ATTR_NAMES:
      return "GLES2 backend requires vertex attribute names";
    case YK___SG_VALIDATE_SHADERDESC_ATTR_SEMANTICS:
      return "D3D11 backend requires vertex attribute semantics";
    case YK___SG_VALIDATE_SHADERDESC_ATTR_STRING_TOO_LONG:
      return "vertex attribute name/semantic string too long (max len 16)";
    /* pipeline creation */
    case YK___SG_VALIDATE_PIPELINEDESC_CANARY:
      return "yk__sg_pipeline_desc not initialized";
    case YK___SG_VALIDATE_PIPELINEDESC_SHADER:
      return "yk__sg_pipeline_desc.shader missing or invalid";
    case YK___SG_VALIDATE_PIPELINEDESC_NO_ATTRS:
      return "yk__sg_pipeline_desc.layout.attrs is empty or not continuous";
    case YK___SG_VALIDATE_PIPELINEDESC_LAYOUT_STRIDE4:
      return "yk__sg_pipeline_desc.layout.buffers[].stride must be multiple of "
             "4";
    case YK___SG_VALIDATE_PIPELINEDESC_ATTR_NAME:
      return "GLES2/WebGL missing vertex attribute name in shader";
    case YK___SG_VALIDATE_PIPELINEDESC_ATTR_SEMANTICS:
      return "D3D11 missing vertex attribute semantics in shader";
    /* pass creation */
    case YK___SG_VALIDATE_PASSDESC_CANARY:
      return "yk__sg_pass_desc not initialized";
    case YK___SG_VALIDATE_PASSDESC_NO_COLOR_ATTS:
      return "yk__sg_pass_desc.color_attachments[0] must be valid";
    case YK___SG_VALIDATE_PASSDESC_NO_CONT_COLOR_ATTS:
      return "color attachments must occupy continuous slots";
    case YK___SG_VALIDATE_PASSDESC_IMAGE:
      return "pass attachment image is not valid";
    case YK___SG_VALIDATE_PASSDESC_MIPLEVEL:
      return "pass attachment mip level is bigger than image has mipmaps";
    case YK___SG_VALIDATE_PASSDESC_FACE:
      return "pass attachment image is cubemap, but face index is too big";
    case YK___SG_VALIDATE_PASSDESC_LAYER:
      return "pass attachment image is array texture, but layer index is too "
             "big";
    case YK___SG_VALIDATE_PASSDESC_SLICE:
      return "pass attachment image is 3d texture, but slice value is too big";
    case YK___SG_VALIDATE_PASSDESC_IMAGE_NO_RT:
      return "pass attachment image must be render targets";
    case YK___SG_VALIDATE_PASSDESC_COLOR_INV_PIXELFORMAT:
      return "pass color-attachment images must have a renderable pixel format";
    case YK___SG_VALIDATE_PASSDESC_DEPTH_INV_PIXELFORMAT:
      return "pass depth-attachment image must have depth pixel format";
    case YK___SG_VALIDATE_PASSDESC_IMAGE_SIZES:
      return "all pass attachments must have the same size";
    case YK___SG_VALIDATE_PASSDESC_IMAGE_SAMPLE_COUNTS:
      return "all pass attachments must have the same sample count";
    /* yk__sg_begin_pass */
    case YK___SG_VALIDATE_BEGINPASS_PASS:
      return "yk__sg_begin_pass: pass must be valid";
    case YK___SG_VALIDATE_BEGINPASS_IMAGE:
      return "yk__sg_begin_pass: one or more attachment images are not valid";
    /* yk__sg_apply_pipeline */
    case YK___SG_VALIDATE_APIP_PIPELINE_VALID_ID:
      return "yk__sg_apply_pipeline: invalid pipeline id provided";
    case YK___SG_VALIDATE_APIP_PIPELINE_EXISTS:
      return "yk__sg_apply_pipeline: pipeline object no longer alive";
    case YK___SG_VALIDATE_APIP_PIPELINE_VALID:
      return "yk__sg_apply_pipeline: pipeline object not in valid state";
    case YK___SG_VALIDATE_APIP_SHADER_EXISTS:
      return "yk__sg_apply_pipeline: shader object no longer alive";
    case YK___SG_VALIDATE_APIP_SHADER_VALID:
      return "yk__sg_apply_pipeline: shader object not in valid state";
    case YK___SG_VALIDATE_APIP_ATT_COUNT:
      return "yk__sg_apply_pipeline: number of pipeline color attachments "
             "doesn't match number of pass color attachments";
    case YK___SG_VALIDATE_APIP_COLOR_FORMAT:
      return "yk__sg_apply_pipeline: pipeline color attachment pixel format "
             "doesn't match pass color attachment pixel format";
    case YK___SG_VALIDATE_APIP_DEPTH_FORMAT:
      return "yk__sg_apply_pipeline: pipeline depth pixel_format doesn't match "
             "pass depth attachment pixel format";
    case YK___SG_VALIDATE_APIP_SAMPLE_COUNT:
      return "yk__sg_apply_pipeline: pipeline MSAA sample count doesn't match "
             "render pass attachment sample count";
    /* yk__sg_apply_bindings */
    case YK___SG_VALIDATE_ABND_PIPELINE:
      return "yk__sg_apply_bindings: must be called after "
             "yk__sg_apply_pipeline";
    case YK___SG_VALIDATE_ABND_PIPELINE_EXISTS:
      return "yk__sg_apply_bindings: currently applied pipeline object no "
             "longer alive";
    case YK___SG_VALIDATE_ABND_PIPELINE_VALID:
      return "yk__sg_apply_bindings: currently applied pipeline object not in "
             "valid state";
    case YK___SG_VALIDATE_ABND_VBS:
      return "yk__sg_apply_bindings: number of vertex buffers doesn't match "
             "number of pipeline vertex layouts";
    case YK___SG_VALIDATE_ABND_VB_EXISTS:
      return "yk__sg_apply_bindings: vertex buffer no longer alive";
    case YK___SG_VALIDATE_ABND_VB_TYPE:
      return "yk__sg_apply_bindings: buffer in vertex buffer slot is not a "
             "YK__SG_BUFFERTYPE_VERTEXBUFFER";
    case YK___SG_VALIDATE_ABND_VB_OVERFLOW:
      return "yk__sg_apply_bindings: buffer in vertex buffer slot is overflown";
    case YK___SG_VALIDATE_ABND_NO_IB:
      return "yk__sg_apply_bindings: pipeline object defines indexed "
             "rendering, but no index buffer provided";
    case YK___SG_VALIDATE_ABND_IB:
      return "yk__sg_apply_bindings: pipeline object defines non-indexed "
             "rendering, but index buffer provided";
    case YK___SG_VALIDATE_ABND_IB_EXISTS:
      return "yk__sg_apply_bindings: index buffer no longer alive";
    case YK___SG_VALIDATE_ABND_IB_TYPE:
      return "yk__sg_apply_bindings: buffer in index buffer slot is not a "
             "YK__SG_BUFFERTYPE_INDEXBUFFER";
    case YK___SG_VALIDATE_ABND_IB_OVERFLOW:
      return "yk__sg_apply_bindings: buffer in index buffer slot is overflown";
    case YK___SG_VALIDATE_ABND_VS_IMGS:
      return "yk__sg_apply_bindings: vertex shader image count doesn't match "
             "yk__sg_shader_desc";
    case YK___SG_VALIDATE_ABND_VS_IMG_EXISTS:
      return "yk__sg_apply_bindings: vertex shader image no longer alive";
    case YK___SG_VALIDATE_ABND_VS_IMG_TYPES:
      return "yk__sg_apply_bindings: one or more vertex shader image types "
             "don't match yk__sg_shader_desc";
    case YK___SG_VALIDATE_ABND_FS_IMGS:
      return "yk__sg_apply_bindings: fragment shader image count doesn't match "
             "yk__sg_shader_desc";
    case YK___SG_VALIDATE_ABND_FS_IMG_EXISTS:
      return "yk__sg_apply_bindings: fragment shader image no longer alive";
    case YK___SG_VALIDATE_ABND_FS_IMG_TYPES:
      return "yk__sg_apply_bindings: one or more fragment shader image types "
             "don't match yk__sg_shader_desc";
    /* yk__sg_apply_uniforms */
    case YK___SG_VALIDATE_AUB_NO_PIPELINE:
      return "yk__sg_apply_uniforms: must be called after "
             "yk__sg_apply_pipeline()";
    case YK___SG_VALIDATE_AUB_NO_UB_AT_SLOT:
      return "yk__sg_apply_uniforms: no uniform block declaration at this "
             "shader stage UB slot";
    case YK___SG_VALIDATE_AUB_SIZE:
      return "yk__sg_apply_uniforms: data size exceeds declared uniform block "
             "size";
    /* yk__sg_update_buffer */
    case YK___SG_VALIDATE_UPDATEBUF_USAGE:
      return "yk__sg_update_buffer: cannot update immutable buffer";
    case YK___SG_VALIDATE_UPDATEBUF_SIZE:
      return "yk__sg_update_buffer: update size is bigger than buffer size";
    case YK___SG_VALIDATE_UPDATEBUF_ONCE:
      return "yk__sg_update_buffer: only one update allowed per buffer and "
             "frame";
    case YK___SG_VALIDATE_UPDATEBUF_APPEND:
      return "yk__sg_update_buffer: cannot call yk__sg_update_buffer and "
             "yk__sg_append_buffer in same frame";
    /* yk__sg_append_buffer */
    case YK___SG_VALIDATE_APPENDBUF_USAGE:
      return "yk__sg_append_buffer: cannot append to immutable buffer";
    case YK___SG_VALIDATE_APPENDBUF_SIZE:
      return "yk__sg_append_buffer: overall appended size is bigger than "
             "buffer size";
    case YK___SG_VALIDATE_APPENDBUF_UPDATE:
      return "yk__sg_append_buffer: cannot call yk__sg_append_buffer and "
             "yk__sg_update_buffer in same frame";
    /* yk__sg_update_image */
    case YK___SG_VALIDATE_UPDIMG_USAGE:
      return "yk__sg_update_image: cannot update immutable image";
    case YK___SG_VALIDATE_UPDIMG_ONCE:
      return "yk__sg_update_image: only one update allowed per image and frame";
    default:
      return "unknown validation error";
  }
}
#endif /* defined(YK__SOKOL_DEBUG) */
/*-- validation checks -------------------------------------------------------*/
#if defined(YK__SOKOL_DEBUG)
YK___SOKOL_PRIVATE void yk___sg_validate_begin(void) {
  yk___sg.validate_error = YK___SG_VALIDATE_SUCCESS;
}
YK___SOKOL_PRIVATE void yk___sg_validate(bool cond,
                                         yk___sg_validate_error_t err) {
  if (!cond) {
    yk___sg.validate_error = err;
    YK__SOKOL_LOG(yk___sg_validate_string(err));
  }
}
YK___SOKOL_PRIVATE bool yk___sg_validate_end(void) {
  if (yk___sg.validate_error != YK___SG_VALIDATE_SUCCESS) {
#if !defined(YK__SOKOL_VALIDATE_NON_FATAL)
    YK__SOKOL_LOG("^^^^  SOKOL-GFX VALIDATION FAILED, TERMINATING ^^^^");
    YK__SOKOL_ASSERT(false);
#endif
    return false;
  } else {
    return true;
  }
}
#endif
YK___SOKOL_PRIVATE bool
yk___sg_validate_buffer_desc(const yk__sg_buffer_desc *desc) {
#if !defined(YK__SOKOL_DEBUG)
  YK___SOKOL_UNUSED(desc);
  return true;
#else
  YK__SOKOL_ASSERT(desc);
  YK__SOKOL_VALIDATE_BEGIN();
  YK__SOKOL_VALIDATE(desc->_start_canary == 0,
                     YK___SG_VALIDATE_BUFFERDESC_CANARY);
  YK__SOKOL_VALIDATE(desc->_end_canary == 0,
                     YK___SG_VALIDATE_BUFFERDESC_CANARY);
  YK__SOKOL_VALIDATE(desc->size > 0, YK___SG_VALIDATE_BUFFERDESC_SIZE);
  bool injected = (0 != desc->gl_buffers[0]) || (0 != desc->mtl_buffers[0]) ||
                  (0 != desc->d3d11_buffer) || (0 != desc->wgpu_buffer);
  if (!injected && (desc->usage == YK__SG_USAGE_IMMUTABLE)) {
    YK__SOKOL_VALIDATE((0 != desc->data.ptr) && (desc->data.size > 0),
                       YK___SG_VALIDATE_BUFFERDESC_DATA);
    YK__SOKOL_VALIDATE(desc->size == desc->data.size,
                       YK___SG_VALIDATE_BUFFERDESC_DATA_SIZE);
  } else {
    YK__SOKOL_VALIDATE(0 == desc->data.ptr,
                       YK___SG_VALIDATE_BUFFERDESC_NO_DATA);
  }
  return YK__SOKOL_VALIDATE_END();
#endif
}
YK___SOKOL_PRIVATE void
yk___sg_validate_image_data(const yk__sg_image_data *data,
                            yk__sg_pixel_format fmt, int width, int height,
                            int num_faces, int num_mips, int num_slices) {
#if !defined(YK__SOKOL_DEBUG)
  YK___SOKOL_UNUSED(data);
  YK___SOKOL_UNUSED(fmt);
  YK___SOKOL_UNUSED(width);
  YK___SOKOL_UNUSED(height);
  YK___SOKOL_UNUSED(num_faces);
  YK___SOKOL_UNUSED(num_mips);
  YK___SOKOL_UNUSED(num_slices);
#else
  for (int face_index = 0; face_index < num_faces; face_index++) {
    for (int mip_index = 0; mip_index < num_mips; mip_index++) {
      const bool has_data = data->subimage[face_index][mip_index].ptr != 0;
      const bool has_size = data->subimage[face_index][mip_index].size > 0;
      YK__SOKOL_VALIDATE(has_data && has_size,
                         YK___SG_VALIDATE_IMAGEDATA_NODATA);
      const int mip_width = yk___sg_max(width >> mip_index, 1);
      const int mip_height = yk___sg_max(height >> mip_index, 1);
      const int bytes_per_slice =
          yk___sg_surface_pitch(fmt, mip_width, mip_height, 1);
      const int expected_size = bytes_per_slice * num_slices;
      YK__SOKOL_VALIDATE(expected_size ==
                             (int) data->subimage[face_index][mip_index].size,
                         YK___SG_VALIDATE_IMAGEDATA_DATA_SIZE);
    }
  }
#endif
}
YK___SOKOL_PRIVATE bool
yk___sg_validate_image_desc(const yk__sg_image_desc *desc) {
#if !defined(YK__SOKOL_DEBUG)
  YK___SOKOL_UNUSED(desc);
  return true;
#else
  YK__SOKOL_ASSERT(desc);
  YK__SOKOL_VALIDATE_BEGIN();
  YK__SOKOL_VALIDATE(desc->_start_canary == 0,
                     YK___SG_VALIDATE_IMAGEDESC_CANARY);
  YK__SOKOL_VALIDATE(desc->_end_canary == 0, YK___SG_VALIDATE_IMAGEDESC_CANARY);
  YK__SOKOL_VALIDATE(desc->width > 0, YK___SG_VALIDATE_IMAGEDESC_WIDTH);
  YK__SOKOL_VALIDATE(desc->height > 0, YK___SG_VALIDATE_IMAGEDESC_HEIGHT);
  const yk__sg_pixel_format fmt = desc->pixel_format;
  const yk__sg_usage usage = desc->usage;
  const bool injected = (0 != desc->gl_textures[0]) ||
                        (0 != desc->mtl_textures[0]) ||
                        (0 != desc->d3d11_texture) || (0 != desc->wgpu_texture);
  if (desc->render_target) {
    YK__SOKOL_ASSERT(((int) fmt >= 0) && ((int) fmt < YK___SG_PIXELFORMAT_NUM));
    YK__SOKOL_VALIDATE(yk___sg.formats[fmt].render,
                       YK___SG_VALIDATE_IMAGEDESC_RT_PIXELFORMAT);
/* on GLES2, sample count for render targets is completely ignored */
#if defined(YK__SOKOL_GLES2) || defined(YK__SOKOL_GLES3)
    if (!yk___sg.gl.gles2) {
#endif
      if (desc->sample_count > 1) {
        YK__SOKOL_VALIDATE(yk___sg.features.msaa_render_targets &&
                               yk___sg.formats[fmt].msaa,
                           YK___SG_VALIDATE_IMAGEDESC_NO_MSAA_RT_SUPPORT);
      }
#if defined(YK__SOKOL_GLES2) || defined(YK__SOKOL_GLES3)
    }
#endif
    YK__SOKOL_VALIDATE(usage == YK__SG_USAGE_IMMUTABLE,
                       YK___SG_VALIDATE_IMAGEDESC_RT_IMMUTABLE);
    YK__SOKOL_VALIDATE(desc->data.subimage[0][0].ptr == 0,
                       YK___SG_VALIDATE_IMAGEDESC_RT_NO_DATA);
  } else {
    YK__SOKOL_VALIDATE(desc->sample_count <= 1,
                       YK___SG_VALIDATE_IMAGEDESC_MSAA_BUT_NO_RT);
    const bool valid_nonrt_fmt =
        !yk___sg_is_valid_rendertarget_depth_format(fmt);
    YK__SOKOL_VALIDATE(valid_nonrt_fmt,
                       YK___SG_VALIDATE_IMAGEDESC_NONRT_PIXELFORMAT);
    const bool is_compressed =
        yk___sg_is_compressed_pixel_format(desc->pixel_format);
    const bool is_immutable = (usage == YK__SG_USAGE_IMMUTABLE);
    if (is_compressed) {
      YK__SOKOL_VALIDATE(is_immutable,
                         YK___SG_VALIDATE_IMAGEDESC_COMPRESSED_IMMUTABLE);
    }
    if (!injected && is_immutable) {
      // image desc must have valid data
      yk___sg_validate_image_data(&desc->data, desc->pixel_format, desc->width,
                                  desc->height,
                                  (desc->type == YK__SG_IMAGETYPE_CUBE) ? 6 : 1,
                                  desc->num_mipmaps, desc->num_slices);
    } else {
      // image desc must not have data
      for (int face_index = 0; face_index < YK__SG_CUBEFACE_NUM; face_index++) {
        for (int mip_index = 0; mip_index < YK__SG_MAX_MIPMAPS; mip_index++) {
          const bool no_data =
              0 == desc->data.subimage[face_index][mip_index].ptr;
          const bool no_size =
              0 == desc->data.subimage[face_index][mip_index].size;
          if (injected) {
            YK__SOKOL_VALIDATE(no_data && no_size,
                               YK___SG_VALIDATE_IMAGEDESC_INJECTED_NO_DATA);
          }
          if (!is_immutable) {
            YK__SOKOL_VALIDATE(no_data && no_size,
                               YK___SG_VALIDATE_IMAGEDESC_DYNAMIC_NO_DATA);
          }
        }
      }
    }
  }
  return YK__SOKOL_VALIDATE_END();
#endif
}
YK___SOKOL_PRIVATE bool
yk___sg_validate_shader_desc(const yk__sg_shader_desc *desc) {
#if !defined(YK__SOKOL_DEBUG)
  YK___SOKOL_UNUSED(desc);
  return true;
#else
  YK__SOKOL_ASSERT(desc);
  YK__SOKOL_VALIDATE_BEGIN();
  YK__SOKOL_VALIDATE(desc->_start_canary == 0,
                     YK___SG_VALIDATE_SHADERDESC_CANARY);
  YK__SOKOL_VALIDATE(desc->_end_canary == 0,
                     YK___SG_VALIDATE_SHADERDESC_CANARY);
#if defined(YK__SOKOL_GLES2)
  YK__SOKOL_VALIDATE(0 != desc->attrs[0].name,
                     YK___SG_VALIDATE_SHADERDESC_ATTR_NAMES);
#elif defined(YK__SOKOL_D3D11)
  YK__SOKOL_VALIDATE(0 != desc->attrs[0].sem_name,
                     YK___SG_VALIDATE_SHADERDESC_ATTR_SEMANTICS);
#endif
#if defined(YK__SOKOL_GLCORE33) || defined(YK__SOKOL_GLES2) ||                 \
    defined(YK__SOKOL_GLES3)
  /* on GL, must provide shader source code */
  YK__SOKOL_VALIDATE(0 != desc->vs.source, YK___SG_VALIDATE_SHADERDESC_SOURCE);
  YK__SOKOL_VALIDATE(0 != desc->fs.source, YK___SG_VALIDATE_SHADERDESC_SOURCE);
#elif defined(YK__SOKOL_METAL) || defined(YK__SOKOL_D3D11)
  /* on Metal or D3D11, must provide shader source code or byte code */
  YK__SOKOL_VALIDATE((0 != desc->vs.source) || (0 != desc->vs.bytecode.ptr),
                     YK___SG_VALIDATE_SHADERDESC_SOURCE_OR_BYTECODE);
  YK__SOKOL_VALIDATE((0 != desc->fs.source) || (0 != desc->fs.bytecode.ptr),
                     YK___SG_VALIDATE_SHADERDESC_SOURCE_OR_BYTECODE);
#elif defined(YK__SOKOL_WGPU)
  /* on WGPU byte code must be provided */
  YK__SOKOL_VALIDATE((0 != desc->vs.bytecode.ptr),
                     YK___SG_VALIDATE_SHADERDESC_BYTECODE);
  YK__SOKOL_VALIDATE((0 != desc->fs.bytecode.ptr),
                     YK___SG_VALIDATE_SHADERDESC_BYTECODE);
#else
  /* Dummy Backend, don't require source or bytecode */
#endif
  for (int i = 0; i < YK__SG_MAX_VERTEX_ATTRIBUTES; i++) {
    if (desc->attrs[i].name) {
      YK__SOKOL_VALIDATE(strlen(desc->attrs[i].name) < YK___SG_STRING_SIZE,
                         YK___SG_VALIDATE_SHADERDESC_ATTR_STRING_TOO_LONG);
    }
    if (desc->attrs[i].sem_name) {
      YK__SOKOL_VALIDATE(strlen(desc->attrs[i].sem_name) < YK___SG_STRING_SIZE,
                         YK___SG_VALIDATE_SHADERDESC_ATTR_STRING_TOO_LONG);
    }
  }
  /* if shader byte code, the size must also be provided */
  if (0 != desc->vs.bytecode.ptr) {
    YK__SOKOL_VALIDATE(desc->vs.bytecode.size > 0,
                       YK___SG_VALIDATE_SHADERDESC_NO_BYTECODE_SIZE);
  }
  if (0 != desc->fs.bytecode.ptr) {
    YK__SOKOL_VALIDATE(desc->fs.bytecode.size > 0,
                       YK___SG_VALIDATE_SHADERDESC_NO_BYTECODE_SIZE);
  }
  for (int stage_index = 0; stage_index < YK__SG_NUM_SHADER_STAGES;
       stage_index++) {
    const yk__sg_shader_stage_desc *stage_desc =
        (stage_index == 0) ? &desc->vs : &desc->fs;
    bool uniform_blocks_continuous = true;
    for (int ub_index = 0; ub_index < YK__SG_MAX_SHADERSTAGE_UBS; ub_index++) {
      const yk__sg_shader_uniform_block_desc *ub_desc =
          &stage_desc->uniform_blocks[ub_index];
      if (ub_desc->size > 0) {
        YK__SOKOL_VALIDATE(uniform_blocks_continuous,
                           YK___SG_VALIDATE_SHADERDESC_NO_CONT_UBS);
        bool uniforms_continuous = true;
        int uniform_offset = 0;
        int num_uniforms = 0;
        for (int u_index = 0; u_index < YK__SG_MAX_UB_MEMBERS; u_index++) {
          const yk__sg_shader_uniform_desc *u_desc =
              &ub_desc->uniforms[u_index];
          if (u_desc->type != YK__SG_UNIFORMTYPE_INVALID) {
            YK__SOKOL_VALIDATE(uniforms_continuous,
                               YK___SG_VALIDATE_SHADERDESC_NO_CONT_UB_MEMBERS);
#if defined(YK__SOKOL_GLES2) || defined(YK__SOKOL_GLES3)
            YK__SOKOL_VALIDATE(0 != u_desc->name,
                               YK___SG_VALIDATE_SHADERDESC_UB_MEMBER_NAME);
#endif
            const int array_count = u_desc->array_count;
            uniform_offset += yk___sg_uniform_size(u_desc->type, array_count);
            num_uniforms++;
          } else {
            uniforms_continuous = false;
          }
        }
#if defined(YK__SOKOL_GLCORE33) || defined(YK__SOKOL_GLES2) ||                 \
    defined(YK__SOKOL_GLES3)
        YK__SOKOL_VALIDATE((size_t) uniform_offset == ub_desc->size,
                           YK___SG_VALIDATE_SHADERDESC_UB_SIZE_MISMATCH);
        YK__SOKOL_VALIDATE(num_uniforms > 0,
                           YK___SG_VALIDATE_SHADERDESC_NO_UB_MEMBERS);
#else
        YK___SOKOL_UNUSED(uniform_offset);
        YK___SOKOL_UNUSED(num_uniforms);
#endif
      } else {
        uniform_blocks_continuous = false;
      }
    }
    bool images_continuous = true;
    for (int img_index = 0; img_index < YK__SG_MAX_SHADERSTAGE_IMAGES;
         img_index++) {
      const yk__sg_shader_image_desc *img_desc = &stage_desc->images[img_index];
      if (img_desc->image_type != YK___SG_IMAGETYPE_DEFAULT) {
        YK__SOKOL_VALIDATE(images_continuous,
                           YK___SG_VALIDATE_SHADERDESC_NO_CONT_IMGS);
#if defined(YK__SOKOL_GLES2)
        YK__SOKOL_VALIDATE(0 != img_desc->name,
                           YK___SG_VALIDATE_SHADERDESC_IMG_NAME);
#endif
      } else {
        images_continuous = false;
      }
    }
  }
  return YK__SOKOL_VALIDATE_END();
#endif
}
YK___SOKOL_PRIVATE bool
yk___sg_validate_pipeline_desc(const yk__sg_pipeline_desc *desc) {
#if !defined(YK__SOKOL_DEBUG)
  YK___SOKOL_UNUSED(desc);
  return true;
#else
  YK__SOKOL_ASSERT(desc);
  YK__SOKOL_VALIDATE_BEGIN();
  YK__SOKOL_VALIDATE(desc->_start_canary == 0,
                     YK___SG_VALIDATE_PIPELINEDESC_CANARY);
  YK__SOKOL_VALIDATE(desc->_end_canary == 0,
                     YK___SG_VALIDATE_PIPELINEDESC_CANARY);
  YK__SOKOL_VALIDATE(desc->shader.id != YK__SG_INVALID_ID,
                     YK___SG_VALIDATE_PIPELINEDESC_SHADER);
  for (int buf_index = 0; buf_index < YK__SG_MAX_SHADERSTAGE_BUFFERS;
       buf_index++) {
    const yk__sg_buffer_layout_desc *l_desc = &desc->layout.buffers[buf_index];
    if (l_desc->stride == 0) { continue; }
    YK__SOKOL_VALIDATE((l_desc->stride & 3) == 0,
                       YK___SG_VALIDATE_PIPELINEDESC_LAYOUT_STRIDE4);
  }
  YK__SOKOL_VALIDATE(desc->layout.attrs[0].format !=
                         YK__SG_VERTEXFORMAT_INVALID,
                     YK___SG_VALIDATE_PIPELINEDESC_NO_ATTRS);
  const yk___sg_shader_t *shd =
      yk___sg_lookup_shader(&yk___sg.pools, desc->shader.id);
  YK__SOKOL_VALIDATE(0 != shd, YK___SG_VALIDATE_PIPELINEDESC_SHADER);
  if (shd) {
    YK__SOKOL_VALIDATE(shd->slot.state == YK__SG_RESOURCESTATE_VALID,
                       YK___SG_VALIDATE_PIPELINEDESC_SHADER);
    bool attrs_cont = true;
    for (int attr_index = 0; attr_index < YK__SG_MAX_VERTEX_ATTRIBUTES;
         attr_index++) {
      const yk__sg_vertex_attr_desc *a_desc = &desc->layout.attrs[attr_index];
      if (a_desc->format == YK__SG_VERTEXFORMAT_INVALID) {
        attrs_cont = false;
        continue;
      }
      YK__SOKOL_VALIDATE(attrs_cont, YK___SG_VALIDATE_PIPELINEDESC_NO_ATTRS);
      YK__SOKOL_ASSERT(a_desc->buffer_index < YK__SG_MAX_SHADERSTAGE_BUFFERS);
#if defined(YK__SOKOL_GLES2)
      /* on GLES2, vertex attribute names must be provided */
      YK__SOKOL_VALIDATE(!yk___sg_strempty(&shd->gl.attrs[attr_index].name),
                         YK___SG_VALIDATE_PIPELINEDESC_ATTR_NAME);
#elif defined(YK__SOKOL_D3D11)
      /* on D3D11, semantic names (and semantic indices) must be provided */
      YK__SOKOL_VALIDATE(
          !yk___sg_strempty(&shd->d3d11.attrs[attr_index].sem_name),
          YK___SG_VALIDATE_PIPELINEDESC_ATTR_SEMANTICS);
#endif
    }
  }
  return YK__SOKOL_VALIDATE_END();
#endif
}
YK___SOKOL_PRIVATE bool
yk___sg_validate_pass_desc(const yk__sg_pass_desc *desc) {
#if !defined(YK__SOKOL_DEBUG)
  YK___SOKOL_UNUSED(desc);
  return true;
#else
  YK__SOKOL_ASSERT(desc);
  YK__SOKOL_VALIDATE_BEGIN();
  YK__SOKOL_VALIDATE(desc->_start_canary == 0,
                     YK___SG_VALIDATE_PASSDESC_CANARY);
  YK__SOKOL_VALIDATE(desc->_end_canary == 0, YK___SG_VALIDATE_PASSDESC_CANARY);
  bool atts_cont = true;
  int width = -1, height = -1, sample_count = -1;
  for (int att_index = 0; att_index < YK__SG_MAX_COLOR_ATTACHMENTS;
       att_index++) {
    const yk__sg_pass_attachment_desc *att =
        &desc->color_attachments[att_index];
    if (att->image.id == YK__SG_INVALID_ID) {
      YK__SOKOL_VALIDATE(att_index > 0,
                         YK___SG_VALIDATE_PASSDESC_NO_COLOR_ATTS);
      atts_cont = false;
      continue;
    }
    YK__SOKOL_VALIDATE(atts_cont, YK___SG_VALIDATE_PASSDESC_NO_CONT_COLOR_ATTS);
    const yk___sg_image_t *img =
        yk___sg_lookup_image(&yk___sg.pools, att->image.id);
    YK__SOKOL_ASSERT(img);
    YK__SOKOL_VALIDATE(img->slot.state == YK__SG_RESOURCESTATE_VALID,
                       YK___SG_VALIDATE_PASSDESC_IMAGE);
    YK__SOKOL_VALIDATE(att->mip_level < img->cmn.num_mipmaps,
                       YK___SG_VALIDATE_PASSDESC_MIPLEVEL);
    if (img->cmn.type == YK__SG_IMAGETYPE_CUBE) {
      YK__SOKOL_VALIDATE(att->slice < 6, YK___SG_VALIDATE_PASSDESC_FACE);
    } else if (img->cmn.type == YK__SG_IMAGETYPE_ARRAY) {
      YK__SOKOL_VALIDATE(att->slice < img->cmn.num_slices,
                         YK___SG_VALIDATE_PASSDESC_LAYER);
    } else if (img->cmn.type == YK__SG_IMAGETYPE_3D) {
      YK__SOKOL_VALIDATE(att->slice < img->cmn.num_slices,
                         YK___SG_VALIDATE_PASSDESC_SLICE);
    }
    YK__SOKOL_VALIDATE(img->cmn.render_target,
                       YK___SG_VALIDATE_PASSDESC_IMAGE_NO_RT);
    if (att_index == 0) {
      width = img->cmn.width >> att->mip_level;
      height = img->cmn.height >> att->mip_level;
      sample_count = img->cmn.sample_count;
    } else {
      YK__SOKOL_VALIDATE(width == img->cmn.width >> att->mip_level,
                         YK___SG_VALIDATE_PASSDESC_IMAGE_SIZES);
      YK__SOKOL_VALIDATE(height == img->cmn.height >> att->mip_level,
                         YK___SG_VALIDATE_PASSDESC_IMAGE_SIZES);
      YK__SOKOL_VALIDATE(sample_count == img->cmn.sample_count,
                         YK___SG_VALIDATE_PASSDESC_IMAGE_SAMPLE_COUNTS);
    }
    YK__SOKOL_VALIDATE(
        yk___sg_is_valid_rendertarget_color_format(img->cmn.pixel_format),
        YK___SG_VALIDATE_PASSDESC_COLOR_INV_PIXELFORMAT);
  }
  if (desc->depth_stencil_attachment.image.id != YK__SG_INVALID_ID) {
    const yk__sg_pass_attachment_desc *att = &desc->depth_stencil_attachment;
    const yk___sg_image_t *img =
        yk___sg_lookup_image(&yk___sg.pools, att->image.id);
    YK__SOKOL_ASSERT(img);
    YK__SOKOL_VALIDATE(img->slot.state == YK__SG_RESOURCESTATE_VALID,
                       YK___SG_VALIDATE_PASSDESC_IMAGE);
    YK__SOKOL_VALIDATE(att->mip_level < img->cmn.num_mipmaps,
                       YK___SG_VALIDATE_PASSDESC_MIPLEVEL);
    if (img->cmn.type == YK__SG_IMAGETYPE_CUBE) {
      YK__SOKOL_VALIDATE(att->slice < 6, YK___SG_VALIDATE_PASSDESC_FACE);
    } else if (img->cmn.type == YK__SG_IMAGETYPE_ARRAY) {
      YK__SOKOL_VALIDATE(att->slice < img->cmn.num_slices,
                         YK___SG_VALIDATE_PASSDESC_LAYER);
    } else if (img->cmn.type == YK__SG_IMAGETYPE_3D) {
      YK__SOKOL_VALIDATE(att->slice < img->cmn.num_slices,
                         YK___SG_VALIDATE_PASSDESC_SLICE);
    }
    YK__SOKOL_VALIDATE(img->cmn.render_target,
                       YK___SG_VALIDATE_PASSDESC_IMAGE_NO_RT);
    YK__SOKOL_VALIDATE(width == img->cmn.width >> att->mip_level,
                       YK___SG_VALIDATE_PASSDESC_IMAGE_SIZES);
    YK__SOKOL_VALIDATE(height == img->cmn.height >> att->mip_level,
                       YK___SG_VALIDATE_PASSDESC_IMAGE_SIZES);
    YK__SOKOL_VALIDATE(sample_count == img->cmn.sample_count,
                       YK___SG_VALIDATE_PASSDESC_IMAGE_SAMPLE_COUNTS);
    YK__SOKOL_VALIDATE(
        yk___sg_is_valid_rendertarget_depth_format(img->cmn.pixel_format),
        YK___SG_VALIDATE_PASSDESC_DEPTH_INV_PIXELFORMAT);
  }
  return YK__SOKOL_VALIDATE_END();
#endif
}
YK___SOKOL_PRIVATE bool yk___sg_validate_begin_pass(yk___sg_pass_t *pass) {
#if !defined(YK__SOKOL_DEBUG)
  YK___SOKOL_UNUSED(pass);
  return true;
#else
  YK__SOKOL_VALIDATE_BEGIN();
  YK__SOKOL_VALIDATE(pass->slot.state == YK__SG_RESOURCESTATE_VALID,
                     YK___SG_VALIDATE_BEGINPASS_PASS);
  for (int i = 0; i < YK__SG_MAX_COLOR_ATTACHMENTS; i++) {
    const yk___sg_pass_attachment_t *att = &pass->cmn.color_atts[i];
    const yk___sg_image_t *img = yk___sg_pass_color_image(pass, i);
    if (img) {
      YK__SOKOL_VALIDATE(img->slot.state == YK__SG_RESOURCESTATE_VALID,
                         YK___SG_VALIDATE_BEGINPASS_IMAGE);
      YK__SOKOL_VALIDATE(img->slot.id == att->image_id.id,
                         YK___SG_VALIDATE_BEGINPASS_IMAGE);
    }
  }
  const yk___sg_image_t *ds_img = yk___sg_pass_ds_image(pass);
  if (ds_img) {
    const yk___sg_pass_attachment_t *att = &pass->cmn.ds_att;
    YK__SOKOL_VALIDATE(ds_img->slot.state == YK__SG_RESOURCESTATE_VALID,
                       YK___SG_VALIDATE_BEGINPASS_IMAGE);
    YK__SOKOL_VALIDATE(ds_img->slot.id == att->image_id.id,
                       YK___SG_VALIDATE_BEGINPASS_IMAGE);
  }
  return YK__SOKOL_VALIDATE_END();
#endif
}
YK___SOKOL_PRIVATE bool
yk___sg_validate_apply_pipeline(yk__sg_pipeline pip_id) {
#if !defined(YK__SOKOL_DEBUG)
  YK___SOKOL_UNUSED(pip_id);
  return true;
#else
  YK__SOKOL_VALIDATE_BEGIN();
  /* the pipeline object must be alive and valid */
  YK__SOKOL_VALIDATE(pip_id.id != YK__SG_INVALID_ID,
                     YK___SG_VALIDATE_APIP_PIPELINE_VALID_ID);
  const yk___sg_pipeline_t *pip =
      yk___sg_lookup_pipeline(&yk___sg.pools, pip_id.id);
  YK__SOKOL_VALIDATE(pip != 0, YK___SG_VALIDATE_APIP_PIPELINE_EXISTS);
  if (!pip) { return YK__SOKOL_VALIDATE_END(); }
  YK__SOKOL_VALIDATE(pip->slot.state == YK__SG_RESOURCESTATE_VALID,
                     YK___SG_VALIDATE_APIP_PIPELINE_VALID);
  /* the pipeline's shader must be alive and valid */
  YK__SOKOL_ASSERT(pip->shader);
  YK__SOKOL_VALIDATE(pip->shader->slot.id == pip->cmn.shader_id.id,
                     YK___SG_VALIDATE_APIP_SHADER_EXISTS);
  YK__SOKOL_VALIDATE(pip->shader->slot.state == YK__SG_RESOURCESTATE_VALID,
                     YK___SG_VALIDATE_APIP_SHADER_VALID);
  /* check that pipeline attributes match current pass attributes */
  const yk___sg_pass_t *pass =
      yk___sg_lookup_pass(&yk___sg.pools, yk___sg.cur_pass.id);
  if (pass) {
    /* an offscreen pass */
    YK__SOKOL_VALIDATE(pip->cmn.color_attachment_count ==
                           pass->cmn.num_color_atts,
                       YK___SG_VALIDATE_APIP_ATT_COUNT);
    for (int i = 0; i < pip->cmn.color_attachment_count; i++) {
      const yk___sg_image_t *att_img = yk___sg_pass_color_image(pass, i);
      YK__SOKOL_VALIDATE(pip->cmn.color_formats[i] == att_img->cmn.pixel_format,
                         YK___SG_VALIDATE_APIP_COLOR_FORMAT);
      YK__SOKOL_VALIDATE(pip->cmn.sample_count == att_img->cmn.sample_count,
                         YK___SG_VALIDATE_APIP_SAMPLE_COUNT);
    }
    const yk___sg_image_t *att_dsimg = yk___sg_pass_ds_image(pass);
    if (att_dsimg) {
      YK__SOKOL_VALIDATE(pip->cmn.depth_format == att_dsimg->cmn.pixel_format,
                         YK___SG_VALIDATE_APIP_DEPTH_FORMAT);
    } else {
      YK__SOKOL_VALIDATE(pip->cmn.depth_format == YK__SG_PIXELFORMAT_NONE,
                         YK___SG_VALIDATE_APIP_DEPTH_FORMAT);
    }
  } else {
    /* default pass */
    YK__SOKOL_VALIDATE(pip->cmn.color_attachment_count == 1,
                       YK___SG_VALIDATE_APIP_ATT_COUNT);
    YK__SOKOL_VALIDATE(pip->cmn.color_formats[0] ==
                           yk___sg.desc.context.color_format,
                       YK___SG_VALIDATE_APIP_COLOR_FORMAT);
    YK__SOKOL_VALIDATE(pip->cmn.depth_format ==
                           yk___sg.desc.context.depth_format,
                       YK___SG_VALIDATE_APIP_DEPTH_FORMAT);
    YK__SOKOL_VALIDATE(pip->cmn.sample_count ==
                           yk___sg.desc.context.sample_count,
                       YK___SG_VALIDATE_APIP_SAMPLE_COUNT);
  }
  return YK__SOKOL_VALIDATE_END();
#endif
}
YK___SOKOL_PRIVATE bool
yk___sg_validate_apply_bindings(const yk__sg_bindings *bindings) {
#if !defined(YK__SOKOL_DEBUG)
  YK___SOKOL_UNUSED(bindings);
  return true;
#else
  YK__SOKOL_VALIDATE_BEGIN();
  /* a pipeline object must have been applied */
  YK__SOKOL_VALIDATE(yk___sg.cur_pipeline.id != YK__SG_INVALID_ID,
                     YK___SG_VALIDATE_ABND_PIPELINE);
  const yk___sg_pipeline_t *pip =
      yk___sg_lookup_pipeline(&yk___sg.pools, yk___sg.cur_pipeline.id);
  YK__SOKOL_VALIDATE(pip != 0, YK___SG_VALIDATE_ABND_PIPELINE_EXISTS);
  if (!pip) { return YK__SOKOL_VALIDATE_END(); }
  YK__SOKOL_VALIDATE(pip->slot.state == YK__SG_RESOURCESTATE_VALID,
                     YK___SG_VALIDATE_ABND_PIPELINE_VALID);
  YK__SOKOL_ASSERT(pip->shader &&
                   (pip->cmn.shader_id.id == pip->shader->slot.id));
  /* has expected vertex buffers, and vertex buffers still exist */
  for (int i = 0; i < YK__SG_MAX_SHADERSTAGE_BUFFERS; i++) {
    if (bindings->vertex_buffers[i].id != YK__SG_INVALID_ID) {
      YK__SOKOL_VALIDATE(pip->cmn.vertex_layout_valid[i],
                         YK___SG_VALIDATE_ABND_VBS);
      /* buffers in vertex-buffer-slots must be of type YK__SG_BUFFERTYPE_VERTEXBUFFER */
      const yk___sg_buffer_t *buf =
          yk___sg_lookup_buffer(&yk___sg.pools, bindings->vertex_buffers[i].id);
      YK__SOKOL_VALIDATE(buf != 0, YK___SG_VALIDATE_ABND_VB_EXISTS);
      if (buf && buf->slot.state == YK__SG_RESOURCESTATE_VALID) {
        YK__SOKOL_VALIDATE(YK__SG_BUFFERTYPE_VERTEXBUFFER == buf->cmn.type,
                           YK___SG_VALIDATE_ABND_VB_TYPE);
        YK__SOKOL_VALIDATE(!buf->cmn.append_overflow,
                           YK___SG_VALIDATE_ABND_VB_OVERFLOW);
      }
    } else {
      /* vertex buffer provided in a slot which has no vertex layout in pipeline */
      YK__SOKOL_VALIDATE(!pip->cmn.vertex_layout_valid[i],
                         YK___SG_VALIDATE_ABND_VBS);
    }
  }
  /* index buffer expected or not, and index buffer still exists */
  if (pip->cmn.index_type == YK__SG_INDEXTYPE_NONE) {
    /* pipeline defines non-indexed rendering, but index buffer provided */
    YK__SOKOL_VALIDATE(bindings->index_buffer.id == YK__SG_INVALID_ID,
                       YK___SG_VALIDATE_ABND_IB);
  } else {
    /* pipeline defines indexed rendering, but no index buffer provided */
    YK__SOKOL_VALIDATE(bindings->index_buffer.id != YK__SG_INVALID_ID,
                       YK___SG_VALIDATE_ABND_NO_IB);
  }
  if (bindings->index_buffer.id != YK__SG_INVALID_ID) {
    /* buffer in index-buffer-slot must be of type YK__SG_BUFFERTYPE_INDEXBUFFER */
    const yk___sg_buffer_t *buf =
        yk___sg_lookup_buffer(&yk___sg.pools, bindings->index_buffer.id);
    YK__SOKOL_VALIDATE(buf != 0, YK___SG_VALIDATE_ABND_IB_EXISTS);
    if (buf && buf->slot.state == YK__SG_RESOURCESTATE_VALID) {
      YK__SOKOL_VALIDATE(YK__SG_BUFFERTYPE_INDEXBUFFER == buf->cmn.type,
                         YK___SG_VALIDATE_ABND_IB_TYPE);
      YK__SOKOL_VALIDATE(!buf->cmn.append_overflow,
                         YK___SG_VALIDATE_ABND_IB_OVERFLOW);
    }
  }
  /* has expected vertex shader images */
  for (int i = 0; i < YK__SG_MAX_SHADERSTAGE_IMAGES; i++) {
    yk___sg_shader_stage_t *stage =
        &pip->shader->cmn.stage[YK__SG_SHADERSTAGE_VS];
    if (bindings->vs_images[i].id != YK__SG_INVALID_ID) {
      YK__SOKOL_VALIDATE(i < stage->num_images, YK___SG_VALIDATE_ABND_VS_IMGS);
      const yk___sg_image_t *img =
          yk___sg_lookup_image(&yk___sg.pools, bindings->vs_images[i].id);
      YK__SOKOL_VALIDATE(img != 0, YK___SG_VALIDATE_ABND_VS_IMG_EXISTS);
      if (img && img->slot.state == YK__SG_RESOURCESTATE_VALID) {
        YK__SOKOL_VALIDATE(img->cmn.type == stage->images[i].image_type,
                           YK___SG_VALIDATE_ABND_VS_IMG_TYPES);
      }
    } else {
      YK__SOKOL_VALIDATE(i >= stage->num_images, YK___SG_VALIDATE_ABND_VS_IMGS);
    }
  }
  /* has expected fragment shader images */
  for (int i = 0; i < YK__SG_MAX_SHADERSTAGE_IMAGES; i++) {
    yk___sg_shader_stage_t *stage =
        &pip->shader->cmn.stage[YK__SG_SHADERSTAGE_FS];
    if (bindings->fs_images[i].id != YK__SG_INVALID_ID) {
      YK__SOKOL_VALIDATE(i < stage->num_images, YK___SG_VALIDATE_ABND_FS_IMGS);
      const yk___sg_image_t *img =
          yk___sg_lookup_image(&yk___sg.pools, bindings->fs_images[i].id);
      YK__SOKOL_VALIDATE(img != 0, YK___SG_VALIDATE_ABND_FS_IMG_EXISTS);
      if (img && img->slot.state == YK__SG_RESOURCESTATE_VALID) {
        YK__SOKOL_VALIDATE(img->cmn.type == stage->images[i].image_type,
                           YK___SG_VALIDATE_ABND_FS_IMG_TYPES);
      }
    } else {
      YK__SOKOL_VALIDATE(i >= stage->num_images, YK___SG_VALIDATE_ABND_FS_IMGS);
    }
  }
  return YK__SOKOL_VALIDATE_END();
#endif
}
YK___SOKOL_PRIVATE bool
yk___sg_validate_apply_uniforms(yk__sg_shader_stage stage_index, int ub_index,
                                const yk__sg_range *data) {
#if !defined(YK__SOKOL_DEBUG)
  YK___SOKOL_UNUSED(stage_index);
  YK___SOKOL_UNUSED(ub_index);
  YK___SOKOL_UNUSED(data);
  return true;
#else
  YK__SOKOL_ASSERT((stage_index == YK__SG_SHADERSTAGE_VS) ||
                   (stage_index == YK__SG_SHADERSTAGE_FS));
  YK__SOKOL_ASSERT((ub_index >= 0) && (ub_index < YK__SG_MAX_SHADERSTAGE_UBS));
  YK__SOKOL_VALIDATE_BEGIN();
  YK__SOKOL_VALIDATE(yk___sg.cur_pipeline.id != YK__SG_INVALID_ID,
                     YK___SG_VALIDATE_AUB_NO_PIPELINE);
  const yk___sg_pipeline_t *pip =
      yk___sg_lookup_pipeline(&yk___sg.pools, yk___sg.cur_pipeline.id);
  YK__SOKOL_ASSERT(pip && (pip->slot.id == yk___sg.cur_pipeline.id));
  YK__SOKOL_ASSERT(pip->shader &&
                   (pip->shader->slot.id == pip->cmn.shader_id.id));
  /* check that there is a uniform block at 'stage' and 'ub_index' */
  const yk___sg_shader_stage_t *stage = &pip->shader->cmn.stage[stage_index];
  YK__SOKOL_VALIDATE(ub_index < stage->num_uniform_blocks,
                     YK___SG_VALIDATE_AUB_NO_UB_AT_SLOT);
  /* check that the provided data size doesn't exceed the uniform block size */
  YK__SOKOL_VALIDATE(data->size <= stage->uniform_blocks[ub_index].size,
                     YK___SG_VALIDATE_AUB_SIZE);
  return YK__SOKOL_VALIDATE_END();
#endif
}
YK___SOKOL_PRIVATE bool
yk___sg_validate_update_buffer(const yk___sg_buffer_t *buf,
                               const yk__sg_range *data) {
#if !defined(YK__SOKOL_DEBUG)
  YK___SOKOL_UNUSED(buf);
  YK___SOKOL_UNUSED(data);
  return true;
#else
  YK__SOKOL_ASSERT(buf && data && data->ptr);
  YK__SOKOL_VALIDATE_BEGIN();
  YK__SOKOL_VALIDATE(buf->cmn.usage != YK__SG_USAGE_IMMUTABLE,
                     YK___SG_VALIDATE_UPDATEBUF_USAGE);
  YK__SOKOL_VALIDATE(buf->cmn.size >= (int) data->size,
                     YK___SG_VALIDATE_UPDATEBUF_SIZE);
  YK__SOKOL_VALIDATE(buf->cmn.update_frame_index != yk___sg.frame_index,
                     YK___SG_VALIDATE_UPDATEBUF_ONCE);
  YK__SOKOL_VALIDATE(buf->cmn.append_frame_index != yk___sg.frame_index,
                     YK___SG_VALIDATE_UPDATEBUF_APPEND);
  return YK__SOKOL_VALIDATE_END();
#endif
}
YK___SOKOL_PRIVATE bool
yk___sg_validate_append_buffer(const yk___sg_buffer_t *buf,
                               const yk__sg_range *data) {
#if !defined(YK__SOKOL_DEBUG)
  YK___SOKOL_UNUSED(buf);
  YK___SOKOL_UNUSED(data);
  return true;
#else
  YK__SOKOL_ASSERT(buf && data && data->ptr);
  YK__SOKOL_VALIDATE_BEGIN();
  YK__SOKOL_VALIDATE(buf->cmn.usage != YK__SG_USAGE_IMMUTABLE,
                     YK___SG_VALIDATE_APPENDBUF_USAGE);
  YK__SOKOL_VALIDATE(buf->cmn.size >= (buf->cmn.append_pos + (int) data->size),
                     YK___SG_VALIDATE_APPENDBUF_SIZE);
  YK__SOKOL_VALIDATE(buf->cmn.update_frame_index != yk___sg.frame_index,
                     YK___SG_VALIDATE_APPENDBUF_UPDATE);
  return YK__SOKOL_VALIDATE_END();
#endif
}
YK___SOKOL_PRIVATE bool
yk___sg_validate_update_image(const yk___sg_image_t *img,
                              const yk__sg_image_data *data) {
#if !defined(YK__SOKOL_DEBUG)
  YK___SOKOL_UNUSED(img);
  YK___SOKOL_UNUSED(data);
  return true;
#else
  YK__SOKOL_ASSERT(img && data);
  YK__SOKOL_VALIDATE_BEGIN();
  YK__SOKOL_VALIDATE(img->cmn.usage != YK__SG_USAGE_IMMUTABLE,
                     YK___SG_VALIDATE_UPDIMG_USAGE);
  YK__SOKOL_VALIDATE(img->cmn.upd_frame_index != yk___sg.frame_index,
                     YK___SG_VALIDATE_UPDIMG_ONCE);
  yk___sg_validate_image_data(data, img->cmn.pixel_format, img->cmn.width,
                              img->cmn.height,
                              (img->cmn.type == YK__SG_IMAGETYPE_CUBE) ? 6 : 1,
                              img->cmn.num_mipmaps, img->cmn.num_slices);
  return YK__SOKOL_VALIDATE_END();
#endif
}
/*== fill in desc default values =============================================*/
YK___SOKOL_PRIVATE yk__sg_buffer_desc
yk___sg_buffer_desc_defaults(const yk__sg_buffer_desc *desc) {
  yk__sg_buffer_desc def = *desc;
  def.type = yk___sg_def(def.type, YK__SG_BUFFERTYPE_VERTEXBUFFER);
  def.usage = yk___sg_def(def.usage, YK__SG_USAGE_IMMUTABLE);
  if (def.size == 0) {
    def.size = def.data.size;
  } else if (def.data.size == 0) {
    def.data.size = def.size;
  }
  return def;
}
YK___SOKOL_PRIVATE yk__sg_image_desc
yk___sg_image_desc_defaults(const yk__sg_image_desc *desc) {
  yk__sg_image_desc def = *desc;
  def.type = yk___sg_def(def.type, YK__SG_IMAGETYPE_2D);
  def.num_slices = yk___sg_def(def.num_slices, 1);
  def.num_mipmaps = yk___sg_def(def.num_mipmaps, 1);
  def.usage = yk___sg_def(def.usage, YK__SG_USAGE_IMMUTABLE);
  if (desc->render_target) {
    def.pixel_format =
        yk___sg_def(def.pixel_format, yk___sg.desc.context.color_format);
    def.sample_count =
        yk___sg_def(def.sample_count, yk___sg.desc.context.sample_count);
  } else {
    def.pixel_format = yk___sg_def(def.pixel_format, YK__SG_PIXELFORMAT_RGBA8);
    def.sample_count = yk___sg_def(def.sample_count, 1);
  }
  def.min_filter = yk___sg_def(def.min_filter, YK__SG_FILTER_NEAREST);
  def.mag_filter = yk___sg_def(def.mag_filter, YK__SG_FILTER_NEAREST);
  def.wrap_u = yk___sg_def(def.wrap_u, YK__SG_WRAP_REPEAT);
  def.wrap_v = yk___sg_def(def.wrap_v, YK__SG_WRAP_REPEAT);
  def.wrap_w = yk___sg_def(def.wrap_w, YK__SG_WRAP_REPEAT);
  def.border_color =
      yk___sg_def(def.border_color, YK__SG_BORDERCOLOR_OPAQUE_BLACK);
  def.max_anisotropy = yk___sg_def(def.max_anisotropy, 1);
  def.max_lod = yk___sg_def_flt(def.max_lod, FLT_MAX);
  return def;
}
YK___SOKOL_PRIVATE yk__sg_shader_desc
yk___sg_shader_desc_defaults(const yk__sg_shader_desc *desc) {
  yk__sg_shader_desc def = *desc;
#if defined(YK__SOKOL_METAL)
  def.vs.entry = yk___sg_def(def.vs.entry, "_main");
  def.fs.entry = yk___sg_def(def.fs.entry, "_main");
#else
  def.vs.entry = yk___sg_def(def.vs.entry, "main");
  def.fs.entry = yk___sg_def(def.fs.entry, "main");
#endif
#if defined(YK__SOKOL_D3D11)
  if (def.vs.source) {
    def.vs.d3d11_target = yk___sg_def(def.vs.d3d11_target, "vs_4_0");
  }
  if (def.fs.source) {
    def.fs.d3d11_target = yk___sg_def(def.fs.d3d11_target, "ps_4_0");
  }
#endif
  for (int stage_index = 0; stage_index < YK__SG_NUM_SHADER_STAGES;
       stage_index++) {
    yk__sg_shader_stage_desc *stage_desc =
        (stage_index == YK__SG_SHADERSTAGE_VS) ? &def.vs : &def.fs;
    for (int ub_index = 0; ub_index < YK__SG_MAX_SHADERSTAGE_UBS; ub_index++) {
      yk__sg_shader_uniform_block_desc *ub_desc =
          &stage_desc->uniform_blocks[ub_index];
      if (0 == ub_desc->size) { break; }
      for (int u_index = 0; u_index < YK__SG_MAX_UB_MEMBERS; u_index++) {
        yk__sg_shader_uniform_desc *u_desc = &ub_desc->uniforms[u_index];
        if (u_desc->type == YK__SG_UNIFORMTYPE_INVALID) { break; }
        u_desc->array_count = yk___sg_def(u_desc->array_count, 1);
      }
    }
    for (int img_index = 0; img_index < YK__SG_MAX_SHADERSTAGE_IMAGES;
         img_index++) {
      yk__sg_shader_image_desc *img_desc = &stage_desc->images[img_index];
      if (img_desc->image_type == YK___SG_IMAGETYPE_DEFAULT) { break; }
      img_desc->sampler_type =
          yk___sg_def(img_desc->sampler_type, YK__SG_SAMPLERTYPE_FLOAT);
    }
  }
  return def;
}
YK___SOKOL_PRIVATE yk__sg_pipeline_desc
yk___sg_pipeline_desc_defaults(const yk__sg_pipeline_desc *desc) {
  yk__sg_pipeline_desc def = *desc;
  def.primitive_type =
      yk___sg_def(def.primitive_type, YK__SG_PRIMITIVETYPE_TRIANGLES);
  def.index_type = yk___sg_def(def.index_type, YK__SG_INDEXTYPE_NONE);
  def.cull_mode = yk___sg_def(def.cull_mode, YK__SG_CULLMODE_NONE);
  def.face_winding = yk___sg_def(def.face_winding, YK__SG_FACEWINDING_CW);
  def.sample_count =
      yk___sg_def(def.sample_count, yk___sg.desc.context.sample_count);
  def.stencil.front.compare =
      yk___sg_def(def.stencil.front.compare, YK__SG_COMPAREFUNC_ALWAYS);
  def.stencil.front.fail_op =
      yk___sg_def(def.stencil.front.fail_op, YK__SG_STENCILOP_KEEP);
  def.stencil.front.depth_fail_op =
      yk___sg_def(def.stencil.front.depth_fail_op, YK__SG_STENCILOP_KEEP);
  def.stencil.front.pass_op =
      yk___sg_def(def.stencil.front.pass_op, YK__SG_STENCILOP_KEEP);
  def.stencil.back.compare =
      yk___sg_def(def.stencil.back.compare, YK__SG_COMPAREFUNC_ALWAYS);
  def.stencil.back.fail_op =
      yk___sg_def(def.stencil.back.fail_op, YK__SG_STENCILOP_KEEP);
  def.stencil.back.depth_fail_op =
      yk___sg_def(def.stencil.back.depth_fail_op, YK__SG_STENCILOP_KEEP);
  def.stencil.back.pass_op =
      yk___sg_def(def.stencil.back.pass_op, YK__SG_STENCILOP_KEEP);
  def.depth.compare = yk___sg_def(def.depth.compare, YK__SG_COMPAREFUNC_ALWAYS);
  def.depth.pixel_format =
      yk___sg_def(def.depth.pixel_format, yk___sg.desc.context.depth_format);
  def.color_count = yk___sg_def(def.color_count, 1);
  if (def.color_count > YK__SG_MAX_COLOR_ATTACHMENTS) {
    def.color_count = YK__SG_MAX_COLOR_ATTACHMENTS;
  }
  for (int i = 0; i < def.color_count; i++) {
    yk__sg_color_state *cs = &def.colors[i];
    cs->pixel_format =
        yk___sg_def(cs->pixel_format, yk___sg.desc.context.color_format);
    cs->write_mask = yk___sg_def(cs->write_mask, YK__SG_COLORMASK_RGBA);
    yk__sg_blend_state *bs = &def.colors[i].blend;
    bs->src_factor_rgb =
        yk___sg_def(bs->src_factor_rgb, YK__SG_BLENDFACTOR_ONE);
    bs->dst_factor_rgb =
        yk___sg_def(bs->dst_factor_rgb, YK__SG_BLENDFACTOR_ZERO);
    bs->op_rgb = yk___sg_def(bs->op_rgb, YK__SG_BLENDOP_ADD);
    bs->src_factor_alpha =
        yk___sg_def(bs->src_factor_alpha, YK__SG_BLENDFACTOR_ONE);
    bs->dst_factor_alpha =
        yk___sg_def(bs->dst_factor_alpha, YK__SG_BLENDFACTOR_ZERO);
    bs->op_alpha = yk___sg_def(bs->op_alpha, YK__SG_BLENDOP_ADD);
  }
  for (int attr_index = 0; attr_index < YK__SG_MAX_VERTEX_ATTRIBUTES;
       attr_index++) {
    yk__sg_vertex_attr_desc *a_desc = &def.layout.attrs[attr_index];
    if (a_desc->format == YK__SG_VERTEXFORMAT_INVALID) { break; }
    YK__SOKOL_ASSERT(a_desc->buffer_index < YK__SG_MAX_SHADERSTAGE_BUFFERS);
    yk__sg_buffer_layout_desc *b_desc =
        &def.layout.buffers[a_desc->buffer_index];
    b_desc->step_func =
        yk___sg_def(b_desc->step_func, YK__SG_VERTEXSTEP_PER_VERTEX);
    b_desc->step_rate = yk___sg_def(b_desc->step_rate, 1);
  }
  /* resolve vertex layout strides and offsets */
  int auto_offset[YK__SG_MAX_SHADERSTAGE_BUFFERS];
  memset(auto_offset, 0, sizeof(auto_offset));
  bool use_auto_offset = true;
  for (int attr_index = 0; attr_index < YK__SG_MAX_VERTEX_ATTRIBUTES;
       attr_index++) {
    /* to use computed offsets, *all* attr offsets must be 0 */
    if (def.layout.attrs[attr_index].offset != 0) { use_auto_offset = false; }
  }
  for (int attr_index = 0; attr_index < YK__SG_MAX_VERTEX_ATTRIBUTES;
       attr_index++) {
    yk__sg_vertex_attr_desc *a_desc = &def.layout.attrs[attr_index];
    if (a_desc->format == YK__SG_VERTEXFORMAT_INVALID) { break; }
    YK__SOKOL_ASSERT(a_desc->buffer_index < YK__SG_MAX_SHADERSTAGE_BUFFERS);
    if (use_auto_offset) { a_desc->offset = auto_offset[a_desc->buffer_index]; }
    auto_offset[a_desc->buffer_index] +=
        yk___sg_vertexformat_bytesize(a_desc->format);
  }
  /* compute vertex strides if needed */
  for (int buf_index = 0; buf_index < YK__SG_MAX_SHADERSTAGE_BUFFERS;
       buf_index++) {
    yk__sg_buffer_layout_desc *l_desc = &def.layout.buffers[buf_index];
    if (l_desc->stride == 0) { l_desc->stride = auto_offset[buf_index]; }
  }
  return def;
}
YK___SOKOL_PRIVATE yk__sg_pass_desc
yk___sg_pass_desc_defaults(const yk__sg_pass_desc *desc) {
  /* FIXME: no values to replace in yk__sg_pass_desc? */
  yk__sg_pass_desc def = *desc;
  return def;
}
/*== allocate/initialize resource private functions ==========================*/
YK___SOKOL_PRIVATE yk__sg_buffer yk___sg_alloc_buffer(void) {
  yk__sg_buffer res;
  int slot_index = yk___sg_pool_alloc_index(&yk___sg.pools.buffer_pool);
  if (YK___SG_INVALID_SLOT_INDEX != slot_index) {
    res.id =
        yk___sg_slot_alloc(&yk___sg.pools.buffer_pool,
                           &yk___sg.pools.buffers[slot_index].slot, slot_index);
  } else {
    /* pool is exhausted */
    res.id = YK__SG_INVALID_ID;
  }
  return res;
}
YK___SOKOL_PRIVATE yk__sg_image yk___sg_alloc_image(void) {
  yk__sg_image res;
  int slot_index = yk___sg_pool_alloc_index(&yk___sg.pools.image_pool);
  if (YK___SG_INVALID_SLOT_INDEX != slot_index) {
    res.id =
        yk___sg_slot_alloc(&yk___sg.pools.image_pool,
                           &yk___sg.pools.images[slot_index].slot, slot_index);
  } else {
    /* pool is exhausted */
    res.id = YK__SG_INVALID_ID;
  }
  return res;
}
YK___SOKOL_PRIVATE yk__sg_shader yk___sg_alloc_shader(void) {
  yk__sg_shader res;
  int slot_index = yk___sg_pool_alloc_index(&yk___sg.pools.shader_pool);
  if (YK___SG_INVALID_SLOT_INDEX != slot_index) {
    res.id =
        yk___sg_slot_alloc(&yk___sg.pools.shader_pool,
                           &yk___sg.pools.shaders[slot_index].slot, slot_index);
  } else {
    /* pool is exhausted */
    res.id = YK__SG_INVALID_ID;
  }
  return res;
}
YK___SOKOL_PRIVATE yk__sg_pipeline yk___sg_alloc_pipeline(void) {
  yk__sg_pipeline res;
  int slot_index = yk___sg_pool_alloc_index(&yk___sg.pools.pipeline_pool);
  if (YK___SG_INVALID_SLOT_INDEX != slot_index) {
    res.id = yk___sg_slot_alloc(&yk___sg.pools.pipeline_pool,
                                &yk___sg.pools.pipelines[slot_index].slot,
                                slot_index);
  } else {
    /* pool is exhausted */
    res.id = YK__SG_INVALID_ID;
  }
  return res;
}
YK___SOKOL_PRIVATE yk__sg_pass yk___sg_alloc_pass(void) {
  yk__sg_pass res;
  int slot_index = yk___sg_pool_alloc_index(&yk___sg.pools.pass_pool);
  if (YK___SG_INVALID_SLOT_INDEX != slot_index) {
    res.id =
        yk___sg_slot_alloc(&yk___sg.pools.pass_pool,
                           &yk___sg.pools.passes[slot_index].slot, slot_index);
  } else {
    /* pool is exhausted */
    res.id = YK__SG_INVALID_ID;
  }
  return res;
}
YK___SOKOL_PRIVATE void yk___sg_dealloc_buffer(yk__sg_buffer buf_id) {
  YK__SOKOL_ASSERT(buf_id.id != YK__SG_INVALID_ID);
  yk___sg_buffer_t *buf = yk___sg_lookup_buffer(&yk___sg.pools, buf_id.id);
  YK__SOKOL_ASSERT(buf && buf->slot.state == YK__SG_RESOURCESTATE_ALLOC);
  yk___sg_reset_slot(&buf->slot);
  yk___sg_pool_free_index(&yk___sg.pools.buffer_pool,
                          yk___sg_slot_index(buf_id.id));
}
YK___SOKOL_PRIVATE void yk___sg_dealloc_image(yk__sg_image img_id) {
  YK__SOKOL_ASSERT(img_id.id != YK__SG_INVALID_ID);
  yk___sg_image_t *img = yk___sg_lookup_image(&yk___sg.pools, img_id.id);
  YK__SOKOL_ASSERT(img && img->slot.state == YK__SG_RESOURCESTATE_ALLOC);
  yk___sg_reset_slot(&img->slot);
  yk___sg_pool_free_index(&yk___sg.pools.image_pool,
                          yk___sg_slot_index(img_id.id));
}
YK___SOKOL_PRIVATE void yk___sg_dealloc_shader(yk__sg_shader shd_id) {
  YK__SOKOL_ASSERT(shd_id.id != YK__SG_INVALID_ID);
  yk___sg_shader_t *shd = yk___sg_lookup_shader(&yk___sg.pools, shd_id.id);
  YK__SOKOL_ASSERT(shd && shd->slot.state == YK__SG_RESOURCESTATE_ALLOC);
  yk___sg_reset_slot(&shd->slot);
  yk___sg_pool_free_index(&yk___sg.pools.shader_pool,
                          yk___sg_slot_index(shd_id.id));
}
YK___SOKOL_PRIVATE void yk___sg_dealloc_pipeline(yk__sg_pipeline pip_id) {
  YK__SOKOL_ASSERT(pip_id.id != YK__SG_INVALID_ID);
  yk___sg_pipeline_t *pip = yk___sg_lookup_pipeline(&yk___sg.pools, pip_id.id);
  YK__SOKOL_ASSERT(pip && pip->slot.state == YK__SG_RESOURCESTATE_ALLOC);
  yk___sg_reset_slot(&pip->slot);
  yk___sg_pool_free_index(&yk___sg.pools.pipeline_pool,
                          yk___sg_slot_index(pip_id.id));
}
YK___SOKOL_PRIVATE void yk___sg_dealloc_pass(yk__sg_pass pass_id) {
  YK__SOKOL_ASSERT(pass_id.id != YK__SG_INVALID_ID);
  yk___sg_pass_t *pass = yk___sg_lookup_pass(&yk___sg.pools, pass_id.id);
  YK__SOKOL_ASSERT(pass && pass->slot.state == YK__SG_RESOURCESTATE_ALLOC);
  yk___sg_reset_slot(&pass->slot);
  yk___sg_pool_free_index(&yk___sg.pools.pass_pool,
                          yk___sg_slot_index(pass_id.id));
}
YK___SOKOL_PRIVATE void yk___sg_init_buffer(yk__sg_buffer buf_id,
                                            const yk__sg_buffer_desc *desc) {
  YK__SOKOL_ASSERT(buf_id.id != YK__SG_INVALID_ID && desc);
  yk___sg_buffer_t *buf = yk___sg_lookup_buffer(&yk___sg.pools, buf_id.id);
  YK__SOKOL_ASSERT(buf && buf->slot.state == YK__SG_RESOURCESTATE_ALLOC);
  buf->slot.ctx_id = yk___sg.active_context.id;
  if (yk___sg_validate_buffer_desc(desc)) {
    buf->slot.state = yk___sg_create_buffer(buf, desc);
  } else {
    buf->slot.state = YK__SG_RESOURCESTATE_FAILED;
  }
  YK__SOKOL_ASSERT((buf->slot.state == YK__SG_RESOURCESTATE_VALID) ||
                   (buf->slot.state == YK__SG_RESOURCESTATE_FAILED));
}
YK___SOKOL_PRIVATE void yk___sg_init_image(yk__sg_image img_id,
                                           const yk__sg_image_desc *desc) {
  YK__SOKOL_ASSERT(img_id.id != YK__SG_INVALID_ID && desc);
  yk___sg_image_t *img = yk___sg_lookup_image(&yk___sg.pools, img_id.id);
  YK__SOKOL_ASSERT(img && img->slot.state == YK__SG_RESOURCESTATE_ALLOC);
  img->slot.ctx_id = yk___sg.active_context.id;
  if (yk___sg_validate_image_desc(desc)) {
    img->slot.state = yk___sg_create_image(img, desc);
  } else {
    img->slot.state = YK__SG_RESOURCESTATE_FAILED;
  }
  YK__SOKOL_ASSERT((img->slot.state == YK__SG_RESOURCESTATE_VALID) ||
                   (img->slot.state == YK__SG_RESOURCESTATE_FAILED));
}
YK___SOKOL_PRIVATE void yk___sg_init_shader(yk__sg_shader shd_id,
                                            const yk__sg_shader_desc *desc) {
  YK__SOKOL_ASSERT(shd_id.id != YK__SG_INVALID_ID && desc);
  yk___sg_shader_t *shd = yk___sg_lookup_shader(&yk___sg.pools, shd_id.id);
  YK__SOKOL_ASSERT(shd && shd->slot.state == YK__SG_RESOURCESTATE_ALLOC);
  shd->slot.ctx_id = yk___sg.active_context.id;
  if (yk___sg_validate_shader_desc(desc)) {
    shd->slot.state = yk___sg_create_shader(shd, desc);
  } else {
    shd->slot.state = YK__SG_RESOURCESTATE_FAILED;
  }
  YK__SOKOL_ASSERT((shd->slot.state == YK__SG_RESOURCESTATE_VALID) ||
                   (shd->slot.state == YK__SG_RESOURCESTATE_FAILED));
}
YK___SOKOL_PRIVATE void
yk___sg_init_pipeline(yk__sg_pipeline pip_id,
                      const yk__sg_pipeline_desc *desc) {
  YK__SOKOL_ASSERT(pip_id.id != YK__SG_INVALID_ID && desc);
  yk___sg_pipeline_t *pip = yk___sg_lookup_pipeline(&yk___sg.pools, pip_id.id);
  YK__SOKOL_ASSERT(pip && pip->slot.state == YK__SG_RESOURCESTATE_ALLOC);
  pip->slot.ctx_id = yk___sg.active_context.id;
  if (yk___sg_validate_pipeline_desc(desc)) {
    yk___sg_shader_t *shd =
        yk___sg_lookup_shader(&yk___sg.pools, desc->shader.id);
    if (shd && (shd->slot.state == YK__SG_RESOURCESTATE_VALID)) {
      pip->slot.state = yk___sg_create_pipeline(pip, shd, desc);
    } else {
      pip->slot.state = YK__SG_RESOURCESTATE_FAILED;
    }
  } else {
    pip->slot.state = YK__SG_RESOURCESTATE_FAILED;
  }
  YK__SOKOL_ASSERT((pip->slot.state == YK__SG_RESOURCESTATE_VALID) ||
                   (pip->slot.state == YK__SG_RESOURCESTATE_FAILED));
}
YK___SOKOL_PRIVATE void yk___sg_init_pass(yk__sg_pass pass_id,
                                          const yk__sg_pass_desc *desc) {
  YK__SOKOL_ASSERT(pass_id.id != YK__SG_INVALID_ID && desc);
  yk___sg_pass_t *pass = yk___sg_lookup_pass(&yk___sg.pools, pass_id.id);
  YK__SOKOL_ASSERT(pass && pass->slot.state == YK__SG_RESOURCESTATE_ALLOC);
  pass->slot.ctx_id = yk___sg.active_context.id;
  if (yk___sg_validate_pass_desc(desc)) {
    /* lookup pass attachment image pointers */
    yk___sg_image_t *att_imgs[YK__SG_MAX_COLOR_ATTACHMENTS + 1];
    for (int i = 0; i < YK__SG_MAX_COLOR_ATTACHMENTS; i++) {
      if (desc->color_attachments[i].image.id) {
        att_imgs[i] = yk___sg_lookup_image(&yk___sg.pools,
                                           desc->color_attachments[i].image.id);
        /* FIXME: this shouldn't be an assertion, but result in a YK__SG_RESOURCESTATE_FAILED pass */
        YK__SOKOL_ASSERT(att_imgs[i] &&
                         att_imgs[i]->slot.state == YK__SG_RESOURCESTATE_VALID);
      } else {
        att_imgs[i] = 0;
      }
    }
    const int ds_att_index = YK__SG_MAX_COLOR_ATTACHMENTS;
    if (desc->depth_stencil_attachment.image.id) {
      att_imgs[ds_att_index] = yk___sg_lookup_image(
          &yk___sg.pools, desc->depth_stencil_attachment.image.id);
      /* FIXME: this shouldn't be an assertion, but result in a YK__SG_RESOURCESTATE_FAILED pass */
      YK__SOKOL_ASSERT(att_imgs[ds_att_index] &&
                       att_imgs[ds_att_index]->slot.state ==
                           YK__SG_RESOURCESTATE_VALID);
    } else {
      att_imgs[ds_att_index] = 0;
    }
    pass->slot.state = yk___sg_create_pass(pass, att_imgs, desc);
  } else {
    pass->slot.state = YK__SG_RESOURCESTATE_FAILED;
  }
  YK__SOKOL_ASSERT((pass->slot.state == YK__SG_RESOURCESTATE_VALID) ||
                   (pass->slot.state == YK__SG_RESOURCESTATE_FAILED));
}
YK___SOKOL_PRIVATE bool yk___sg_uninit_buffer(yk__sg_buffer buf_id) {
  yk___sg_buffer_t *buf = yk___sg_lookup_buffer(&yk___sg.pools, buf_id.id);
  if (buf) {
    if (buf->slot.ctx_id == yk___sg.active_context.id) {
      yk___sg_destroy_buffer(buf);
      yk___sg_reset_buffer(buf);
      return true;
    } else {
      YK__SOKOL_LOG("yk___sg_uninit_buffer: active context mismatch (must be "
                    "same as for creation)");
      YK___SG_TRACE_NOARGS(err_context_mismatch);
    }
  }
  return false;
}
YK___SOKOL_PRIVATE bool yk___sg_uninit_image(yk__sg_image img_id) {
  yk___sg_image_t *img = yk___sg_lookup_image(&yk___sg.pools, img_id.id);
  if (img) {
    if (img->slot.ctx_id == yk___sg.active_context.id) {
      yk___sg_destroy_image(img);
      yk___sg_reset_image(img);
      return true;
    } else {
      YK__SOKOL_LOG("yk___sg_uninit_image: active context mismatch (must be "
                    "same as for creation)");
      YK___SG_TRACE_NOARGS(err_context_mismatch);
    }
  }
  return false;
}
YK___SOKOL_PRIVATE bool yk___sg_uninit_shader(yk__sg_shader shd_id) {
  yk___sg_shader_t *shd = yk___sg_lookup_shader(&yk___sg.pools, shd_id.id);
  if (shd) {
    if (shd->slot.ctx_id == yk___sg.active_context.id) {
      yk___sg_destroy_shader(shd);
      yk___sg_reset_shader(shd);
      return true;
    } else {
      YK__SOKOL_LOG("yk___sg_uninit_shader: active context mismatch (must be "
                    "same as for creation)");
      YK___SG_TRACE_NOARGS(err_context_mismatch);
    }
  }
  return false;
}
YK___SOKOL_PRIVATE bool yk___sg_uninit_pipeline(yk__sg_pipeline pip_id) {
  yk___sg_pipeline_t *pip = yk___sg_lookup_pipeline(&yk___sg.pools, pip_id.id);
  if (pip) {
    if (pip->slot.ctx_id == yk___sg.active_context.id) {
      yk___sg_destroy_pipeline(pip);
      yk___sg_reset_pipeline(pip);
      return true;
    } else {
      YK__SOKOL_LOG("yk___sg_uninit_pipeline: active context mismatch (must be "
                    "same as for creation)");
      YK___SG_TRACE_NOARGS(err_context_mismatch);
    }
  }
  return false;
}
YK___SOKOL_PRIVATE bool yk___sg_uninit_pass(yk__sg_pass pass_id) {
  yk___sg_pass_t *pass = yk___sg_lookup_pass(&yk___sg.pools, pass_id.id);
  if (pass) {
    if (pass->slot.ctx_id == yk___sg.active_context.id) {
      yk___sg_destroy_pass(pass);
      yk___sg_reset_pass(pass);
      return true;
    } else {
      YK__SOKOL_LOG("yk___sg_uninit_pass: active context mismatch (must be "
                    "same as for creation)");
      YK___SG_TRACE_NOARGS(err_context_mismatch);
    }
  }
  return false;
}
/*== PUBLIC API FUNCTIONS ====================================================*/
#if defined(YK__SOKOL_METAL)
// this is ARC compatible
#if defined(__cplusplus)
#define YK___SG_CLEAR(type, item)                                              \
  { item = (type){}; }
#else
#define YK___SG_CLEAR(type, item)                                              \
  { item = (type){0}; }
#endif
#else
#define YK___SG_CLEAR(type, item)                                              \
  { memset(&item, 0, sizeof(item)); }
#endif
YK__SOKOL_API_IMPL void yk__sg_setup(const yk__sg_desc *desc) {
  YK__SOKOL_ASSERT(desc);
  YK__SOKOL_ASSERT((desc->_start_canary == 0) && (desc->_end_canary == 0));
  YK___SG_CLEAR(yk___sg_state_t, yk___sg);
  yk___sg.desc = *desc;
/* replace zero-init items with their default values
        NOTE: on WebGPU, the default color pixel format MUST be provided,
        cannot be a default compile-time constant.
    */
#if defined(YK__SOKOL_WGPU)
  YK__SOKOL_ASSERT(YK__SG_PIXELFORMAT_NONE !=
                   yk___sg.desc.context.color_format);
#elif defined(YK__SOKOL_METAL) || defined(YK__SOKOL_D3D11)
  yk___sg.desc.context.color_format =
      yk___sg_def(yk___sg.desc.context.color_format, YK__SG_PIXELFORMAT_BGRA8);
#else
  yk___sg.desc.context.color_format =
      yk___sg_def(yk___sg.desc.context.color_format, YK__SG_PIXELFORMAT_RGBA8);
#endif
  yk___sg.desc.context.depth_format = yk___sg_def(
      yk___sg.desc.context.depth_format, YK__SG_PIXELFORMAT_DEPTH_STENCIL);
  yk___sg.desc.context.sample_count =
      yk___sg_def(yk___sg.desc.context.sample_count, 1);
  yk___sg.desc.buffer_pool_size = yk___sg_def(yk___sg.desc.buffer_pool_size,
                                              YK___SG_DEFAULT_BUFFER_POOL_SIZE);
  yk___sg.desc.image_pool_size = yk___sg_def(yk___sg.desc.image_pool_size,
                                             YK___SG_DEFAULT_IMAGE_POOL_SIZE);
  yk___sg.desc.shader_pool_size = yk___sg_def(yk___sg.desc.shader_pool_size,
                                              YK___SG_DEFAULT_SHADER_POOL_SIZE);
  yk___sg.desc.pipeline_pool_size = yk___sg_def(
      yk___sg.desc.pipeline_pool_size, YK___SG_DEFAULT_PIPELINE_POOL_SIZE);
  yk___sg.desc.pass_pool_size =
      yk___sg_def(yk___sg.desc.pass_pool_size, YK___SG_DEFAULT_PASS_POOL_SIZE);
  yk___sg.desc.context_pool_size = yk___sg_def(
      yk___sg.desc.context_pool_size, YK___SG_DEFAULT_CONTEXT_POOL_SIZE);
  yk___sg.desc.uniform_buffer_size =
      yk___sg_def(yk___sg.desc.uniform_buffer_size, YK___SG_DEFAULT_UB_SIZE);
  yk___sg.desc.staging_buffer_size = yk___sg_def(
      yk___sg.desc.staging_buffer_size, YK___SG_DEFAULT_STAGING_SIZE);
  yk___sg.desc.sampler_cache_size = yk___sg_def(
      yk___sg.desc.sampler_cache_size, YK___SG_DEFAULT_SAMPLER_CACHE_CAPACITY);
  yk___sg_setup_pools(&yk___sg.pools, &yk___sg.desc);
  yk___sg.frame_index = 1;
  yk___sg_setup_backend(&yk___sg.desc);
  yk___sg.valid = true;
  yk__sg_setup_context();
}
YK__SOKOL_API_IMPL void yk__sg_shutdown(void) {
  /* can only delete resources for the currently set context here, if multiple
    contexts are used, the app code must take care of properly releasing them
    (since only the app code can switch between 3D-API contexts)
    */
  if (yk___sg.active_context.id != YK__SG_INVALID_ID) {
    yk___sg_context_t *ctx =
        yk___sg_lookup_context(&yk___sg.pools, yk___sg.active_context.id);
    if (ctx) {
      yk___sg_destroy_all_resources(&yk___sg.pools, yk___sg.active_context.id);
      yk___sg_destroy_context(ctx);
    }
  }
  yk___sg_discard_backend();
  yk___sg_discard_pools(&yk___sg.pools);
  yk___sg.valid = false;
}
YK__SOKOL_API_IMPL bool yk__sg_isvalid(void) { return yk___sg.valid; }
YK__SOKOL_API_IMPL yk__sg_desc yk__sg_query_desc(void) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  return yk___sg.desc;
}
YK__SOKOL_API_IMPL yk__sg_backend yk__sg_query_backend(void) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  return yk___sg.backend;
}
YK__SOKOL_API_IMPL yk__sg_features yk__sg_query_features(void) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  return yk___sg.features;
}
YK__SOKOL_API_IMPL yk__sg_limits yk__sg_query_limits(void) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  return yk___sg.limits;
}
YK__SOKOL_API_IMPL yk__sg_pixelformat_info
yk__sg_query_pixelformat(yk__sg_pixel_format fmt) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  int fmt_index = (int) fmt;
  YK__SOKOL_ASSERT((fmt_index > YK__SG_PIXELFORMAT_NONE) &&
                   (fmt_index < YK___SG_PIXELFORMAT_NUM));
  return yk___sg.formats[fmt_index];
}
YK__SOKOL_API_IMPL yk__sg_context yk__sg_setup_context(void) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk__sg_context res;
  int slot_index = yk___sg_pool_alloc_index(&yk___sg.pools.context_pool);
  if (YK___SG_INVALID_SLOT_INDEX != slot_index) {
    res.id = yk___sg_slot_alloc(&yk___sg.pools.context_pool,
                                &yk___sg.pools.contexts[slot_index].slot,
                                slot_index);
    yk___sg_context_t *ctx = yk___sg_context_at(&yk___sg.pools, res.id);
    ctx->slot.state = yk___sg_create_context(ctx);
    YK__SOKOL_ASSERT(ctx->slot.state == YK__SG_RESOURCESTATE_VALID);
    yk___sg_activate_context(ctx);
  } else {
    /* pool is exhausted */
    res.id = YK__SG_INVALID_ID;
  }
  yk___sg.active_context = res;
  return res;
}
YK__SOKOL_API_IMPL void yk__sg_discard_context(yk__sg_context ctx_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk___sg_destroy_all_resources(&yk___sg.pools, ctx_id.id);
  yk___sg_context_t *ctx = yk___sg_lookup_context(&yk___sg.pools, ctx_id.id);
  if (ctx) {
    yk___sg_destroy_context(ctx);
    yk___sg_reset_context(ctx);
    yk___sg_reset_slot(&ctx->slot);
    yk___sg_pool_free_index(&yk___sg.pools.context_pool,
                            yk___sg_slot_index(ctx_id.id));
  }
  yk___sg.active_context.id = YK__SG_INVALID_ID;
  yk___sg_activate_context(0);
}
YK__SOKOL_API_IMPL void yk__sg_activate_context(yk__sg_context ctx_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk___sg.active_context = ctx_id;
  yk___sg_context_t *ctx = yk___sg_lookup_context(&yk___sg.pools, ctx_id.id);
  /* NOTE: ctx can be 0 here if the context is no longer valid */
  yk___sg_activate_context(ctx);
}
YK__SOKOL_API_IMPL yk__sg_trace_hooks
yk__sg_install_trace_hooks(const yk__sg_trace_hooks *trace_hooks) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK__SOKOL_ASSERT(trace_hooks);
  YK___SOKOL_UNUSED(trace_hooks);
#if defined(YK__SOKOL_TRACE_HOOKS)
  yk__sg_trace_hooks old_hooks = yk___sg.hooks;
  yk___sg.hooks = *trace_hooks;
#else
  static yk__sg_trace_hooks old_hooks;
  YK__SOKOL_LOG("yk__sg_install_trace_hooks() called, but SG_TRACE_HOOKS is "
                "not defined!");
#endif
  return old_hooks;
}
YK__SOKOL_API_IMPL yk__sg_buffer yk__sg_alloc_buffer(void) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk__sg_buffer res = yk___sg_alloc_buffer();
  YK___SG_TRACE_ARGS(alloc_buffer, res);
  return res;
}
YK__SOKOL_API_IMPL yk__sg_image yk__sg_alloc_image(void) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk__sg_image res = yk___sg_alloc_image();
  YK___SG_TRACE_ARGS(alloc_image, res);
  return res;
}
YK__SOKOL_API_IMPL yk__sg_shader yk__sg_alloc_shader(void) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk__sg_shader res = yk___sg_alloc_shader();
  YK___SG_TRACE_ARGS(alloc_shader, res);
  return res;
}
YK__SOKOL_API_IMPL yk__sg_pipeline yk__sg_alloc_pipeline(void) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk__sg_pipeline res = yk___sg_alloc_pipeline();
  YK___SG_TRACE_ARGS(alloc_pipeline, res);
  return res;
}
YK__SOKOL_API_IMPL yk__sg_pass yk__sg_alloc_pass(void) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk__sg_pass res = yk___sg_alloc_pass();
  YK___SG_TRACE_ARGS(alloc_pass, res);
  return res;
}
YK__SOKOL_API_IMPL void yk__sg_dealloc_buffer(yk__sg_buffer buf_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk___sg_dealloc_buffer(buf_id);
  YK___SG_TRACE_ARGS(dealloc_buffer, buf_id);
}
YK__SOKOL_API_IMPL void yk__sg_dealloc_image(yk__sg_image img_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk___sg_dealloc_image(img_id);
  YK___SG_TRACE_ARGS(dealloc_image, img_id);
}
YK__SOKOL_API_IMPL void yk__sg_dealloc_shader(yk__sg_shader shd_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk___sg_dealloc_shader(shd_id);
  YK___SG_TRACE_ARGS(dealloc_shader, shd_id);
}
YK__SOKOL_API_IMPL void yk__sg_dealloc_pipeline(yk__sg_pipeline pip_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk___sg_dealloc_pipeline(pip_id);
  YK___SG_TRACE_ARGS(dealloc_pipeline, pip_id);
}
YK__SOKOL_API_IMPL void yk__sg_dealloc_pass(yk__sg_pass pass_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk___sg_dealloc_pass(pass_id);
  YK___SG_TRACE_ARGS(dealloc_pass, pass_id);
}
YK__SOKOL_API_IMPL void yk__sg_init_buffer(yk__sg_buffer buf_id,
                                           const yk__sg_buffer_desc *desc) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk__sg_buffer_desc desc_def = yk___sg_buffer_desc_defaults(desc);
  yk___sg_init_buffer(buf_id, &desc_def);
  YK___SG_TRACE_ARGS(init_buffer, buf_id, &desc_def);
}
YK__SOKOL_API_IMPL void yk__sg_init_image(yk__sg_image img_id,
                                          const yk__sg_image_desc *desc) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk__sg_image_desc desc_def = yk___sg_image_desc_defaults(desc);
  yk___sg_init_image(img_id, &desc_def);
  YK___SG_TRACE_ARGS(init_image, img_id, &desc_def);
}
YK__SOKOL_API_IMPL void yk__sg_init_shader(yk__sg_shader shd_id,
                                           const yk__sg_shader_desc *desc) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk__sg_shader_desc desc_def = yk___sg_shader_desc_defaults(desc);
  yk___sg_init_shader(shd_id, &desc_def);
  YK___SG_TRACE_ARGS(init_shader, shd_id, &desc_def);
}
YK__SOKOL_API_IMPL void yk__sg_init_pipeline(yk__sg_pipeline pip_id,
                                             const yk__sg_pipeline_desc *desc) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk__sg_pipeline_desc desc_def = yk___sg_pipeline_desc_defaults(desc);
  yk___sg_init_pipeline(pip_id, &desc_def);
  YK___SG_TRACE_ARGS(init_pipeline, pip_id, &desc_def);
}
YK__SOKOL_API_IMPL void yk__sg_init_pass(yk__sg_pass pass_id,
                                         const yk__sg_pass_desc *desc) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk__sg_pass_desc desc_def = yk___sg_pass_desc_defaults(desc);
  yk___sg_init_pass(pass_id, &desc_def);
  YK___SG_TRACE_ARGS(init_pass, pass_id, &desc_def);
}
YK__SOKOL_API_IMPL bool yk__sg_uninit_buffer(yk__sg_buffer buf_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  bool res = yk___sg_uninit_buffer(buf_id);
  YK___SG_TRACE_ARGS(uninit_buffer, buf_id);
  return res;
}
YK__SOKOL_API_IMPL bool yk__sg_uninit_image(yk__sg_image img_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  bool res = yk___sg_uninit_image(img_id);
  YK___SG_TRACE_ARGS(uninit_image, img_id);
  return res;
}
YK__SOKOL_API_IMPL bool yk__sg_uninit_shader(yk__sg_shader shd_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  bool res = yk___sg_uninit_shader(shd_id);
  YK___SG_TRACE_ARGS(uninit_shader, shd_id);
  return res;
}
YK__SOKOL_API_IMPL bool yk__sg_uninit_pipeline(yk__sg_pipeline pip_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  bool res = yk___sg_uninit_pipeline(pip_id);
  YK___SG_TRACE_ARGS(uninit_pipeline, pip_id);
  return res;
}
YK__SOKOL_API_IMPL bool yk__sg_uninit_pass(yk__sg_pass pass_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  bool res = yk___sg_uninit_pass(pass_id);
  YK___SG_TRACE_ARGS(uninit_pass, pass_id);
  return res;
}
/*-- set allocated resource to failed state ----------------------------------*/
YK__SOKOL_API_IMPL void yk__sg_fail_buffer(yk__sg_buffer buf_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK__SOKOL_ASSERT(buf_id.id != YK__SG_INVALID_ID);
  yk___sg_buffer_t *buf = yk___sg_lookup_buffer(&yk___sg.pools, buf_id.id);
  YK__SOKOL_ASSERT(buf && buf->slot.state == YK__SG_RESOURCESTATE_ALLOC);
  buf->slot.ctx_id = yk___sg.active_context.id;
  buf->slot.state = YK__SG_RESOURCESTATE_FAILED;
  YK___SG_TRACE_ARGS(fail_buffer, buf_id);
}
YK__SOKOL_API_IMPL void yk__sg_fail_image(yk__sg_image img_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK__SOKOL_ASSERT(img_id.id != YK__SG_INVALID_ID);
  yk___sg_image_t *img = yk___sg_lookup_image(&yk___sg.pools, img_id.id);
  YK__SOKOL_ASSERT(img && img->slot.state == YK__SG_RESOURCESTATE_ALLOC);
  img->slot.ctx_id = yk___sg.active_context.id;
  img->slot.state = YK__SG_RESOURCESTATE_FAILED;
  YK___SG_TRACE_ARGS(fail_image, img_id);
}
YK__SOKOL_API_IMPL void yk__sg_fail_shader(yk__sg_shader shd_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK__SOKOL_ASSERT(shd_id.id != YK__SG_INVALID_ID);
  yk___sg_shader_t *shd = yk___sg_lookup_shader(&yk___sg.pools, shd_id.id);
  YK__SOKOL_ASSERT(shd && shd->slot.state == YK__SG_RESOURCESTATE_ALLOC);
  shd->slot.ctx_id = yk___sg.active_context.id;
  shd->slot.state = YK__SG_RESOURCESTATE_FAILED;
  YK___SG_TRACE_ARGS(fail_shader, shd_id);
}
YK__SOKOL_API_IMPL void yk__sg_fail_pipeline(yk__sg_pipeline pip_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK__SOKOL_ASSERT(pip_id.id != YK__SG_INVALID_ID);
  yk___sg_pipeline_t *pip = yk___sg_lookup_pipeline(&yk___sg.pools, pip_id.id);
  YK__SOKOL_ASSERT(pip && pip->slot.state == YK__SG_RESOURCESTATE_ALLOC);
  pip->slot.ctx_id = yk___sg.active_context.id;
  pip->slot.state = YK__SG_RESOURCESTATE_FAILED;
  YK___SG_TRACE_ARGS(fail_pipeline, pip_id);
}
YK__SOKOL_API_IMPL void yk__sg_fail_pass(yk__sg_pass pass_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK__SOKOL_ASSERT(pass_id.id != YK__SG_INVALID_ID);
  yk___sg_pass_t *pass = yk___sg_lookup_pass(&yk___sg.pools, pass_id.id);
  YK__SOKOL_ASSERT(pass && pass->slot.state == YK__SG_RESOURCESTATE_ALLOC);
  pass->slot.ctx_id = yk___sg.active_context.id;
  pass->slot.state = YK__SG_RESOURCESTATE_FAILED;
  YK___SG_TRACE_ARGS(fail_pass, pass_id);
}
/*-- get resource state */
YK__SOKOL_API_IMPL yk__sg_resource_state
yk__sg_query_buffer_state(yk__sg_buffer buf_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk___sg_buffer_t *buf = yk___sg_lookup_buffer(&yk___sg.pools, buf_id.id);
  yk__sg_resource_state res =
      buf ? buf->slot.state : YK__SG_RESOURCESTATE_INVALID;
  return res;
}
YK__SOKOL_API_IMPL yk__sg_resource_state
yk__sg_query_image_state(yk__sg_image img_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk___sg_image_t *img = yk___sg_lookup_image(&yk___sg.pools, img_id.id);
  yk__sg_resource_state res =
      img ? img->slot.state : YK__SG_RESOURCESTATE_INVALID;
  return res;
}
YK__SOKOL_API_IMPL yk__sg_resource_state
yk__sg_query_shader_state(yk__sg_shader shd_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk___sg_shader_t *shd = yk___sg_lookup_shader(&yk___sg.pools, shd_id.id);
  yk__sg_resource_state res =
      shd ? shd->slot.state : YK__SG_RESOURCESTATE_INVALID;
  return res;
}
YK__SOKOL_API_IMPL yk__sg_resource_state
yk__sg_query_pipeline_state(yk__sg_pipeline pip_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk___sg_pipeline_t *pip = yk___sg_lookup_pipeline(&yk___sg.pools, pip_id.id);
  yk__sg_resource_state res =
      pip ? pip->slot.state : YK__SG_RESOURCESTATE_INVALID;
  return res;
}
YK__SOKOL_API_IMPL yk__sg_resource_state
yk__sg_query_pass_state(yk__sg_pass pass_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk___sg_pass_t *pass = yk___sg_lookup_pass(&yk___sg.pools, pass_id.id);
  yk__sg_resource_state res =
      pass ? pass->slot.state : YK__SG_RESOURCESTATE_INVALID;
  return res;
}
/*-- allocate and initialize resource ----------------------------------------*/
YK__SOKOL_API_IMPL yk__sg_buffer
yk__sg_make_buffer(const yk__sg_buffer_desc *desc) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK__SOKOL_ASSERT(desc);
  yk__sg_buffer_desc desc_def = yk___sg_buffer_desc_defaults(desc);
  yk__sg_buffer buf_id = yk___sg_alloc_buffer();
  if (buf_id.id != YK__SG_INVALID_ID) {
    yk___sg_init_buffer(buf_id, &desc_def);
  } else {
    YK__SOKOL_LOG("buffer pool exhausted!");
    YK___SG_TRACE_NOARGS(err_buffer_pool_exhausted);
  }
  YK___SG_TRACE_ARGS(make_buffer, &desc_def, buf_id);
  return buf_id;
}
YK__SOKOL_API_IMPL yk__sg_image
yk__sg_make_image(const yk__sg_image_desc *desc) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK__SOKOL_ASSERT(desc);
  yk__sg_image_desc desc_def = yk___sg_image_desc_defaults(desc);
  yk__sg_image img_id = yk___sg_alloc_image();
  if (img_id.id != YK__SG_INVALID_ID) {
    yk___sg_init_image(img_id, &desc_def);
  } else {
    YK__SOKOL_LOG("image pool exhausted!");
    YK___SG_TRACE_NOARGS(err_image_pool_exhausted);
  }
  YK___SG_TRACE_ARGS(make_image, &desc_def, img_id);
  return img_id;
}
YK__SOKOL_API_IMPL yk__sg_shader
yk__sg_make_shader(const yk__sg_shader_desc *desc) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK__SOKOL_ASSERT(desc);
  yk__sg_shader_desc desc_def = yk___sg_shader_desc_defaults(desc);
  yk__sg_shader shd_id = yk___sg_alloc_shader();
  if (shd_id.id != YK__SG_INVALID_ID) {
    yk___sg_init_shader(shd_id, &desc_def);
  } else {
    YK__SOKOL_LOG("shader pool exhausted!");
    YK___SG_TRACE_NOARGS(err_shader_pool_exhausted);
  }
  YK___SG_TRACE_ARGS(make_shader, &desc_def, shd_id);
  return shd_id;
}
YK__SOKOL_API_IMPL yk__sg_pipeline
yk__sg_make_pipeline(const yk__sg_pipeline_desc *desc) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK__SOKOL_ASSERT(desc);
  yk__sg_pipeline_desc desc_def = yk___sg_pipeline_desc_defaults(desc);
  yk__sg_pipeline pip_id = yk___sg_alloc_pipeline();
  if (pip_id.id != YK__SG_INVALID_ID) {
    yk___sg_init_pipeline(pip_id, &desc_def);
  } else {
    YK__SOKOL_LOG("pipeline pool exhausted!");
    YK___SG_TRACE_NOARGS(err_pipeline_pool_exhausted);
  }
  YK___SG_TRACE_ARGS(make_pipeline, &desc_def, pip_id);
  return pip_id;
}
YK__SOKOL_API_IMPL yk__sg_pass yk__sg_make_pass(const yk__sg_pass_desc *desc) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK__SOKOL_ASSERT(desc);
  yk__sg_pass_desc desc_def = yk___sg_pass_desc_defaults(desc);
  yk__sg_pass pass_id = yk___sg_alloc_pass();
  if (pass_id.id != YK__SG_INVALID_ID) {
    yk___sg_init_pass(pass_id, &desc_def);
  } else {
    YK__SOKOL_LOG("pass pool exhausted!");
    YK___SG_TRACE_NOARGS(err_pass_pool_exhausted);
  }
  YK___SG_TRACE_ARGS(make_pass, &desc_def, pass_id);
  return pass_id;
}
/*-- destroy resource --------------------------------------------------------*/
YK__SOKOL_API_IMPL void yk__sg_destroy_buffer(yk__sg_buffer buf_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK___SG_TRACE_ARGS(destroy_buffer, buf_id);
  if (yk___sg_uninit_buffer(buf_id)) { yk___sg_dealloc_buffer(buf_id); }
}
YK__SOKOL_API_IMPL void yk__sg_destroy_image(yk__sg_image img_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK___SG_TRACE_ARGS(destroy_image, img_id);
  if (yk___sg_uninit_image(img_id)) { yk___sg_dealloc_image(img_id); }
}
YK__SOKOL_API_IMPL void yk__sg_destroy_shader(yk__sg_shader shd_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK___SG_TRACE_ARGS(destroy_shader, shd_id);
  if (yk___sg_uninit_shader(shd_id)) { yk___sg_dealloc_shader(shd_id); }
}
YK__SOKOL_API_IMPL void yk__sg_destroy_pipeline(yk__sg_pipeline pip_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK___SG_TRACE_ARGS(destroy_pipeline, pip_id);
  if (yk___sg_uninit_pipeline(pip_id)) { yk___sg_dealloc_pipeline(pip_id); }
}
YK__SOKOL_API_IMPL void yk__sg_destroy_pass(yk__sg_pass pass_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK___SG_TRACE_ARGS(destroy_pass, pass_id);
  if (yk___sg_uninit_pass(pass_id)) { yk___sg_dealloc_pass(pass_id); }
}
YK__SOKOL_API_IMPL void
yk__sg_begin_default_pass(const yk__sg_pass_action *pass_action, int width,
                          int height) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK__SOKOL_ASSERT(pass_action);
  YK__SOKOL_ASSERT((pass_action->_start_canary == 0) &&
                   (pass_action->_end_canary == 0));
  yk__sg_pass_action pa;
  yk___sg_resolve_default_pass_action(pass_action, &pa);
  yk___sg.cur_pass.id = YK__SG_INVALID_ID;
  yk___sg.pass_valid = true;
  yk___sg_begin_pass(0, &pa, width, height);
  YK___SG_TRACE_ARGS(begin_default_pass, pass_action, width, height);
}
YK__SOKOL_API_IMPL void
yk__sg_begin_default_passf(const yk__sg_pass_action *pass_action, float width,
                           float height) {
  yk__sg_begin_default_pass(pass_action, (int) width, (int) height);
}
YK__SOKOL_API_IMPL void
yk__sg_begin_pass(yk__sg_pass pass_id, const yk__sg_pass_action *pass_action) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK__SOKOL_ASSERT(pass_action);
  YK__SOKOL_ASSERT((pass_action->_start_canary == 0) &&
                   (pass_action->_end_canary == 0));
  yk___sg.cur_pass = pass_id;
  yk___sg_pass_t *pass = yk___sg_lookup_pass(&yk___sg.pools, pass_id.id);
  if (pass && yk___sg_validate_begin_pass(pass)) {
    yk___sg.pass_valid = true;
    yk__sg_pass_action pa;
    yk___sg_resolve_default_pass_action(pass_action, &pa);
    const yk___sg_image_t *img = yk___sg_pass_color_image(pass, 0);
    YK__SOKOL_ASSERT(img);
    const int w = img->cmn.width;
    const int h = img->cmn.height;
    yk___sg_begin_pass(pass, &pa, w, h);
    YK___SG_TRACE_ARGS(begin_pass, pass_id, pass_action);
  } else {
    yk___sg.pass_valid = false;
    YK___SG_TRACE_NOARGS(err_pass_invalid);
  }
}
YK__SOKOL_API_IMPL void yk__sg_apply_viewport(int x, int y, int width,
                                              int height,
                                              bool origin_top_left) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  if (!yk___sg.pass_valid) {
    YK___SG_TRACE_NOARGS(err_pass_invalid);
    return;
  }
  yk___sg_apply_viewport(x, y, width, height, origin_top_left);
  YK___SG_TRACE_ARGS(apply_viewport, x, y, width, height, origin_top_left);
}
YK__SOKOL_API_IMPL void yk__sg_apply_viewportf(float x, float y, float width,
                                               float height,
                                               bool origin_top_left) {
  yk__sg_apply_viewport((int) x, (int) y, (int) width, (int) height,
                        origin_top_left);
}
YK__SOKOL_API_IMPL void yk__sg_apply_scissor_rect(int x, int y, int width,
                                                  int height,
                                                  bool origin_top_left) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  if (!yk___sg.pass_valid) {
    YK___SG_TRACE_NOARGS(err_pass_invalid);
    return;
  }
  yk___sg_apply_scissor_rect(x, y, width, height, origin_top_left);
  YK___SG_TRACE_ARGS(apply_scissor_rect, x, y, width, height, origin_top_left);
}
YK__SOKOL_API_IMPL void yk__sg_apply_scissor_rectf(float x, float y,
                                                   float width, float height,
                                                   bool origin_top_left) {
  yk__sg_apply_scissor_rect((int) x, (int) y, (int) width, (int) height,
                            origin_top_left);
}
YK__SOKOL_API_IMPL void yk__sg_apply_pipeline(yk__sg_pipeline pip_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk___sg.bindings_valid = false;
  if (!yk___sg_validate_apply_pipeline(pip_id)) {
    yk___sg.next_draw_valid = false;
    YK___SG_TRACE_NOARGS(err_draw_invalid);
    return;
  }
  if (!yk___sg.pass_valid) {
    YK___SG_TRACE_NOARGS(err_pass_invalid);
    return;
  }
  yk___sg.cur_pipeline = pip_id;
  yk___sg_pipeline_t *pip = yk___sg_lookup_pipeline(&yk___sg.pools, pip_id.id);
  YK__SOKOL_ASSERT(pip);
  yk___sg.next_draw_valid = (YK__SG_RESOURCESTATE_VALID == pip->slot.state);
  YK__SOKOL_ASSERT(pip->shader &&
                   (pip->shader->slot.id == pip->cmn.shader_id.id));
  yk___sg_apply_pipeline(pip);
  YK___SG_TRACE_ARGS(apply_pipeline, pip_id);
}
YK__SOKOL_API_IMPL void yk__sg_apply_bindings(const yk__sg_bindings *bindings) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK__SOKOL_ASSERT(bindings);
  YK__SOKOL_ASSERT((bindings->_start_canary == 0) &&
                   (bindings->_end_canary == 0));
  if (!yk___sg_validate_apply_bindings(bindings)) {
    yk___sg.next_draw_valid = false;
    YK___SG_TRACE_NOARGS(err_draw_invalid);
    return;
  }
  yk___sg.bindings_valid = true;
  yk___sg_pipeline_t *pip =
      yk___sg_lookup_pipeline(&yk___sg.pools, yk___sg.cur_pipeline.id);
  YK__SOKOL_ASSERT(pip);
  yk___sg_buffer_t *vbs[YK__SG_MAX_SHADERSTAGE_BUFFERS] = {0};
  int num_vbs = 0;
  for (int i = 0; i < YK__SG_MAX_SHADERSTAGE_BUFFERS; i++, num_vbs++) {
    if (bindings->vertex_buffers[i].id) {
      vbs[i] =
          yk___sg_lookup_buffer(&yk___sg.pools, bindings->vertex_buffers[i].id);
      YK__SOKOL_ASSERT(vbs[i]);
      yk___sg.next_draw_valid &=
          (YK__SG_RESOURCESTATE_VALID == vbs[i]->slot.state);
      yk___sg.next_draw_valid &= !vbs[i]->cmn.append_overflow;
    } else {
      break;
    }
  }
  yk___sg_buffer_t *ib = 0;
  if (bindings->index_buffer.id) {
    ib = yk___sg_lookup_buffer(&yk___sg.pools, bindings->index_buffer.id);
    YK__SOKOL_ASSERT(ib);
    yk___sg.next_draw_valid &= (YK__SG_RESOURCESTATE_VALID == ib->slot.state);
    yk___sg.next_draw_valid &= !ib->cmn.append_overflow;
  }
  yk___sg_image_t *vs_imgs[YK__SG_MAX_SHADERSTAGE_IMAGES] = {0};
  int num_vs_imgs = 0;
  for (int i = 0; i < YK__SG_MAX_SHADERSTAGE_IMAGES; i++, num_vs_imgs++) {
    if (bindings->vs_images[i].id) {
      vs_imgs[i] =
          yk___sg_lookup_image(&yk___sg.pools, bindings->vs_images[i].id);
      YK__SOKOL_ASSERT(vs_imgs[i]);
      yk___sg.next_draw_valid &=
          (YK__SG_RESOURCESTATE_VALID == vs_imgs[i]->slot.state);
    } else {
      break;
    }
  }
  yk___sg_image_t *fs_imgs[YK__SG_MAX_SHADERSTAGE_IMAGES] = {0};
  int num_fs_imgs = 0;
  for (int i = 0; i < YK__SG_MAX_SHADERSTAGE_IMAGES; i++, num_fs_imgs++) {
    if (bindings->fs_images[i].id) {
      fs_imgs[i] =
          yk___sg_lookup_image(&yk___sg.pools, bindings->fs_images[i].id);
      YK__SOKOL_ASSERT(fs_imgs[i]);
      yk___sg.next_draw_valid &=
          (YK__SG_RESOURCESTATE_VALID == fs_imgs[i]->slot.state);
    } else {
      break;
    }
  }
  if (yk___sg.next_draw_valid) {
    const int *vb_offsets = bindings->vertex_buffer_offsets;
    int ib_offset = bindings->index_buffer_offset;
    yk___sg_apply_bindings(pip, vbs, vb_offsets, num_vbs, ib, ib_offset,
                           vs_imgs, num_vs_imgs, fs_imgs, num_fs_imgs);
    YK___SG_TRACE_ARGS(apply_bindings, bindings);
  } else {
    YK___SG_TRACE_NOARGS(err_draw_invalid);
  }
}
YK__SOKOL_API_IMPL void yk__sg_apply_uniforms(yk__sg_shader_stage stage,
                                              int ub_index,
                                              const yk__sg_range *data) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK__SOKOL_ASSERT((stage == YK__SG_SHADERSTAGE_VS) ||
                   (stage == YK__SG_SHADERSTAGE_FS));
  YK__SOKOL_ASSERT((ub_index >= 0) && (ub_index < YK__SG_MAX_SHADERSTAGE_UBS));
  YK__SOKOL_ASSERT(data && data->ptr && (data->size > 0));
  if (!yk___sg_validate_apply_uniforms(stage, ub_index, data)) {
    yk___sg.next_draw_valid = false;
    YK___SG_TRACE_NOARGS(err_draw_invalid);
    return;
  }
  if (!yk___sg.pass_valid) {
    YK___SG_TRACE_NOARGS(err_pass_invalid);
    return;
  }
  if (!yk___sg.next_draw_valid) {
    YK___SG_TRACE_NOARGS(err_draw_invalid);
    return;
  }
  yk___sg_apply_uniforms(stage, ub_index, data);
  YK___SG_TRACE_ARGS(apply_uniforms, stage, ub_index, data);
}
YK__SOKOL_API_IMPL void yk__sg_draw(int base_element, int num_elements,
                                    int num_instances) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK__SOKOL_ASSERT(base_element >= 0);
  YK__SOKOL_ASSERT(num_elements >= 0);
  YK__SOKOL_ASSERT(num_instances >= 0);
#if defined(YK__SOKOL_DEBUG)
  if (!yk___sg.bindings_valid) {
    YK__SOKOL_LOG("attempting to draw without resource bindings");
  }
#endif
  if (!yk___sg.pass_valid) {
    YK___SG_TRACE_NOARGS(err_pass_invalid);
    return;
  }
  if (!yk___sg.next_draw_valid) {
    YK___SG_TRACE_NOARGS(err_draw_invalid);
    return;
  }
  if (!yk___sg.bindings_valid) {
    YK___SG_TRACE_NOARGS(err_bindings_invalid);
    return;
  }
  /* attempting to draw with zero elements or instances is not technically an
       error, but might be handled as an error in the backend API (e.g. on Metal)
    */
  if ((0 == num_elements) || (0 == num_instances)) {
    YK___SG_TRACE_NOARGS(err_draw_invalid);
    return;
  }
  yk___sg_draw(base_element, num_elements, num_instances);
  YK___SG_TRACE_ARGS(draw, base_element, num_elements, num_instances);
}
YK__SOKOL_API_IMPL void yk__sg_end_pass(void) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  if (!yk___sg.pass_valid) {
    YK___SG_TRACE_NOARGS(err_pass_invalid);
    return;
  }
  yk___sg_end_pass();
  yk___sg.cur_pass.id = YK__SG_INVALID_ID;
  yk___sg.cur_pipeline.id = YK__SG_INVALID_ID;
  yk___sg.pass_valid = false;
  YK___SG_TRACE_NOARGS(end_pass);
}
YK__SOKOL_API_IMPL void yk__sg_commit(void) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk___sg_commit();
  YK___SG_TRACE_NOARGS(commit);
  yk___sg.frame_index++;
}
YK__SOKOL_API_IMPL void yk__sg_reset_state_cache(void) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk___sg_reset_state_cache();
  YK___SG_TRACE_NOARGS(reset_state_cache);
}
YK__SOKOL_API_IMPL void yk__sg_update_buffer(yk__sg_buffer buf_id,
                                             const yk__sg_range *data) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK__SOKOL_ASSERT(data && data->ptr && (data->size > 0));
  yk___sg_buffer_t *buf = yk___sg_lookup_buffer(&yk___sg.pools, buf_id.id);
  if ((data->size > 0) && buf &&
      (buf->slot.state == YK__SG_RESOURCESTATE_VALID)) {
    if (yk___sg_validate_update_buffer(buf, data)) {
      YK__SOKOL_ASSERT(data->size <= (size_t) buf->cmn.size);
      /* only one update allowed per buffer and frame */
      YK__SOKOL_ASSERT(buf->cmn.update_frame_index != yk___sg.frame_index);
      /* update and append on same buffer in same frame not allowed */
      YK__SOKOL_ASSERT(buf->cmn.append_frame_index != yk___sg.frame_index);
      yk___sg_update_buffer(buf, data);
      buf->cmn.update_frame_index = yk___sg.frame_index;
    }
  }
  YK___SG_TRACE_ARGS(update_buffer, buf_id, data);
}
YK__SOKOL_API_IMPL int yk__sg_append_buffer(yk__sg_buffer buf_id,
                                            const yk__sg_range *data) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK__SOKOL_ASSERT(data && data->ptr);
  yk___sg_buffer_t *buf = yk___sg_lookup_buffer(&yk___sg.pools, buf_id.id);
  int result;
  if (buf) {
    /* rewind append cursor in a new frame */
    if (buf->cmn.append_frame_index != yk___sg.frame_index) {
      buf->cmn.append_pos = 0;
      buf->cmn.append_overflow = false;
    }
    if ((buf->cmn.append_pos + yk___sg_roundup((int) data->size, 4)) >
        buf->cmn.size) {
      buf->cmn.append_overflow = true;
    }
    const int start_pos = buf->cmn.append_pos;
    if (buf->slot.state == YK__SG_RESOURCESTATE_VALID) {
      if (yk___sg_validate_append_buffer(buf, data)) {
        if (!buf->cmn.append_overflow && (data->size > 0)) {
          /* update and append on same buffer in same frame not allowed */
          YK__SOKOL_ASSERT(buf->cmn.update_frame_index != yk___sg.frame_index);
          int copied_num_bytes = yk___sg_append_buffer(
              buf, data, buf->cmn.append_frame_index != yk___sg.frame_index);
          buf->cmn.append_pos += copied_num_bytes;
          buf->cmn.append_frame_index = yk___sg.frame_index;
        }
      }
    }
    result = start_pos;
  } else {
    /* FIXME: should we return -1 here? */
    result = 0;
  }
  YK___SG_TRACE_ARGS(append_buffer, buf_id, data, result);
  return result;
}
YK__SOKOL_API_IMPL bool yk__sg_query_buffer_overflow(yk__sg_buffer buf_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk___sg_buffer_t *buf = yk___sg_lookup_buffer(&yk___sg.pools, buf_id.id);
  bool result = buf ? buf->cmn.append_overflow : false;
  return result;
}
YK__SOKOL_API_IMPL void yk__sg_update_image(yk__sg_image img_id,
                                            const yk__sg_image_data *data) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk___sg_image_t *img = yk___sg_lookup_image(&yk___sg.pools, img_id.id);
  if (img && img->slot.state == YK__SG_RESOURCESTATE_VALID) {
    if (yk___sg_validate_update_image(img, data)) {
      YK__SOKOL_ASSERT(img->cmn.upd_frame_index != yk___sg.frame_index);
      yk___sg_update_image(img, data);
      img->cmn.upd_frame_index = yk___sg.frame_index;
    }
  }
  YK___SG_TRACE_ARGS(update_image, img_id, data);
}
YK__SOKOL_API_IMPL void yk__sg_push_debug_group(const char *name) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK__SOKOL_ASSERT(name);
  YK___SOKOL_UNUSED(name);
  YK___SG_TRACE_ARGS(push_debug_group, name);
}
YK__SOKOL_API_IMPL void yk__sg_pop_debug_group(void) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  YK___SG_TRACE_NOARGS(pop_debug_group);
}
YK__SOKOL_API_IMPL yk__sg_buffer_info
yk__sg_query_buffer_info(yk__sg_buffer buf_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk__sg_buffer_info info;
  memset(&info, 0, sizeof(info));
  const yk___sg_buffer_t *buf =
      yk___sg_lookup_buffer(&yk___sg.pools, buf_id.id);
  if (buf) {
    info.slot.state = buf->slot.state;
    info.slot.res_id = buf->slot.id;
    info.slot.ctx_id = buf->slot.ctx_id;
    info.update_frame_index = buf->cmn.update_frame_index;
    info.append_frame_index = buf->cmn.append_frame_index;
    info.append_pos = buf->cmn.append_pos;
    info.append_overflow = buf->cmn.append_overflow;
#if defined(YK__SOKOL_D3D11)
    info.num_slots = 1;
    info.active_slot = 0;
#else
    info.num_slots = buf->cmn.num_slots;
    info.active_slot = buf->cmn.active_slot;
#endif
  }
  return info;
}
YK__SOKOL_API_IMPL yk__sg_image_info
yk__sg_query_image_info(yk__sg_image img_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk__sg_image_info info;
  memset(&info, 0, sizeof(info));
  const yk___sg_image_t *img = yk___sg_lookup_image(&yk___sg.pools, img_id.id);
  if (img) {
    info.slot.state = img->slot.state;
    info.slot.res_id = img->slot.id;
    info.slot.ctx_id = img->slot.ctx_id;
    info.upd_frame_index = img->cmn.upd_frame_index;
#if defined(YK__SOKOL_D3D11)
    info.num_slots = 1;
    info.active_slot = 0;
#else
    info.num_slots = img->cmn.num_slots;
    info.active_slot = img->cmn.active_slot;
#endif
    info.width = img->cmn.width;
    info.height = img->cmn.height;
  }
  return info;
}
YK__SOKOL_API_IMPL yk__sg_shader_info
yk__sg_query_shader_info(yk__sg_shader shd_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk__sg_shader_info info;
  memset(&info, 0, sizeof(info));
  const yk___sg_shader_t *shd =
      yk___sg_lookup_shader(&yk___sg.pools, shd_id.id);
  if (shd) {
    info.slot.state = shd->slot.state;
    info.slot.res_id = shd->slot.id;
    info.slot.ctx_id = shd->slot.ctx_id;
  }
  return info;
}
YK__SOKOL_API_IMPL yk__sg_pipeline_info
yk__sg_query_pipeline_info(yk__sg_pipeline pip_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk__sg_pipeline_info info;
  memset(&info, 0, sizeof(info));
  const yk___sg_pipeline_t *pip =
      yk___sg_lookup_pipeline(&yk___sg.pools, pip_id.id);
  if (pip) {
    info.slot.state = pip->slot.state;
    info.slot.res_id = pip->slot.id;
    info.slot.ctx_id = pip->slot.ctx_id;
  }
  return info;
}
YK__SOKOL_API_IMPL yk__sg_pass_info
yk__sg_query_pass_info(yk__sg_pass pass_id) {
  YK__SOKOL_ASSERT(yk___sg.valid);
  yk__sg_pass_info info;
  memset(&info, 0, sizeof(info));
  const yk___sg_pass_t *pass = yk___sg_lookup_pass(&yk___sg.pools, pass_id.id);
  if (pass) {
    info.slot.state = pass->slot.state;
    info.slot.res_id = pass->slot.id;
    info.slot.ctx_id = pass->slot.ctx_id;
  }
  return info;
}
YK__SOKOL_API_IMPL yk__sg_buffer_desc
yk__sg_query_buffer_defaults(const yk__sg_buffer_desc *desc) {
  YK__SOKOL_ASSERT(yk___sg.valid && desc);
  return yk___sg_buffer_desc_defaults(desc);
}
YK__SOKOL_API_IMPL yk__sg_image_desc
yk__sg_query_image_defaults(const yk__sg_image_desc *desc) {
  YK__SOKOL_ASSERT(yk___sg.valid && desc);
  return yk___sg_image_desc_defaults(desc);
}
YK__SOKOL_API_IMPL yk__sg_shader_desc
yk__sg_query_shader_defaults(const yk__sg_shader_desc *desc) {
  YK__SOKOL_ASSERT(yk___sg.valid && desc);
  return yk___sg_shader_desc_defaults(desc);
}
YK__SOKOL_API_IMPL yk__sg_pipeline_desc
yk__sg_query_pipeline_defaults(const yk__sg_pipeline_desc *desc) {
  YK__SOKOL_ASSERT(yk___sg.valid && desc);
  return yk___sg_pipeline_desc_defaults(desc);
}
YK__SOKOL_API_IMPL yk__sg_pass_desc
yk__sg_query_pass_defaults(const yk__sg_pass_desc *desc) {
  YK__SOKOL_ASSERT(yk___sg.valid && desc);
  return yk___sg_pass_desc_defaults(desc);
}
YK__SOKOL_API_IMPL const void *yk__sg_d3d11_device(void) {
#if defined(YK__SOKOL_D3D11)
  return (const void *) yk___sg.d3d11.dev;
#else
  return 0;
#endif
}
YK__SOKOL_API_IMPL const void *yk__sg_mtl_device(void) {
#if defined(YK__SOKOL_METAL)
  if (nil != yk___sg.mtl.device) {
    return (__bridge const void *) yk___sg.mtl.device;
  } else {
    return 0;
  }
#else
  return 0;
#endif
}
YK__SOKOL_API_IMPL const void *yk__sg_mtl_render_command_encoder(void) {
#if defined(YK__SOKOL_METAL)
  if (nil != yk___sg.mtl.cmd_encoder) {
    return (__bridge const void *) yk___sg.mtl.cmd_encoder;
  } else {
    return 0;
  }
#else
  return 0;
#endif
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif /* YK__SOKOL_GFX_IMPL */
