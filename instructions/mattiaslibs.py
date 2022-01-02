import re
use_source("libs")
for lib in ["ini", "thread", "http"]:
    ids = extract_ids(lib + ".h")
    P = [x for x in ids if x.startswith(lib)]
    PU = [x for x in ids if x.startswith(lib.upper())]
    prefix(lib + ".h", PREFIX, P)
    prefix(lib + ".h", PREFIX_U, PU)
    rename(lib + ".h", [[re.escape(r'yk__http://'), 'http://'],
                        ["YK__THREAD_PRIORITY_HIGHEST", "THREAD_PRIORITY_HIGHEST"]])
    copy_file(lib + ".h", PREFIX + lib + ".h", is_temp=False)
    clang_format(PREFIX + lib + ".h", is_temp=False)
