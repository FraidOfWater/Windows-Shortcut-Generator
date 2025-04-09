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
If you need to create shortcuts programmatically in batches (calling CreateSortcut.exe repeatedly for each shortcut via python is pretty slow);
Use CreateShortcutBatch.exe, which is activated with gen_shortcut_batch.py.
It reads from a file "arguments.txt", and creates all shortcuts within.
Each line has 4 arguments: Filepath, workingdirectory_of_filepath, shortcutdirectory, shortcutname.

How to write the arguments.txt?
Python example:
```
with open("arguments.txt", "a", encoding="utf-8") as args:
  os.makedirs(shortcut_directory, exist_ok=True)
  arguments = f'"{os.path.realpath(path_to_file)}" "{os.path.dirname(path_to_file)}" "{shortcut_directory}" "{os.path.basename(path_to_file)}"'
  args.write(arguments + "\n")
gen_shortcut.create_shortcut()
```
Note: The folders must already exist for shortcuts to be placed in them.

Lines in the arguments.txt would look like this:
```
"E:\directory\test.png" "E:\directory" "E:\director_two" "test (shortcut).png"
"E:\directory\test2.png" "E:\directory" "E:\director_three" "test2 (shortcut).png"
"E:\directory\test3.png" "E:\directory" "E:\director_four" "test3 (shortcut).png"
```
the text file should end in a newline character

Tips: If you are checking the existence of a shortcut with python, remember to add the .lnk file extension at the end.
