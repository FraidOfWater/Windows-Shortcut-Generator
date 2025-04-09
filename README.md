# Windows-Shortcut-Generator
This is a command-line-utility to allow shortcuts to be generated ignoring path limits and supporting unicode characters.
Images generate their thumbnails correctly. I couldn't find a good python library to do this correctly. There is way or two to create shortcuts with python, but they all fail when using long paths (255+), where others fail to parse unicode.

Usage:

All arguments must be enclosed with "".
```
python gen_shortcut.py "full_path_to_file" "directory_to_save_shortcut_in" "shortcut_name"
```
Example:
```
python gen_shortcut.py "C:\test.png" "C:\folder\" "test2.png"
```
The py file acts as the command-line-interface. The .exe file does the work. The .cpp file is provided for code viewing.
You can build the .cpp by installing visual studio (not code) and c++ build tools from there. Then you build by navigating to the .cpp script and using (in x64 Native Tools Command Terminal):
```
cl CreateShortcut.cpp ole32.lib shell32.lib
```
