#!/usr/bin/env python3
import os, re, sys, glob, shutil, argparse, subprocess


IDENTIFIER = "[a-zA-Z_][a-zA-Z_0-9]*"

ALLOWED_CHARS = set(
"""
\r\n\t
 !"#$%&\'()*+,-./0123456789:;<=>?@
ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`
abcdefghijklmnopqrstuvwxyz{|}~
"""
)

DISALLOWED_SUBSTRINGS = """
unistd.h
dirent.h
fcntl.h
<io.h>
"io.h"
sys/types.h
sys/stat.h
_POSIX
POSIX_
GNU_
"""

DISALLOWED_IDENTIFIERS = """
S_ISREG
S_ISDIR
F_OK
R_OK
W_OK
X_OK
O_CREAT
O_WRONLY
opendir
strlcat
strlcpy
strdup
ungetc
errno
pragma
static
goto
setjmp
longjmp
"""


def format_code(path):
    if shutil.which("clang-format") is None:
        print("\nERROR: clang-format has not been installed. Please read")
        print("https://pad.hhu.de/xRi3VBfrTBmgOFegWdbzWw#Code-Vorgaben")
        sys.exit(1)

    clang_format = """BasedOnStyle: Google
IndentWidth: 4
DerivePointerAlignment: false
PointerAlignment: Left
ColumnLimit: 100
AllowShortFunctionsOnASingleLine: None
AllowShortLoopsOnASingleLine: false
"""
    with open(".clang-format", "w") as f:
        f.write(clang_format)

    code = subprocess.check_output(["clang-format", path]).decode("utf-8")

    os.remove(".clang-format")

    return code


class CodeStyleError(ValueError):
    def __init__(self, message):
        super().__init__(message)
        self.message = message


def remove_comments_and_strings(s):
    result = []
    i = 0
    n = len(s)
    while i < n:
        c = s[i]

        if s[i : i + 2] == "//":
            # remove line comment
            while i < n and s[i] != "\n":
                i += 1
            continue

        if s[i : i + 2] == "/*":
            # remove block comment
            while i < n and s[i : i + 2] != "*/":
                # but keep line breaks
                if s[i] == "\n":
                    result.append("\n")
                i += 1

            if s[i : i + 2] != "*/":
                raise CodeStyleError("Block comment not closed.")

            # skip "*/"
            i += 2

            continue

        # skip non-quote chars
        if c != '"' and c != "'":
            result.append(c)
            i += 1
            continue

        # parse quoted string
        quote = c
        result.append(c)
        i += 1

        while i < n and s[i] != quote:
            # skip escaped char
            if s[i] == "\\":
                i += 1
            # skip regular string char
            if i < n:
                i += 1

        if i >= n or s[i] != quote:
            raise CodeStyleError(f"{quote} not closed")

        result.append(s[i])
        i += 1

    return "".join(result)


def check_lines_too_long(args, code):
    for line_index, line in enumerate(code.split("\n")):
        if len(line.rstrip()) > args.max_line_length:
            num = line_index + 1
            msg = f"Line {num} is too long ({len(line)} characters):\n\n{line}"
            raise CodeStyleError(msg)


def add_line_numbers(code):
    lines = code.split("\n")
    return "\n".join(f"{i + 1:3d}:{line}" for i, line in enumerate(lines))


def check_globals(path):
    if shutil.which("ctags") is None:
        print("\nERROR: universal-ctags has not been installed. Please read")
        print("https://pad.hhu.de/xRi3VBfrTBmgOFegWdbzWw#Code-Vorgaben")
        sys.exit(1)

    # ctags does not work with *.ts files. Still no globals allowed though!
    if path.endswith(".ts"):
        return

    command = [
        "ctags",
        "-R",
        "-x",
        "--sort=yes",
        "--c-kinds=v",
        "--extras=-F",
        path,
    ]

    result = subprocess.check_output(command).decode("utf-8").strip()

    if result:
        raise CodeStyleError(f"Found global variable(s):\n\n{result}")


def check_multiple_statements(args, code):
    for line_index, line in enumerate(code.split("\n")):
        identifiers = set(re.findall(IDENTIFIER, line))

        if "for" in identifiers or "break" in identifiers:
            continue

        semicolons = line.count(";")

        if semicolons > 1:
            num = line_index + 1
            message = f"Line {num} contains multiple statements ({semicolons} semicolons):\n\n"
            message += f"{line_index + 1:3d}:{line}"
            raise CodeStyleError(message)


def find_curly_pairs(code, max_depth=0):
    starts = []
    for i, c in enumerate(code):
        if c == "{":
            starts.append(i)

        elif c == "}":
            if len(starts) == 0:
                raise CodeStyleError("More '}' than '{' in file.")

            start = starts.pop()

            # Only return curly parentheses on depth 0 by default
            if len(starts) <= max_depth:
                yield start, i

    if len(starts) > 0:
        raise CodeStyleError("More '{' than '}' in file.")


def remove_whitespace_lines(text):
    lines = text.split("\n")

    lines = [line for line in lines if len(line.strip()) > 0]

    return "\n".join(lines)


def check_functions_too_long(args, code):
    pairs = find_curly_pairs(code)

    for start, end in pairs:

        # scan backwards to previous function/struct/include
        # TODO parser could get confused by macros (lets hope nobody uses them)
        i = start
        while i > 0 and code[i] not in '};">':
            i -= 1
        if i > 0:
            i += 1

        # scan forward to skip whitespace
        while i < len(code) and code[i] in " \t\r\n":
            i += 1

        start = i

        function = code[start : end + 1]

        function = remove_whitespace_lines(function)

        function = function.rstrip() + "\n"

        lines = function.count("\n")

        max_lines = args.max_function_lines

        if lines > max_lines:
            message = f"""Function has too many lines ({lines} of {max_lines}).
Note: The function has been formatted automatically.
Whitespace, comments and strings have been removed.\n\n"""
            message += f"{add_line_numbers(function.rstrip())}"
            raise CodeStyleError(message)


def check_funny_symbols(code):
    for c in code:
        if c not in ALLOWED_CHARS:
            message = f"Character '{c}' (Code {ord(c)}) is not allowed."
            raise CodeStyleError(message)


def check_disallowed(args, code, path):
    identifiers = set(re.findall(IDENTIFIER, code))

    for disallowed in DISALLOWED_IDENTIFIERS.strip().split("\n"):
        if disallowed in identifiers:
            raise CodeStyleError(f'"{disallowed}" not allowed (not C99 or frequently used incorrectly)')
    
    for disallowed in DISALLOWED_SUBSTRINGS.strip().split("\n"):
        # Allow unistd.h for keyboard.c just this once
        filename = path.split(os.path.sep)[-1]
        if disallowed == "unistd.h" and filename in ["keyboard.c", "keyboard.h"]:
            continue
        
        if disallowed in code:
            raise CodeStyleError(f'"{disallowed}" not allowed (not C99 or frequently used incorrectly)')

def check_codestyle(args, path):
    with open(path, "rb") as f:
        try:
            f.read().decode("utf-8")
        except UnicodeDecodeError:
            raise CodeStyleError("The file is not UTF-8-encoded.")

    check_globals(path)

    with open(path, encoding="utf-8") as f:
        code = f.read()

    check_disallowed(args, code, path)

    check_lines_too_long(args, code)

    check_multiple_statements(args, remove_comments_and_strings(code))

    code = format_code(path)

    code = remove_comments_and_strings(code)

    check_funny_symbols(code)

    check_functions_too_long(args, code)


def main():
    parser = argparse.ArgumentParser(description="Code Style Checker")

    parser.add_argument(
        "--max-line-length",
        type=int,
        default=100,
        help="Maximum line length (includes comments and strings!)",
    )
    parser.add_argument(
        "--max-function-lines",
        type=int,
        default=60,
        help="Maximum number of lines per function (does not count blank lines and comments)",
    )
    parser.add_argument(
        "--directory",
        type=str,
        default=None,
        help="Directory to search c-, h- and ts-files in",
    )

    args = parser.parse_args()
    if args.directory is not None and not os.path.exists(args.directory):
        print(f"Directory {args.directory} does not exist.")
        sys.exit(1)

    error = 0
    num_files = 0
    for pattern in ["**/*.c", "**/*.h", "**/*.ts"]:
        if args.directory is not None:
            pattern = os.path.join(args.directory, pattern)

        for path in glob.iglob(pattern, recursive=True):
            num_files += 1
            print(f"{num_files:3d}: Checking code style of {path}")
            try:
                check_codestyle(args, path)
            except CodeStyleError as e:
                print(f"\nERROR: Code style violation in file:\n    {path}\n")
                print(e.message)
                print()
                error = 1
            except FileNotFoundError:
                print(f"\nERROR: File {path} not found\n")
                error = 1

    if num_files == 0:
        error = 1
        directory = "current" if args.directory is None else args.directory
        print(f"Could not find any *.c-files or *.h-files {directory} directory")

    print("\n\tOK: Code style test passed.\n")

    sys.exit(error)


if __name__ == "__main__":
    main()
