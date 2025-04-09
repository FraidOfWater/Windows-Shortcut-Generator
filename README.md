# Windows-Shortcut-Generator
Command-line / python importable module to create windows shortcuts. Shortcut generation correctly.
I couldn't find a good python library to do this. All the existing ones fail somewhere (no support for long paths, unicode), so this exists now.
# Features #
1. Ignore path limits.
2. Embrace unicode.
3. Does generate working thumbnails!
4. Command-line `python gen_shortcut.py "full_path_to_file" "directory_to_save_shortcut_in" "shortcut_name"`
5. Module (no arguments) `python gen_shortcut.py`
  * Creates shortcuts from "arguments.txt" file. (Must be created in the same directory as the script)
  * arguments.txt format:
    1. Each line is its own shortcut command.
    2. Each line must have 4 (FOUR) arguments, "Filepath, workingdirectory_of_filepath, shortcutdirectory, shortcutname".
      ```
      "E:\directory\test.png" "E:\directory" "E:\director_two" "test (shortcut).png"
      "E:\directory\test2.png" "E:\directory" "E:\director_three" "test2 (shortcut).png"
      "E:\directory\test3.png" "E:\directory" "E:\director_four" "test3 (shortcut).png"
      ```
      End in a newline to be safe.
  
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
# Building and editing #

1. Py file is there to integrate the C++ code into python.
2. Py file simply calls the C++ .exe. It gives either 3 arguments or 0.
3. If it gives 0 arguments, the .exe attempts to read arguments.txt. If 3 arguments are given, it uses those.

You can build the .cpp by installing visual studio (not code) and c++ build tools from there. Then you build by navigating to the .cpp script and using (in x64 Native Tools Command Terminal):
```
cl CreateShortcut.cpp ole32.lib shell32.lib
```

How to write the arguments.txt?
Python example: (shortcut_directory was created using shortcut_directory = os.path.join())
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
