# potential-octo-invention
> Currently, this is just a placeholder name for the project
#### What does this do?
* Convert C libraries to header only libraries with prefixed names/identifiers/macros.
#### How it works?
* Provides a small python DSL to modify C code.
* DSL Functions
  * `use_source(path: str)` - use this source directory (copy it to temp) and chdir
  * `patch(patch_filename: str)`  - apply a .patch file in current temp directory
  * `rename(filename: str, renames: Iterable[Tuple[str, str]])` - perform given regex renames
  * `remove_comments(filename: str)` - remove comments from given file
  * `prefix(filename: str, prefix_: str, renames: Iterable[str])` - rename given identifiers with prefix
  * `preprocess(filename: str, target: str, is_temp=True, args=("-M",))` - run preprocessor
  * `extract_ids(filename: str) -> Set[str]` - extract all identifiers (non keyword and larger than 1 char)
  * `pack(intro_files: str = "", macro: str = None, private: str = "",
    public: str = "", target: str = None, is_temp=True)` - package files to a single header file
  * `PREFIX` default prefix
  * `PREFIX_U` default prefix upper case
* 3rd party dependencies
  * See in 3rd folder.
  * I needed to patch `fcpp` with `fcpp.patch` so it worked for me in Windows.
    * This needs to be compiled
  * `libs` - Apoorva Joshi's single header packer (used in pack function above)
  * `python-patch` - techtonik's patch script
* Tools
  * `cids` - extract c identifiers (ignores preprocessor)
    * Ensure this is compiled first before you run `packer.py`
    * This is used by `extract_ids`
* Main Program
  * `packer.py` this executes all packing scripts in instructions folder and create single file header files.