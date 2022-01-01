"""
Tool to Package C libraries as single header files, patch & do simple replaces
"""
import builtins
import glob
import os
import re
import shutil
import subprocess
import sys
from typing import Iterable, Tuple, Set, List

import inctree as _inctree

PYTHON_EXE = sys.executable
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PACKER = os.path.abspath(os.path.join(SCRIPT_DIR, "3rd/libs/scripts/single_header_packer.py"))
PATCHER = os.path.abspath(os.path.join(SCRIPT_DIR, "3rd/python-patch/patch.py"))
PREPROCESS = os.path.abspath(os.path.join(SCRIPT_DIR, "bin/fcpp"))
ID_EXTRACTOR = os.path.abspath(os.path.join(SCRIPT_DIR, "bin/cids"))
INSTRUCTIONS = os.path.join(SCRIPT_DIR, "instructions")
OUTPUT_DIR = os.path.join(SCRIPT_DIR, "output")
LIBS = os.path.join(SCRIPT_DIR, "libs")
TEMP = os.path.join(SCRIPT_DIR, "temp", "delete_me")
G_CURRENT_PACKAGE = "unknown"
# These are default prefixes for packages single header libs
DEFAULT_PREFIX_U = "YK__"
DEFAULT_PREFIX = "yk__"
KEYWORDS = set(
    "auto|break|case|char|const|continue|default|do|double|else|enum|extern|float|for|goto|if|inline|int|long|"
    "register|restrict|return|short|signed|sizeof|static|struct|switch|typedef|union|"
    "unsigned|void|volatile|while|_Alignas|_Alignof|_Atomic|_Bool|_Complex|_Decimal128|_Decimal32|"
    "_Decimal64|_Generic|_Imaginary|_Noreturn|_Static_assert|_Thread_local|NULL|TRUE|FALSE".split("|")
)
REGEX_IDENTIFIER = re.compile(r"[_a-zA-Z][_a-zA-Z0-9]*")
PREPROC = "#if, #ifdef, #ifndef, #else, #elif, #elifdef, #elifndef, #endif, #define, #undef".split(", ")


def use_source(path: str):
    global G_CURRENT_PACKAGE
    G_CURRENT_PACKAGE = os.path.basename(path)
    try:
        shutil.rmtree(TEMP)
        os.rmdir(TEMP)
    except OSError:
        pass
    shutil.copytree(os.path.join(LIBS, path), TEMP, dirs_exist_ok=True)
    os.chdir(TEMP)


def patch(patch_filename: str):
    subprocess.run([PYTHON_EXE, PATCHER, os.path.join(INSTRUCTIONS, patch_filename)],
                   stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, check=True)


def rename(filename: str, renames: Iterable[Tuple[str, str]]):
    with open(filename, "r+", encoding="utf-8") as h:
        data = h.read()
    for f, r in renames:
        from_ = re.compile(f)
        data = from_.sub(r, data)
    with open(filename, "w+", encoding="utf-8") as h:
        h.write(data)


# Reference: https://stackoverflow.com/a/241506
def remove_comments_(text: str, count: int) -> str:
    def replacer(match):
        s = match.group(0)
        if s.startswith('/'):
            return " "  # note: a space and not an empty string
        else:
            return s

    pattern = re.compile(
        r'//.*?$|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
        re.DOTALL | re.MULTILINE
    )
    if count >= 1:
        return re.subn(pattern, replacer, text, count=count)[0]
    return re.sub(pattern, replacer, text)


def remove_comments(filename: str, count: int = -1):
    with open(filename, "r+", encoding="utf-8") as h:
        data = h.read()
    data = remove_comments_(data, count)
    with open(filename, "w+", encoding="utf-8") as h:
        h.write(data)


def prefix(filename: str, prefix_: str, renames: Iterable[str]):
    rename(filename, (("\\b" + re.escape(x) + "\\b", re.escape(prefix_ + x)) for x in renames))


def copy_file(filename: str, target_name: str, is_temp=True):
    f = os.path.join(TEMP, filename)
    if is_temp:
        t = os.path.join(TEMP, target_name)
    else:
        t = os.path.join(OUTPUT_DIR, target_name)
    shutil.copy(f, t)


def preprocess(filename: str, target: str, is_temp=True, args=("-M",)):
    arguments = [PREPROCESS] + list(args) + [os.path.join(TEMP, filename)]
    data = subprocess.check_output(arguments, encoding="utf-8", universal_newlines=True)
    out = os.path.join(OUTPUT_DIR, target)
    if is_temp:
        out = os.path.join(TEMP, target)
    with open(out, "w+", encoding="utf-8") as h:
        h.write(data)


def extract_ids(filename: str) -> Set[str]:
    to_read = filename  # os.path.join(TEMP, filename)
    defines = []
    with open(to_read, "r+", encoding="utf-8") as h:
        code = h.readlines()
        for line in code:
            ls = line.strip()
            for p in PREPROC:
                if ls.startswith(p):
                    ls = ls[len(p):]
                    break
            else:
                continue
            for ident in REGEX_IDENTIFIER.findall(ls):
                defines.append(ident)
    arguments = [ID_EXTRACTOR, to_read]
    data: str = subprocess.check_output(arguments, encoding="utf-8", universal_newlines=True)
    ids = data.splitlines(keepends=False) + defines
    ids = [x.strip() for x in ids]
    ids = [x for x in ids if x and len(x) > 1]
    return set(ids) - KEYWORDS


def scan_code(args: List[str]) -> List[str]:
    return _inctree.scan(args)


def apply_includes(args: List[str]) -> List[str]:
    return _inctree.incs(args)


def pack(intro_files: str = "", macro: str = None, private: str = "",
         public: str = "", target: str = None, is_temp=True):
    m = macro
    if not m:
        m = DEFAULT_PREFIX_U + G_CURRENT_PACKAGE.upper()
    arguments = [PYTHON_EXE, PACKER, "--macro", m]
    if intro_files:
        arguments.append("--intro")
        arguments.append(intro_files)
    if private:
        arguments.append("--priv")
        arguments.append(private)
    if public:
        arguments.append("--pub")
        arguments.append(public)
    data = subprocess.check_output(arguments, encoding="utf-8", universal_newlines=True)
    f = target
    if not f:
        f = G_CURRENT_PACKAGE + ".h"
    if is_temp:
        f = os.path.join(TEMP, f)
    else:
        f = os.path.join(OUTPUT_DIR, f)
    with open(f, "w+", encoding="utf-8") as h:
        h.write(data)


def clang_format(filename: str, is_temp=True):
    if is_temp:
        f = os.path.join(TEMP, filename)
    else:
        f = os.path.join(OUTPUT_DIR, filename)
    arguments = ["clang-format", "-style=file", "-i", f]
    subprocess.run(arguments,
                   stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, check=True)


GLOBAL_DICT = {
    "__builtins__": builtins,
    "use_source": use_source,
    "patch": patch,
    "rename": rename,
    "pack": pack,
    "prefix": prefix,
    "preprocess": preprocess,
    "extract_ids": extract_ids,
    "remove_comments": remove_comments,
    "copy_file": copy_file,
    "clang_format": clang_format,
    "scan_code": scan_code,
    "apply_includes": apply_includes,
    "PREFIX": DEFAULT_PREFIX,
    "PREFIX_U": DEFAULT_PREFIX_U,
}


def main():
    for instruction in glob.glob(INSTRUCTIONS + "/*.py"):
        with open(instruction, "r+", encoding="utf-8") as h:
            print("executing ->", os.path.basename(instruction))
            exec(h.read(), GLOBAL_DICT)


if __name__ == "__main__":
    main()
