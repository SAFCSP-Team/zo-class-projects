#include "../include/elf_parser.h"

#include <stdlib.h>
#include <sys/stat.h>

bool read_file(const char* path, elf_file_obj_t* elf_obj) {
    if (!path || !elf_obj)
        return false;

    FILE* file_handler = fopen(path, "rb");

    if (!file_handler)
        return false;

    struct stat file_md;
    if (fstat(fileno(file_handler), &file_md) != 0) {
        fclose(file_handler);
        return false;
    }

    elf_obj->size = file_md.st_size;

    uint8_t* data = malloc(elf_obj->size);
    if (!data) {
        fclose(file_handler);
        return false;
    }

    if (fread(data, elf_obj->size, 1, file_handler) == 0) {
        free(data);
        fclose(file_handler);
        return false;
    }

    elf_obj->data = data;

    fclose(file_handler);
    return true;
}

bool read_header(elf_file_obj_t* elf_obj) {
    if (!elf_obj || !elf_obj->data)
        return false;

    elf_obj->header = *((const Elf64_Ehdr*)elf_obj->data);

    if (elf_obj->header.e_ident[EI_MAG0] != ELFMAG0 || elf_obj->header.e_ident[EI_MAG1] != ELFMAG1 || elf_obj->header.e_ident[EI_MAG2] != ELFMAG2 ||
        elf_obj->header.e_ident[EI_MAG3] != ELFMAG3)
        return false;

    if (elf_obj->header.e_ident[EI_CLASS] != ELFCLASS64) {
        fprintf(stderr, "only 64-bit supported\n");
        return false;
    }

    if (elf_obj->header.e_ident[EI_DATA] != ELFDATA2LSB) {
        fprintf(stderr, "only little-endian supported\n");
        return false;
    }

    if (elf_obj->header.e_ident[EI_VERSION] != EV_CURRENT)
        return false;

    if (elf_obj->header.e_type != ET_REL) {
        fprintf(stderr, "only object files supported\n");
        return false;
    }

    if (elf_obj->header.e_ehsize != sizeof(Elf64_Ehdr))
        return false;
    if (elf_obj->header.e_shentsize != sizeof(Elf64_Shdr))
        return false;

    return true;
}

bool read_sections(elf_file_obj_t* elf_obj) {
    if (!elf_obj || !elf_obj->data)
        return false;

    elf_obj->sections       = (Elf64_Shdr*)(elf_obj->data + elf_obj->header.e_shoff);
    elf_obj->sections_count = elf_obj->header.e_shnum;

    elf_obj->sections_str_name_base = elf_obj->data + elf_obj->sections[elf_obj->header.e_shstrndx].sh_offset;

    return true;
}

bool read_symbols(elf_file_obj_t* elf_obj) {
    if (!elf_obj || !elf_obj->data)
        return false;

    const Elf64_Shdr* symbol_section = get_section_by_type(elf_obj, SHT_SYMTAB);

    if (!symbol_section)
        return false;

    elf_obj->symbols               = (Elf64_Sym*)(elf_obj->data + symbol_section->sh_offset);
    elf_obj->symbols_count         = symbol_section->sh_size / symbol_section->sh_entsize;
    elf_obj->symbols_str_name_base = elf_obj->data + elf_obj->sections[symbol_section->sh_link].sh_offset;

    return true;
}
