use_source("1st")
# ------ bhalib --------
ids = extract_ids("bhalib.h")
P = [x for x in ids if x.startswith("bhalib")]
PU = [x for x in ids if x.startswith("BHALIB")]
prefix("bhalib.h", PREFIX, P)
prefix("bhalib.h", PREFIX_U, PU)
copy_file("bhalib.h", "yk__bhalib.h", is_temp=False)
clang_format("yk__bhalib.h", is_temp=False)
# ------ bhasknk --------
ids = extract_ids("bhasknk.h")
sw = "snk, sg, sapp, sokol, nk, bha".split(", ")
def startswith(x):
    for s in sw:
        if x.lower().startswith(s):
            return True
    return False
xx = [x for x in ids if startswith(x)]
P = [x for x in xx if is_lower(x)]
PU = [x for x in xx if not is_lower(x)]
prefix("bhasknk.h", PREFIX, P)
prefix("bhasknk.h", PREFIX_U, PU)
rename("bhasknk.h", [["#include \"", "#include \"yk__"]])
copy_file("bhasknk.h", "yk__bhasknk.h", is_temp=False)
clang_format("yk__bhasknk.h", is_temp=False)
# -- yaksha runtime library --
import os
import shutil
use_output()
shutil.copy(os.path.join(LOCATION, "libs", "1st", "yk__lib.h"), os.path.join(TEMP, "yk__lib.h"))
shutil.copy(os.path.join(LOCATION, "output", "yk__stb_ds.h"), os.path.join(TEMP, "yk__stb_ds.h"))
# This patch applies to yk__stb_ds
# This allow to use the strdup method of stb_ds with sds by delegating to features of sds :) cool ha!
patch("yk__stb_ds.patch")
copy_file("yk__stb_ds.h", "yk__stb_ds_patched.h", is_temp=True)
apply_includes("--remove-prefix . -I. yk__lib.h".split(" "))
copy_file("yk__lib.h", "yk__lib.h", is_temp=False)
clang_format("yk__lib.h", is_temp=False)
