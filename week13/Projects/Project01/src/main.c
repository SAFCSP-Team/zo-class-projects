#include "../include/elf_parser.h"

#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    bool  dump_content = false;
    char* path         = NULL;

    if (argc == 2) {
        path = argv[1];
    } else if (argc == 3 && strcmp(argv[1], "-c") == 0) {
        dump_content = true;
        path         = argv[2];
    } else {
        fprintf(stderr, "usage: %s [-c] <file.o>\n", argv[0]);
        return 1;
    }

    elf_file_obj_t elf_obj;

    if (!read_file(path, &elf_obj)) {
        fprintf(stderr, "%s: cannot read\n", path);
        return 1;
    }

    if (!read_header(&elf_obj)) {
        free(elf_obj.data);
        return 1;
    }

    if (!read_sections(&elf_obj)) {
        free(elf_obj.data);
        return 1;
    }

    if (!read_symbols(&elf_obj)) {
        free(elf_obj.data);
        return 1;
    }

    print_header(&elf_obj);
    print_sections(&elf_obj, dump_content);
    print_symbols(&elf_obj);
    print_relocations(&elf_obj);

    free(elf_obj.data);
    return 0;
}