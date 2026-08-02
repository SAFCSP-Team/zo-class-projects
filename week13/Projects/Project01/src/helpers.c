#include "../include/elf_parser.h"
#include <string.h>

const Elf64_Shdr* get_section_by_type(const elf_file_obj_t* elf_obj, const uint32_t type) {
    if (!elf_obj || !elf_obj->sections)
        return NULL;

    for (size_t i = 0; i < elf_obj->sections_count; i++)
        if (elf_obj->sections[i].sh_type == type && elf_obj->sections[i].sh_entsize != 0)
            return &elf_obj->sections[i];

    return NULL;
}

const char* get_symbol_name(const elf_file_obj_t* elf_obj, const Elf64_Sym* sym) {
    return (ELF64_ST_TYPE(sym->st_info) != STT_SECTION) ? (const char*)(elf_obj->symbols_str_name_base + sym->st_name) :
                                                          (const char*)(elf_obj->sections_str_name_base + elf_obj->sections[sym->st_shndx].sh_name); // if it's section we take it
                                                                                                                                                     // from section table name
}