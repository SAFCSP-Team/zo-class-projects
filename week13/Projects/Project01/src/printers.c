#include "../include/elf_parser.h"

void print_header(const elf_file_obj_t* elf_obj) {

    printf("\n\t--------------------------------------------------------\n");
    printf("\t\t\t\tELF Header \n");
    printf("\t--------------------------------------------------------\n\n");

    printf("\tMagic:   ");
    for (size_t i = 0; i < EI_NIDENT; i++)
        printf("%02x ", elf_obj->header.e_ident[i]);
    printf("\n");

    printf("\t%-35s %s\n", "Class:", header_class_str(elf_obj->header.e_ident[EI_CLASS]));
    printf("\t%-35s %s\n", "Data:", header_data_str(elf_obj->header.e_ident[EI_DATA]));
    printf("\t%-35s %u%s\n", "Ident version:", elf_obj->header.e_ident[EI_VERSION], elf_obj->header.e_ident[EI_VERSION] == EV_CURRENT ? " (current)" : "");
    printf("\t%-35s %s\n", "OS/ABI:", header_osabi_str(elf_obj->header.e_ident[EI_OSABI]));
    printf("\t%-35s %u\n", "ABI version:", elf_obj->header.e_ident[EI_ABIVERSION]);

    printf("\t%-35s %s\n", "Type:", header_type_str(elf_obj->header.e_type));
    printf("\t%-35s %s\n", "Machine:", header_machine_str(elf_obj->header.e_machine));
    printf("\t%-35s 0x%u\n", "Version:", elf_obj->header.e_version);

    printf("\t%-35s 0x%lu\n", "Entry point address:", elf_obj->header.e_entry);
    printf("\t%-35s %lu (bytes into file)\n", "Start of program headers:", elf_obj->header.e_phoff);
    printf("\t%-35s %lu (bytes into file)\n", "Start of section headers:", elf_obj->header.e_shoff);
    printf("\t%-35s 0x%u \n", "Flags:", elf_obj->header.e_flags);

    printf("\t%-35s %u (bytes)\n", "Size of this header:", elf_obj->header.e_ehsize);
    printf("\t%-35s %u (bytes)\n", "Size of program headers:", elf_obj->header.e_phentsize);
    printf("\t%-35s %u\n", "Number of program headers:", elf_obj->header.e_phnum);
    printf("\t%-35s %u (bytes)\n", "Size of section headers:", elf_obj->header.e_shentsize);
    printf("\t%-35s %u\n", "Number of section headers:", elf_obj->header.e_shnum);
    printf("\t%-35s %u\n", "Section header string table index:", elf_obj->header.e_shstrndx);
}

void print_sections(const elf_file_obj_t* elf_obj, const bool dump_content) {
    if (!elf_obj || !elf_obj->data || !elf_obj->sections)
        return;
    printf("\n\t--------------------------------------------------------\n");
    printf("\t\t\t\tSections  \n");
    printf("\t--------------------------------------------------------\n");

    printf("\n\n\t%-8s %-24s %-20s %-20s %-19s %-15s %-8s %-6s %-8s %-11s %s\n\n", "[Nr]", "Name", "Size", "EntSize", "Type", "Address", "Flags", "Link", "info", "Align",
           "Offset");
    for (size_t i = 0; i < elf_obj->sections_count; i++) {
        const Elf64_Shdr*    section = &elf_obj->sections[i];

        const unsigned char* section_name = elf_obj->sections_str_name_base + section->sh_name;

        char                 buf[16];
        section_flags_str(section->sh_flags, buf);

        printf("\t%2zu: %2s  %-20s %016lx %-5s %016lx %-5s %-15s %016lx %-4s %-8s "
               "%-6u "
               "%-9u %-8lu "
               "%08lx\n",
               i, "", section_name, section->sh_size, "", section->sh_entsize, "", section_type_str(section->sh_type), section->sh_addr, "", buf, section->sh_link,
               section->sh_info, section->sh_addralign, section->sh_offset);

        if (dump_content) {
            for (size_t j = 0; j < section->sh_size; j++) {
                if (j % 32 == 0)
                    printf("\n\t\t\t\t");
                printf("%02x ", elf_obj->data[section->sh_offset + j]);
            }
            printf("\n\n");
        }
    }
}

void print_symbols(const elf_file_obj_t* elf_obj) {
    if (!elf_obj || !elf_obj->data || !elf_obj->symbols)
        return;

    printf("\n\t\t--------------------------------------------------------\n");
    printf("\t\t\t\t\tSymbol Table \n");
    printf("\t\t--------------------------------------------------------\n\n");

    printf("\n\n\t%-13s %-15s %-11s %-9s %-11s %-9s %-10s %-10s\n\n", "Num", "Value", "Size", "Type", "Bind", "Vis", "NDX", "Name");

    for (size_t i = 0; i < elf_obj->symbols_count; i++) {

        const Elf64_Sym* cur_symbol = &elf_obj->symbols[i];

        const char*      cur_symbol_name = get_symbol_name(elf_obj, cur_symbol);

        printf("\t%-8zu %016lx %-5s %-8lu %-10s %-10s %-10s ", i, cur_symbol->st_value, "", cur_symbol->st_size, symbol_type_str(cur_symbol->st_info),
               symbol_bind_str(cur_symbol->st_info), symbol_visibility_str(cur_symbol->st_other));

        const char* shndx_str = symbol_shndx_str(cur_symbol->st_shndx);
        shndx_str ? printf("%-10s", shndx_str) : printf("%-10u", cur_symbol->st_shndx);

        printf("%-10s\n", cur_symbol_name);
    }
}

void print_rela_section(const elf_file_obj_t* elf_obj, const Elf64_Shdr* section) {
    const Elf64_Rela* cur_rela_section = (const Elf64_Rela*)(elf_obj->data + section->sh_offset);
    size_t            rela_count       = section->sh_size / section->sh_entsize;

    printf("\n\t\t--------------------------------------------------------\n");
    printf("\t\t\tRelocations: %s(%zu)\n", elf_obj->sections_str_name_base + section->sh_name, rela_count);
    printf("\t\t--------------------------------------------------------\n\n");
    printf("\t%-8s %-14s %-14s %-18s %-18s %s\n\n", "Num", "Offset", "Info", "Type", "Sym. Value", "Sym. Name + Addend");

    for (size_t i = 0; i < rela_count; i++) {
        const Elf64_Rela* cur_entry = &cur_rela_section[i];
        const Elf64_Sym*  sym_info  = &elf_obj->symbols[ELF64_R_SYM(cur_entry->r_info)];

        printf("\t%2zu %-4s %012lx   %012lx   %-18s %016lx %-5s   %s ", i, "", cur_entry->r_offset, cur_entry->r_info, rela_type_str(ELF64_R_TYPE(cur_entry->r_info)),
               sym_info->st_value, "", get_symbol_name(elf_obj, sym_info));

        cur_entry->r_addend < 0 ? printf("%ld\n", cur_entry->r_addend) : printf("%lx\n", cur_entry->r_addend);
    }
}

void print_relocations(const elf_file_obj_t* elf_obj) {
    if (!elf_obj || !elf_obj->sections || !elf_obj->symbols)
        return;

    for (size_t i = 0; i < elf_obj->sections_count; i++) {
        if (elf_obj->sections[i].sh_type == SHT_RELA)
            print_rela_section(elf_obj, &elf_obj->sections[i]);
    }
}
