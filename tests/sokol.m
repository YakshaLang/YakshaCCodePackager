#define YK__SOKOL_IMPL
#if defined(_WIN32)
#define YK__SOKOL_LOG(s) OutputDebugStringA(s)
#endif
/* this is only needed for the debug-inspection headers */
#define YK__SOKOL_TRACE_HOOKS
/* sokol 3D-API defines are provided by build options */
#include "yk__sokol_app.h"
#include "yk__sokol_gfx.h"
#include "yk__sokol_glue.h"