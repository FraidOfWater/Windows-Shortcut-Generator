import subprocess
import os
import argparse



def create_shortcut(arguments, verbose=True, silent=False):
    """
    Creates shortcuts from a list of tuples ('filepath', shortcutpath').
    
    Filepath = full path to file
    
    Shortcutpath = full path to shortcut

    Verbose=True: Print whats happening, with this false, errors are STILL displayed. DEFAULT: True
    
    Silent=True: Don't print errors. DEFAULT: False

    This function also validates that the shortcuts were indeed created, and prints to terminal.
    """
    def generate_arguments_txt(arguments):
        "Generate arguments.txt and create folders for all shortcuts"
        try:
            with open("arguments.txt", "a", encoding="utf-8") as args:
                for argument in arguments:

                    filepath = argument[0]
                    filepath_directory = os.path.dirname(filepath)
                    shortcut_path = argument[1]
                    shortcut_directory = os.path.dirname(shortcut_path)
                    
                    arguments = f'"{filepath}" "{filepath_directory}" "{shortcut_directory}" "{os.path.basename(shortcut_path)}"'

                    os.makedirs(os.path.dirname(os.path.join(shortcut_directory, os.path.basename(filepath))), exist_ok=True)

                    if not os.path.exists(f"{os.path.join(shortcut_directory, os.path.basename(filepath))}.lnk"):
                        args.write(arguments + "\n")
                        validate_these.append(f"{os.path.join(shortcut_directory, os.path.basename(filepath))}.lnk")
        except Exception as e:
            print("Error generating arguments.txt", e)
    def launch_subprocess():
        try:
            #result = subprocess.run(
            #    ["CreateShortcut_batch.exe"],
            #    capture_output=True,
            #    text=True,
            #    check=True,
            #    encoding="utf-8"
            #)
            process = subprocess.Popen(
                ["CreateShortcut_batch.exe"],  # Example command (replace with your command)
                stdout=subprocess.PIPE,  # Capture standard output
                stderr=subprocess.PIPE,  # Capture standard error
                text=True,  # Make sure the output is returned as text (not bytes)
                encoding="utf-8"
            )
            if verbose and not silent:
                for line in process.stdout:
                    print(line)
            if not silent:
                errors = 0
                for line in process.stdout:
                    errors += 1
                if errors != 0:
                    print(f"ERRORS: {errors}")
                for line in process.stdout:
                    print(line)

        except subprocess.CalledProcessError as e:
            print(f"Error: {e}")
        except FileNotFoundError:
            print("The executable file was not found. Make sure 'CreateShortcut.exe' is in the same directory.")
    def validate():
        validated = 0
        errors = 0
        for x in validate_these:
            if os.path.exists(x) and os.path.isfile(x):
                validated += 1
                pass
            else:
                print("Shortcut wasn't created:", x)
                errors += 1
        print(f"Shortcuts created: {validated} with {errors} errors.")
            
    validate_these = []
    if os.path.isfile("arguments.txt"):
        os.remove("arguments.txt")
    generate_arguments_txt(arguments)
    launch_subprocess()
    if not silent:
        validate()

def create_shortcut_command_line(arguments):
    # Run the executable with arguments
    try:
        result = subprocess.run(arguments, capture_output=True, text=True, check=True)
        #print("Program output:", result.stdout)  # Output of the program
        if result.stderr != '':
            print("Program error:", result.stderr)   # Error output (if any)
    except subprocess.CalledProcessError as e:
        print(f"Error: {e}")
    except FileNotFoundError:
        print("The executable file was not found.")

def main():
    # Set up argument parser
    parser = argparse.ArgumentParser(description='CreateShortcut.exe and this script must be located in the same directory. This script creates shortcuts.')
    
    # Only target_path is required, others are optional
    parser.add_argument('target_path', type=str, help='The filepath of the file you want to create a shortcut for.')
    parser.add_argument('shortcut_directory', nargs='?', type=str, default=None, help='The folder to save the shortcut in. Defaults to the Desktop.')
    parser.add_argument('shortcut_name', nargs='?', type=str, default=None, help='The name of the shortcut. Defaults to the target filename.')

    # Parse the arguments
    args = parser.parse_args()

    # If no shortcut_directory is provided, use the Desktop
    if not args.shortcut_directory:
        # Get the Desktop path
        desktop_path = os.path.join(os.path.expanduser("~"), "Desktop")
        args.shortcut_directory = desktop_path

    # If no shortcut_name is provided, use the filename of the target_path
    if not args.shortcut_name:
        args.shortcut_name = os.path.basename(os.path.realpath(args.target_path))

    # Prepare the arguments for CreateShortcut.exe
    arguments = [
        r"CreateShortcut.exe",
        os.path.realpath(args.target_path),
        os.path.dirname(os.path.realpath(args.target_path)),
        os.path.realpath(args.shortcut_directory),
        os.path.basename(args.shortcut_name)
    ]

    # Call the function to create the shortcut
    create_shortcut_command_line(arguments)

if __name__ == "__main__":
    main()
