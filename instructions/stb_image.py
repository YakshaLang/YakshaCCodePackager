use_source("stb")
ids = extract_ids("stb_image.h", check_cids=False)
sw = "stb, stbi".split(", ")
def startswith(x):
    for s in sw:
        if x.lower().startswith(s):
            return True
    return False
xx = [x for x in ids if startswith(x)]
P = [x for x in xx if is_lower(x)]
PU = [x for x in xx if not is_lower(x)]
prefix("stb_image.h", PREFIX, P)
prefix("stb_image.h", PREFIX_U, PU)
copy_file("stb_image.h", "yk__stb_image.h", is_temp=False)
clang_format("yk__stb_image.h", is_temp=False)
