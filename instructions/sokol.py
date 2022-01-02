if DO_BIG_JOBS:
    import os
    use_source("sokol")
    files = ["sokol_app.h", "sokol_gfx.h", "util\sokol_nuklear.h", "sokol_glue.h"]
    ids: set = extract_ids("sokol_app.h") | extract_ids("sokol_gfx.h") | \
          extract_ids("util\sokol_nuklear.h") | extract_ids("sokol_glue.h")
    ids.remove("MSG") # needed for windows.h
    xx = set([x for x in ids if "sapp" in x.lower() or "sokol" in x.lower()
              or "sg" in x.lower() or "snk" in x.lower() or x.startswith("nk") or x.startswith("NK")])
    P = [x for x in xx if is_lower(x)]
    PU = [x for x in xx if not is_lower(x)]
    for f in files:
        prefix(f, PREFIX, P)
        prefix(f, PREFIX_U, PU)
        name = PREFIX + os.path.basename(f)
        copy_file(f, name, is_temp=False)
        clang_format(name, is_temp=False)
