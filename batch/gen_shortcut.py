import subprocess

def create_shortcut():
    try:
        result = subprocess.run(
            ["CreateShortcut.exe"],
            capture_output=True,
            text=True,
            check=True,
            encoding="utf-8"
        )
        print("Program output:", result.stdout)
        print("Program error:", result.stderr)
    except subprocess.CalledProcessError as e:
        print("Error running CreateShortcut.exe:")
        print("Return code:", e.returncode)
        print("Output:", e.output)
        print("Error output:", e.stderr)
    except FileNotFoundError:
        print("The executable file was not found. Make sure 'CreateShortcut.exe' is in the same directory.")

def main():
    create_shortcut()

if __name__ == "__main__":
    main()
