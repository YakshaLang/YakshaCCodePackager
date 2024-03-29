use_source("sds")
pack(public="sds.h", private="sdsalloc.h,sds.c", target="sds_temp.h")
ids = extract_ids("sds_temp.h")
P = [x for x in ids if x.startswith("sds") or x.startswith("s_")]
PU = [x for x in ids if x.startswith("SDS")] + ["__SDS_H"]
prefix("sds.h", PREFIX, P)
prefix("sdsalloc.h", PREFIX, P)
prefix("sds.c", PREFIX, P)
prefix("sds.h", PREFIX_U, PU)
prefix("sdsalloc.h", PREFIX_U, PU)
prefix("sds.c", PREFIX_U, PU)
FILENAME_FIXES = [["yk__sds\\.h", "sds.h"], ["yk__sdsalloc\\.h", "sdsalloc.h"]]
rename("sds.c", FILENAME_FIXES)
rename("sds.h", FILENAME_FIXES)
rename("sdsalloc.h", FILENAME_FIXES)
remove_comments("sds.c", 1)
remove_comments("sds.h", 1)
remove_comments("sdsalloc.h", 1)
pack(intro_files="LICENSE", public="sds.h", private="sdsalloc.h,sds.c", target="yk__sds.h", is_temp=True)
MSVC_FIX = """
#if _MSC_VER && !__INTEL_COMPILER
#define __attribute__(X)
#define ssize_t intmax_t
#endif
"""
rename("yk__sds.h", [["#define YK____SDS_H", "#define YK____SDS_H\n" + MSVC_FIX]])
copy_file("yk__sds.h", "yk__sds.h", is_temp=False)
clang_format("yk__sds.h", is_temp=False)
