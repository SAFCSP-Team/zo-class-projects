
zasm — colored tokenizer output for najd

A small demo built on top of the najd string-reader library (macro-based NSR_* variant) that tokenizes a source string and prints it back out with ANSI color highlighting, similar to how an assembler listing or syntax highlighter would render mnemonics vs. immediate values.

Files
File	Role
najd.h / najd.c	The library itself — unchanged, provided as-is
main.c	Defines the zasm reader table and implements the 5 lifecycle hooks
What's in main.c
Reader table (zasm)

Three token readers built with the library's NSR_* macros:

id	name	matches
1	number	digits (isdigit)
2	identifier	letters/underscore start, then alnum/underscore
3	space	whitespace (isspace)
The 5 hooks

The original code had these as bare comments inside the read loop. They're now real functions:

on_create() — prints a listing header before tokenization starts.
pre_read(str) — runs before each token is read. Currently a no-op, left as a place to add tracing/counters later.
on_result(result) — the core of the coloring. Switches on result->id and prints the token's value wrapped in the matching ANSI color code:
number → red
identifier → bold cyan (treated as a mnemonic)
space → printed as-is, to preserve original spacing
post_read(result) — runs after each token, after on_result. Also a no-op placeholder for now.
on_delete() — prints a listing footer after the source is fully consumed and the string is deleted.
Main loop
c
on_create();
while(najd_string_nextable(str)){
    pre_read(str);
    result = najd_string_read(str);
    on_result(result);
    post_read(result);
    najd_string_result_delete(result);
    najd_buffer_reset(&str->buffer);
}
najd_string_delete(str);
on_delete();
Build & run
bash
gcc -Wall -o zasm main.c najd.c
./zasm
Sample output

Input string: "int 10"

; zasm listing
int 10
; end of listing

...except int renders in bold cyan and 10 in red in an actual terminal — the plain text above doesn't show the ANSI colors.

Known limitation

The zasm table only recognizes digits, identifiers, and whitespace. If code in main() is changed to something with punctuation (e.g. "mov ax, 10"), the , won't match any reader. Since najd_string_read doesn't advance the cursor when nothing matches, the while loop will spin forever on that character. A symbol/punctuation reader would need to be added to the table to support real assembly-style syntax with operands separated by commas, brackets, etc.