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
