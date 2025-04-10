# Windows-Shortcut-Generator (Longer paths, Unicode)
Command-line / python importable module to create windows shortcuts succesfully. Supports unicode in names/paths and longer paths.

I couldn't find a good python library to do this. All the existing ones fail somewhere (no support for longer paths, unicode), so this exists now.

## Why ##
I couldn't find a reliable way to create shortcuts. 
Either they would break because of strange path-limits or they'd wouldn't even support unicode characters.
I noticed if I manually used the context menu to "Create shortcut" on an image, it would just do it, every time, even with ridiculously long filepaths and filenames.
None of the libraries in python could do this, but it was obvious it was possible. This should work.
It uses C++ libraries to interact with windows, but uses python as an interface. Mostly used: "https://www.codeproject.com/articles/11467/how-to-create-short-cuts-link-files".
### Features ###
1. Ignore path limits.
2. Embrace unicode.
3. Does generate working thumbnails!
4. Command-line `python gen_shortcut.py "full_path_to_file" "directory_to_save_shortcut_in" "shortcut_name"`
6. Python module `import gen_shortcut`

# Usage #
Paths passed should not include \\?\ long path formats.
```
python gen_shortcut.py [filepath] [Optional: Shortcut folder] [Optional: Shortcut name]
```
* Default folder is DESKTOP, default name is the filename.
* Enclose filepaths with spaces in brackets "".
* You may use relative paths, too.
```
python gen_shortcut.py "C:\test.png" "C:\folder\" test2.png
```
For batch processing in python,
you can pass a list of tuples as an argument:

```
import gen_shortcut
arguments = [(filepath, shortcut_path), (filepath2, shortcut_path2)]
gen_shortcut.create_shortcut(arguments, info=True, silent=False)
```
* verbose = whether to print verbose.
* silent = whether to ignore errors.
* ex. [(os.path.realpath(r"C:\test.png"), os.path.realpath(r"C:\shortcuts\test.png")]
## Building and editing ##
1. You may edit the .cpp file, which is used to compile the .exe (C++ code)
2. Build .cpp using "x64 Native Tools Command Terminal" from visual studio.
```
cl CreateShortcut.cpp ole32.lib shell32.lib
```

Tips: 
* If you are checking the existence of a shortcut with python, remember to add the .lnk file extension at the end.
