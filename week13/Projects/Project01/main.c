#include <stdio.h>
#include <string.h>

#include "elf_reader.h"

static void print_usage(const char *prog)
{
    printf("Usage: %s <option> <elf-file>\n", prog);
    printf("options:\n");
    printf("  -h  Display ELF header\n");
    printf("  -S  Display section headers\n");
    printf("  -s  Display symbol table\n");
    printf("  -l  Display program headers\n");
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    const char *option = argv[1];
    const char *path = argv[2];

    elf_file_t *elf = elf_file_create(path);
    if (!elf) {
        return 1;
    }

    if (strcmp(option, "-h") == 0) {elf_file_print_header(elf);
    } else if (strcmp(option, "-S") == 0) {elf_file_print_sections(elf);
    } else if (strcmp(option, "-s") == 0) {elf_file_print_symbols(elf);
    } else if (strcmp(option, "-l") == 0) {elf_file_print_program_headers(elf);
    } 
    
    else {
        printf("Unknown option: %s\n", option);
        elf_file_delete(elf);
        return 1;
    }

    elf_file_delete(elf);
    return 0;
}