if DO_BIG_JOBS:
    use_source("Nuklear")
    ids = extract_ids("nuklear.h")
    xx = set([x for x in ids if "stb" in x.lower() or x.startswith("nk") or x.startswith("NK")])
    P = [x for x in xx if is_lower(x)]
    PU = [x for x in xx if not is_lower(x)]
    prefix("nuklear.h", PREFIX, P)
    prefix("nuklear.h", PREFIX_U, PU)
    copy_file("nuklear.h", PREFIX + "nuklear.h", is_temp=False)
    clang_format(PREFIX + "nuklear.h", is_temp=False)
