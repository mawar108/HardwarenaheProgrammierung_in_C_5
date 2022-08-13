#!/usr/bin/env python3
import os, re, sys, glob

def check_endboss():
    if not os.path.isfile("endboss.png"):
        print('ERROR: There is no file "endboss.png".')
        sys.exit(1)

    size = os.path.getsize("endboss.png")
    
    if size > 500 * 1000:
        print('ERROR: The file "endboss.png" is too large.')
        print(f"It should be less than 500 kB, but the size is {size} byte.")
        sys.exit(1)
    else:
        print("\tOK: File endboss.png exists and is smaller than 500 kB.")

def check_source_files():
    texts = []
    for pattern in "**/*.c", "**/*.h":
        for path in glob.glob(pattern, recursive=True):
            # Exclude provided files
            if path in ["keyboard.c", "keyboard.c"]: continue
            
            with open(path) as f:
                text = f.read()
                texts.append(text)
                
                for number in re.findall(r"\d+", text):
                    if int(number) > 100:
                        print(f'\tERROR: The number {number}, which is larger than 100, was found in file "{path}". Large constants are not allowed to make it less likely that you ignore the stack size limit.')
                        sys.exit(1)
    
    if any("struct" in text for text in texts):
        print('\tOK: The word "struct" was found. Hopefully it has been used in a sensible way.')
    else:
        print('\tERROR: No struct was used.')
        sys.exit(1)
    
    if any("enum" in text for text in texts):
        print('\tOK: The word "enum" was found. Hopefully it has been used in a sensible way.')
    else:
        print('\tERROR: No enum was used.')
        sys.exit(1)

def check_sizes():
    total_size = 0
    files = []
    for path in glob.glob("**/*", recursive=True):
        size = os.path.getsize(path)
        files.append((size, path))
        total_size += size
    
    if len(files) > 1000:
        print(f"\tERROR: Maximum file limit of 1000 exceeded with {len(files)} files")
        sys.exit(1)
    
    if total_size > 1e7:
        print("Files ordered by size:\n")
        files.sort()
        for size, path in files:
            print(f"{size:8} byte - {path}")
        print("-----------------------------------------")
        print(f"{total_size:8} byte total")
        print()
        print("\tERROR: Maximum total file size of 10 mB exceeded.")
        sys.exit(1)

def main():
    print("INFO: Testing various things.\n")
    
    check_endboss()
    check_source_files()
    check_sizes()
    
    print()

if __name__ == "__main__":
    main()

