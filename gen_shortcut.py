import subprocess
import os
import argparse

# "E:\AntiDupl\All things pony\Mane 6\Applejack (M6)\(Apple family)\(Changeling) Apple Family\Long af folder with a ridficulously long name\still longer af folder lenghts sfgdjka gdfljng hjkdfshdjkfg hsjdfg  hjfsgdhjbsgk dfbshjkdfg  hbjkdsfgsdfgb hjk\58ea953561dd2_1407149__safe_artist-colon-momomistress_princesscadance_princesscelestia_princessluna_-wilightsparkle_alcohol_alicorn_alicorntetrarchy_blushing_ch.png.1a2563d642eb86cd84247492ced60b83.png"
def create_shortcut(arguments):
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
    parser = argparse.ArgumentParser(description='CreateShortcut.exe and gen_shortcut.py must be located in the same directory. The python script passes arguments to the C++ code, which creates the shortcut.')
    parser.add_argument('target_path',  type=str,  help='The filepath of the file you want to create a shortcut for.')
    parser.add_argument('shortcut_directory',type=str,  help='The folder to save the shortcut in.')
    parser.add_argument('shortcut_name',type=str,  help='The name of the shortcut.')

    # Parse the arguments
    args = parser.parse_args()

    arguments = [
        r"CreateShortcut.exe",  
        os.path.realpath(args.target_path),
        os.path.dirname(args.target_path),
        os.path.dirname(args.shortcut_directory),
        os.path.basename(args.shortcut_name)
    ]
    # Call the function to create the shortcut
    create_shortcut(arguments)

if __name__ == "__main__":
    main()