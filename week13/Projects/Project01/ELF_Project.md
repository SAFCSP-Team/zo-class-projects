# Project: ELF Parser

Lightweight ELF Parser in C

A minimalist C utility that inspects 64-bit ELF (Executable and Linkable Format) binary headers, section tables, and symbol tables on Linux systems. It parses binary metadata and raw section contents directly from disk without executing the target file, operating similarly to a lightweight clone of the system command readelf.
Features

    Magic Byte Verification: Ensures the target file is a valid ELF binary (0x7F 'E' 'L' 'F').

    Header Inspection: Reads and decodes high-level binary architecture details:

        Class & Endianness: Checks bit size (ELF32/ELF64) and byte ordering (Little vs. Big Endian).

        Target Machine & ABI: Identifies the target CPU (x86-64, ARM, Intel 80386) and OS ABI.

        Binary Purpose: Differentiates between executables (ET_EXEC), shared objects (ET_DYN), relocatables (ET_REL), and core dumps (ET_CORE).

        Memory Layout Offsets: Displays entry point address, program header offsets, and section table offsets.

    Section Table Extraction: Parses Section Headers (Elf64_Shdr) and string table (.shstrtab) to output section names, types, addresses, offsets, and sizes.

    Symbol Table Parsing: Inspects .symtab entries, printing symbol names, sizes, types (FUNC, OBJECT, etc.), and bindings (GLOBAL, LOCAL).

    Section Hex Dump (-x): Allows inspection of raw hex contents for any specific section (e.g., .text, .data, .rodata).

Prerequisites

    A Linux-based operating system.

    A C compiler (e.g., gcc or clang).

    Standard C libraries (<stdio.h>, <stdlib.h>, <string.h>, <elf.h>).

Compilation

Compile the C source file using gcc:
Bash

gcc -o e1 ELF.c

Usage
1. Basic Inspection

To inspect the ELF header, section headers, and symbol table of a binary:
Bash

./e1 <elf_file>

Example:
Bash

./e1 test.o

2. Section Hex Dump (-x)

To display the raw hexadecimal content of a specific section, pass the -x flag followed by the section name:
Bash

./e1 -x <section_name> <elf_file>

Examples:
Bash

# Dump the code section (.text)
./e1 -x .text test.o

# Dump the initialized data section (.data)
./e1 -x .data test.o

# Dump read-only data (.rodata)
./e1 -x .rodata test.o