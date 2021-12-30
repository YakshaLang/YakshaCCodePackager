use_source("stb")
ids = extract_ids("stb_ds.h")
P = [x for x in ids if x.startswith("stbds") or x.startswith("arr") or x.startswith("sh")
     or x.startswith("hm") or x.startswith("stb")]
PU = [x for x in ids if x.startswith("STBDS") or x.startswith("STB")] + ["INCLUDE_STB_DS_H"]
prefix("stb_ds.h", PREFIX, P)
prefix("stb_ds.h", PREFIX_U, PU)
REPLACE_FIXES = [[r"http://nothings\\.org/yk__stb_ds", "http://nothings.org/stb_ds"],
                 ["stb_ds\\.h", "yk__stb_ds.h"]]
rename("stb_ds.h", REPLACE_FIXES)
copy_file("stb_ds.h", "yk__stb_ds.h", is_temp=False)
