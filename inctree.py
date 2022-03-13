"""
inctree - scan given code files and dirs and derive a tree of #include-s
print it in topological sort
"""
import argparse
import glob
import os
import re
import sys
from typing import List, Union

ROOT = "<<root>>"
NOT_FOUND = "<<not found>>"
MODE_DBL = 1
MODE_ANGLE = 2
MODE_NONE = 0


# Reference: https://stackoverflow.com/a/18234680
def remove_comments_cpp(text):
    def blot_out_non_newlines(str_in):  # Return a string containing only the newline chars contained in strIn
        return "" + ("\n" * str_in.count('\n'))

    def replacer(match):
        s = match.group(0)
        if s.startswith('/'):  # Matched string is //...EOL or /*...*/  ==> Blot out all non-newline chars
            return blot_out_non_newlines(s)
        else:  # Matched string is '...' or "..."  ==> Keep unchanged
            return s

    pattern = re.compile(
        r'//.*?$|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
        re.DOTALL | re.MULTILINE
    )

    return re.sub(pattern, replacer, text)


def extract_include_filename(ls: str) -> (int, str):
    t = ls[len("#include"):].strip()
    mode = MODE_NONE
    chars = []
    for c in t:
        if mode == MODE_NONE:
            if c == "\"":
                mode = MODE_DBL
            elif c == "<":
                mode = MODE_ANGLE
            else:
                raise ValueError("invalid string ->" + repr(ls))
        elif (mode == MODE_DBL and c == "\"") or (mode == MODE_ANGLE and c == ">"):
            break
        else:
            chars.append(c)
    f = "".join(chars)
    if not f:
        raise ValueError("invalid string ->" + repr(ls))
    return mode, f


class Tree:
    def __init__(self, include_dirs: List[str], sources: List[str]):
        self.tree = []
        self.include_dirs = include_dirs
        self.sources = sources
        self.not_found = []
        self._cache = {}

    def scan(self):
        sub = []
        for s in self.sources:
            sub.append([-1, os.path.abspath(s), self.do_file(s)])
        self.tree.append([-1, ROOT, sub])

    def perform_includes(self):
        for fname in self.sources:
            found, f = self.find(fname)
            if not found:
                continue
            mp = {}
            lines = []
            with open(f, "r+", encoding="utf-8") as h:
                x = remove_comments_cpp(h.read())
            for number, line in enumerate(x.splitlines()):
                ls = line.strip()
                if not ls.startswith("#include"):
                    continue
                m, fname = extract_include_filename(ls)
                if m != MODE_ANGLE:
                    mp[number] = fname
            with open(f, "r+", encoding="utf-8") as h:
                for number, line in enumerate(h.read().splitlines()):
                    if number in mp:
                        lines += self.get_lines(line, mp[number])
                    else:
                        lines.append(line)
            lines = [x.rstrip() for x in lines]
            with open(f, "w+", encoding="utf-8") as h:
                h.write("\n".join(lines))

    def find(self, f: str) -> (bool, str):
        # cur dir
        if os.path.isfile(f):
            return True, os.path.abspath(f)
        for inc in self.include_dirs:
            x = os.path.join(inc, f)
            if os.path.isfile(x):
                return True, os.path.abspath(x)
        return False, f

    def do_file(self, filename: str) -> Union[list, str]:
        found, f = self.find(filename)
        if not found:
            return NOT_FOUND
        if f in self._cache:
            return self._cache[f]
        nodes = []
        with open(f, "r+", encoding="utf-8") as h:
            x = remove_comments_cpp(h.read())
            for number, line in enumerate(x.splitlines()):
                ls = line.strip()
                if not ls.startswith("#include"):
                    continue
                m, fname = extract_include_filename(ls)
                if m != MODE_ANGLE:
                    _, sf = self.find(fname)
                    nodes.append([number, sf, self.do_file(fname)])
        return nodes

    def get_lines(self, line: str, fname: str) -> List[str]:
        found, f = self.find(fname)
        if not found:
            return [line]
        with open(f, "r+", encoding="utf-8") as h:
            return h.read().splitlines()


class TopologicalSort:
    def __init__(self, tree: Tree):
        self.tree = tree
        self.node_set = set()
        self.node_map = {}
        self._pm = set()
        self._tm = {}
        self.sorted = []

    def sort(self):
        self._start_get_nodes()
        self._pm = set(self.node_set)  # remove from this when permanently marked
        self._tm = {x: False for x in self.node_set}
        while True:
            if not self._pm:
                break
            for n in self.node_set:
                self._visit(n)
        self.sorted = [x for x in self.sorted if os.path.isfile(x)]

    def remove_prefix(self, f: str):
        p = os.path.abspath(f)
        self.sorted = [os.path.abspath(x) for x in self.sorted]
        self.sorted = [os.path.relpath(x, p) for x in self.sorted]

    def _visit(self, node):
        if node not in self._pm:  # done
            return
        if self._tm[node]:
            raise ValueError("not a DAG")

        self._tm[node] = True
        try:
            for dep in self.node_map[node]:
                self._visit(dep)
        except KeyError:
            pass

        self._tm[node] = False
        self._pm.remove(node)
        self.sorted.append(node)

    def _start_get_nodes(self):
        for x in self.tree.tree[0][2]:
            self._get_nodes(x)

    def _get_nodes(self, node):
        _, f, children = node
        self.node_set.add(f)
        if children == NOT_FOUND:
            return
        self.node_map[f] = set()
        for x in children:
            if x[2] != NOT_FOUND:
                self.node_map[f].add(x[1])
            self._get_nodes(x)


def parse_arguments(argv):
    parser = argparse.ArgumentParser("inctree.py", description="include tree calculator")
    parser.add_argument("-I", type=str, nargs="*", action='append')
    parser.add_argument("files", type=str, nargs="+")
    parser.add_argument("--remove-prefix", type=str, default=None, dest="rp")
    p = parser.parse_args(argv)
    return p


def scan(argv: List[str]) -> List[str]:
    p = parse_arguments(argv)
    a = get_code_tree(p)
    a.scan()
    t = TopologicalSort(a)
    t.sort()
    if p.rp:
        t.remove_prefix(p.rp)
    return t.sorted


def get_code_tree(p):
    files = []
    for f in p.files:
        if '*' in f or '?' in f:
            files += list(glob.glob(f))
        else:
            files.append(f)
    files = sorted(files)
    a = Tree([x[0] for x in p.I], files)
    return a


def incs(argv: List[str]):
    p = parse_arguments(argv)
    a = get_code_tree(p)
    a.perform_includes()


def main():
    print(scan(sys.argv))


if __name__ == "__main__":
    main()
