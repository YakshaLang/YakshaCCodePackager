use_source("1st")
ids = extract_ids("bhalib.h")
P = [x for x in ids if x.startswith("bhalib")]
PU = [x for x in ids if x.startswith("BHALIB")]
prefix("bhalib.h", PREFIX, P)
prefix("bhalib.h", PREFIX_U, PU)
copy_file("bhalib.h", "yk__bhalib.h", is_temp=False)
clang_format("yk__bhalib.h", is_temp=False)
