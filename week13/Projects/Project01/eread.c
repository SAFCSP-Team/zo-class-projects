#include "eread.h"

bool ereader_read_file(char *filename,size_t file_size, void* elf_data){
    if(filename == NULL || elf_data == NULL) return false;

    // Open File
    FILE* elf_fh = fopen(filename,"r");

    if (elf_fh == NULL) return false;

    // Read into elf_data block
    size_t read_into = fread(elf_data, file_size, 1, elf_fh);

    if(read_into == 0) {
        fclose(elf_fh);
        return false;
    }
    
    fclose(elf_fh);
    return true;
}

bool ereader_delete_elf_data(void *elf_data){
    if (elf_data) {
        free(elf_data);
    }
}

int ereader_find_section_by_type(Elf64_Shdr* elf_sec, int section_type, int entries_size){
    for (int i = 0; i < entries_size; i++) {
        if (elf_sec[i].sh_type == section_type){
            return i;
        }
    }
    return -1;
}

int ereader_find_section_by_name(void* elf_data, Elf64_Ehdr* elf_header, Elf64_Shdr* elf_sec, char* section_name){
    // SECTION HEADER STRING TABLE
    Elf64_Shdr* shstrtab = (Elf64_Shdr*) &elf_sec[elf_header->e_shstrndx];

    // SECTIONS TABLE FIRST BASE
    char* shstrbase = (char*) (elf_data + shstrtab->sh_offset);

    for (int i = 0; i < elf_header->e_shnum; i++) {
        if (strcmp((shstrbase+elf_sec[i].sh_name),section_name) == 0){
            return i;
        }
    }

    return -1;
}

/* ELF HEADER */

void ereader_print_elfheader_ident_magic(Elf64_Ehdr* elf_header){
    if(elf_header){
        printf("  %-38s: %c %c %c %c\n", "Magic",
            elf_header->e_ident[EI_MAG0],
            elf_header->e_ident[EI_MAG1],
            elf_header->e_ident[EI_MAG2],
            elf_header->e_ident[EI_MAG3]
        );
    }
}

void ereader_print_elfheader_ident_class(Elf64_Ehdr* elf_header){
    if(elf_header){
        char* class_type = "";
        switch (elf_header->e_ident[EI_CLASS])
        {
        case ELFCLASS32:
            class_type = BIT_32;
            break;
        
        case ELFCLASS64:
            class_type = BIT_64;
            break;
        
        default:
            class_type = NONE_CLASS;
            break;
        }
        printf("  %-38s: %s\n", "Class",class_type);
    }
}

void ereader_print_elfheader_ident_endian(Elf64_Ehdr* elf_header){
    if(elf_header){
        char* data_encoding_type = "";
        switch (elf_header->e_ident[EI_DATA])
        {
        case ELFDATA2LSB:
            data_encoding_type = ELF_LITTLE_ENDIAN;
            break;
        
        case ELFDATA2MSB:
            data_encoding_type = ELF_BIG_ENDIAN;
            break;
        
        default:
            data_encoding_type = UNKNOWN_DATA;
            break;
        }
        printf("  %-38s: %s\n", "Data",data_encoding_type);
    }
}

void ereader_print_elfheader_ident_version(Elf64_Ehdr* elf_header){
    if(elf_header){
        char* version = "";
        switch (elf_header->e_ident[EI_VERSION])
        {
        case ELFDATA2LSB:
            version = ELF_VERSION_CURRENT;
            break;
        
        
        default:
            version = ELF_VERSION_NONE;
            break;
        }
        printf("  %-38s: %s\n", "Version",version);
    }
}

void ereader_print_elfheader_ident_abi_type(Elf64_Ehdr* elf_header){
        if(elf_header){
        char* osabi_type = "";
        switch (elf_header->e_ident[EI_OSABI])
        {
        case ELFOSABI_SYSV:
            osabi_type = OSABI_UNIX;
            break;

        case ELFOSABI_HPUX:
            osabi_type = OSABI_HPUX;
            break;
        
        case ELFOSABI_NETBSD:
            osabi_type = OSABI_NETBSD;
            break;
        
        case ELFOSABI_LINUX:
            osabi_type = OSABI_LINUX;
            break;

        case ELFOSABI_SOLARIS:
            osabi_type = OSABI_SOLARIS;
            break;

        case ELFOSABI_IRIX:
            osabi_type = OSABI_IRIX;
            break;

        case ELFOSABI_FREEBSD:
            osabi_type = OSABI_FREEBSD;
            break;

        case ELFOSABI_TRU64:
            osabi_type = OSABI_TRU64;
            break;

        case ELFOSABI_ARM:
            osabi_type = OSABI_ARM;
            break;
        
        case ELFOSABI_STANDALONE:
            osabi_type = OSABI_STANDALONE;
            break;    

        default:
            osabi_type = OSABI_UNIX;
            break;
        }
        printf("  %-38s: %s\n", "OS/ABI",osabi_type);
    }
}

void ereader_print_elfheader_ident_abi_version(Elf64_Ehdr* elf_header){
    if(elf_header){
        printf("  %-38s: %d\n", "ABI Version",elf_header->e_ident[EI_ABIVERSION]);
    }
}

void ereader_print_elfheader_type(Elf64_Ehdr* elf_header){
    if(elf_header){
        char* ob_type = "";
        switch (elf_header->e_type)
        {
        case ET_REL:
            ob_type = OB_REL;
            break;
        
        case ET_EXEC:
            ob_type = OB_EXEC;
            break;
    
        case ET_DYN:
            ob_type = OB_SHR;
            break;
                   
        case ET_CORE:
            ob_type = OB_CORE;
            break;
        
        default:
            ob_type = OB_UNKNOWN;
            break;
        }
        printf("  %-38s: %s\n", "Type",ob_type);
    }
}
void ereader_print_elfheader_machine(Elf64_Ehdr* elf_header){
    if(elf_header){
        char* ob_machine = "";
        switch (elf_header->e_machine)
        {
        case EM_NONE:
            ob_machine = MACH_NONE;
            break;
        
        case EM_M32:
            ob_machine = MACH_M32;
            break;
    
        case EM_SPARC:
            ob_machine = MACH_SPARC;
            break;
                   
        case EM_386:
            ob_machine = MACH_386;
            break;
        
        case EM_68K:
            ob_machine = MACH_68K;
            break;
        
        case EM_88K:
            ob_machine = MACH_88K;
            break;
        
        case EM_860:
            ob_machine = MACH_860;
            break;
        
        case EM_MIPS:
            ob_machine = MACH_MIPS;
            break;
        
        case EM_S370:
            ob_machine = MACH_S370;
            break;
        
        case EM_PARISC:
            ob_machine = MACH_PARISC;
            break;
        
        case EM_PPC:
            ob_machine = MACH_PPC;
            break;
        
        case EM_PPC64:
            ob_machine = MACH_PPC64;
            break;
        
        case EM_S390:
            ob_machine = MACH_S390;
            break;
        
        case EM_ARM:
            ob_machine = MACH_ARM;
            break;
        
        case EM_X86_64:
            ob_machine = MACH_X86_64;
            break;
        
        case EM_AARCH64:
            ob_machine = MACH_AARCH64;
            break;
        
        default:
            ob_machine = OB_UNKNOWN;
            break;
        }
        printf("  %-38s: %s\n", "Machine",ob_machine);
    }
}

void ereader_print_elfheader_file_version(Elf64_Ehdr* elf_header){
    if(elf_header){
        printf("  %-38s: %x\n", "Version",elf_header->e_version);
    }
}

void ereader_print_elfheader_entry_vaddress(Elf64_Ehdr* elf_header){
    if(elf_header){
        printf("  %-38s: %lx\n", "Entry point address",elf_header->e_entry);
    }
}
void ereader_print_elfheader_prhd_offset(Elf64_Ehdr* elf_header){
    if(elf_header){
        printf("  %-38s: %ld (bytes)\n", "Start of program headers",elf_header->e_phoff);
    }
}
void ereader_print_elfheader_sechd_offset(Elf64_Ehdr* elf_header){
    if(elf_header){
        printf("  %-38s: %ld (bytes)\n", "Start of section headers",elf_header->e_shoff);
    }
}

void ereader_print_elfheader_machine_flags(Elf64_Ehdr* elf_header){
    if(elf_header){
        printf("  %-38s: %x\n", "Flags",elf_header->e_flags);
    }
}
void ereader_print_elfheader_size(Elf64_Ehdr* elf_header){
    if(elf_header){
        printf("  %-38s: %d (bytes)\n", "Size of ELF header",elf_header->e_ehsize);
    }
}

void ereader_print_elfheader_prhd_entry_size(Elf64_Ehdr* elf_header){
    if(elf_header){
        printf("  %-38s: %d (bytes)\n", "Size of program headers",elf_header->e_phentsize);
    }
}
void ereader_print_elfheader_prhd_entries_size(Elf64_Ehdr* elf_header){
    if(elf_header){
        printf("  %-38s: %d\n", "Number of program headers",elf_header->e_phnum);
    }
}

void ereader_print_elfheader_sechd_entry_size(Elf64_Ehdr* elf_header){
    if(elf_header){
        printf("  %-38s: %d (bytes)\n", "Size of section headers",elf_header->e_shentsize);
    }
}
void ereader_print_elfheader_sechd_entries_size(Elf64_Ehdr* elf_header){
    if(elf_header){
        printf("  %-38s: %d\n", "Number of section headers",elf_header->e_shnum);
    }
}

void ereader_print_elfheader_sechd_index(Elf64_Ehdr* elf_header){
    if(elf_header){
        printf("  %-38s: %d\n", "Section header string table index",elf_header->e_shstrndx);
    }
}

void ereader_print_elfheader(Elf64_Ehdr* elf_header){
    printf("\n=================================================================\n");
    printf("                         ELF HEADER\n");
    printf("=================================================================\n");
    ereader_print_elfheader_ident_magic(elf_header);
    ereader_print_elfheader_ident_class(elf_header);
    ereader_print_elfheader_ident_endian(elf_header);
    ereader_print_elfheader_ident_version(elf_header);
    ereader_print_elfheader_ident_abi_type(elf_header);
    ereader_print_elfheader_ident_abi_version(elf_header);

    ereader_print_elfheader_type(elf_header);
    ereader_print_elfheader_machine(elf_header);

    ereader_print_elfheader_file_version(elf_header);

    ereader_print_elfheader_entry_vaddress(elf_header);
    ereader_print_elfheader_prhd_offset(elf_header);
    ereader_print_elfheader_sechd_offset(elf_header);

    ereader_print_elfheader_machine_flags(elf_header);
    ereader_print_elfheader_size(elf_header);

    ereader_print_elfheader_prhd_entry_size(elf_header);
    ereader_print_elfheader_prhd_entries_size(elf_header);

    ereader_print_elfheader_sechd_entry_size(elf_header);
    ereader_print_elfheader_sechd_entries_size(elf_header);

    ereader_print_elfheader_sechd_index(elf_header);
}

/* SECTIONS */

void ereader_print_section_type(Elf64_Shdr* elf_sec){
    if(elf_sec){
        char* sec_type = "";
        switch (elf_sec->sh_type)
        {
        case SHT_NULL:
            sec_type = SEC_NULL;
            break;
        
        case SHT_PROGBITS:
            sec_type = SEC_PROGBITS;
            break;
    
        case SHT_SYMTAB:
            sec_type = SEC_SYMTAB;
            break;
                   
        case SHT_STRTAB:
            sec_type = SEC_STRTAB;
            break;
        
        case SHT_RELA:
            sec_type = SEC_RELA;
            break;
        
        case SHT_HASH:
            sec_type = SEC_HASH;
            break;
        
        case SHT_DYNAMIC:
            sec_type = SEC_DYNAMIC;
            break;
        
        case SHT_NOTE:
            sec_type = SEC_NOTE;
            break;
        
        case SHT_NOBITS:
            sec_type = SEC_NOBITS;
            break;
        
        case SHT_REL:
            sec_type = SEC_REL;
            break;
        
        case SHT_SHLIB:
            sec_type = SEC_SHLIB;
            break;
        
        case SHT_DYNSYM:
            sec_type = SEC_DYNSYM;
            break;
        
        case SHT_LOPROC:
            sec_type = SEC_LOPROC;
            break;
        
        case SHT_HIPROC:
            sec_type = SEC_HIPROC;
            break;
        
        case SHT_LOUSER:
            sec_type = SEC_LOUSER;
            break;
        
        case SHT_HIUSER:
            sec_type = SEC_HIUSER;
            break;
        
        default:
            sec_type = SEC_UNKNOWN;
            break;
        }
        printf("  %-34s: %s\n", "Section type",sec_type);
    }
}

void ereader_print_section_flags(Elf64_Shdr* elf_sec){
    if(elf_sec) {
        printf("  %-34s: ", "Section flags");
        if((elf_sec->sh_flags & SHF_WRITE)) {
            printf("%c",'W');
        }
        if((elf_sec->sh_flags & SHF_ALLOC)) {
            printf("%c",'A');
        }
        if((elf_sec->sh_flags & SHF_EXECINSTR)) {
            printf("%c",'X');
        }
        if((elf_sec->sh_flags & SHF_INFO_LINK)) {
            printf("%c",'I');
        }
        if((elf_sec->sh_flags & SHF_MERGE)) {
            printf("%c",'M');
        }
        if((elf_sec->sh_flags & SHF_STRINGS)) {
            printf("%c",'S');
        }
        if((elf_sec->sh_flags & SHF_LINK_ORDER)) {
            printf("%c",'L');
        }
        if((elf_sec->sh_flags & SHF_OS_NONCONFORMING)) {
            printf("%c",'O');
        }
        if((elf_sec->sh_flags & SHF_GROUP)) {
            printf("%c",'G');
        }
        if((elf_sec->sh_flags & SHF_TLS)) {
            printf("%c",'T');
        }
        if((elf_sec->sh_flags & SHF_COMPRESSED)) {
            printf("%c",'C');
        }
        if((elf_sec->sh_flags & SHF_EXCLUDE)) {
            printf("%c",'E');
        }
        printf("\n");
    }
}

void ereader_print_elfsections(void* elf_data, Elf64_Shdr* elf_sec, Elf64_Ehdr* elf_header){
    // SECTION HEADER STRING TABLE
    Elf64_Shdr* shstrtab = (Elf64_Shdr*) &elf_sec[elf_header->e_shstrndx];

    // SECTIONS TABLE FIRST BASE
    char* shstrbase = (char*) (elf_data + shstrtab->sh_offset);

    printf("\n=================================================================\n");
    printf("                            SECTIONS\n");
    printf("=================================================================\n");
    for(int i = 0; i < elf_header->e_shnum; i++) {
        printf("-----------------------------------------------------------------\n");
        printf("  Section [%2d]  %s\n",i, shstrbase + elf_sec[i].sh_name);
        printf("-----------------------------------------------------------------\n");
        printf("  %-34s: %s\n","Section name",shstrbase + elf_sec[i].sh_name);
        printf("  %-34s: %lx\n","Section size",(elf_sec[i].sh_size));
        ereader_print_section_type(elf_sec+i);
        printf("  %-34s: %lx\n","Section entry size",elf_sec[i].sh_entsize);
        printf("  %-34s: %lx\n","Section virtual Address",elf_sec[i].sh_addr);
        ereader_print_section_flags(elf_sec+i);
        printf("  %-34s: %d\n","Section link index",elf_sec[i].sh_link);
        printf("  %-34s: %d\n","Section additional information",elf_sec[i].sh_info);
        printf("  %-34s: %lx\n","Section offset",elf_sec[i].sh_offset);
        printf("  %-34s: %lx\n","Section address alignment",elf_sec[i].sh_addralign);
    }

}

/* SYMBOLS */

void ereader_print_symbol_type(Elf64_Sym* elf_sym){
    if(elf_sym){
        unsigned char bind = ELF64_ST_BIND(elf_sym->st_info);
        unsigned char type = ELF64_ST_TYPE(elf_sym->st_info);

        char* sym_type = "";
        switch (type)
        {
        case STT_NOTYPE:
            sym_type = STT_NOTYPE_SYM;
            break;
        
        case STT_OBJECT:
            sym_type = STT_OBJECT_SYM;
            break;
        
        case STT_FUNC:
            sym_type = STT_FUNC_SYM;
            break;
        
        case STT_SECTION:
            sym_type = STT_SECTION_SYM;
            break;
        
        case STT_FILE:
            sym_type = STT_FILE_SYM;
            break;
        
        case STT_LOPROC:
            sym_type = STT_LOPROC_SYM;
            break;
        
        case STT_HIPROC:
            sym_type = STT_HIPROC_SYM;
            break;
        
        default:
            sym_type = STT_UNKNOWN;
            break;
        }

        char* sym_bind = "";
        switch (bind)
        {
        case STB_LOCAL:
            sym_bind = STB_LOCAL_SYM;
            break;
        
        case STB_GLOBAL:
            sym_bind = STB_GLOBAL_SYM;
            break;
        
        case STB_WEAK:
            sym_bind = STB_WEAK_SYM;
            break;
        
        case STB_LOPROC:
            sym_bind = STB_LOPROC_SYM;
            break;
        
        case STB_HIPROC:
            sym_bind = STB_HIPROC_SYM;
            break;
        
        default:
            sym_bind = STB_UNKNOWN;
            break;
        }

        printf(" Type(%s)",sym_type);
        printf(" Bind(%s)",sym_bind);
    }
}

void erader_print_symbol_vis(Elf64_Sym* elf_sym){
    if(elf_sym){
        unsigned char vis = ELF64_ST_VISIBILITY(elf_sym->st_other);

        char* sym_vis = "";
        switch (vis)
        {
        case STV_DEFAULT:
            sym_vis = STV_DEFAULT_SYM;
            break;
        
        case STV_INTERNAL:
            sym_vis = STV_INTERNAL_SYM;
            break;
        
        case STV_HIDDEN:
            sym_vis = STV_HIDDEN_SYM;
            break;
        
        case STV_PROTECTED:
            sym_vis = STV_PROTECTED_SYM;
            break;
        
        default:
            sym_vis = STV_UNKNOWN;
            break;
        }
        printf(" Visibility(%s)",sym_vis);
    }
}

void ereader_print_symbols(void* elf_data, Elf64_Ehdr* elf_header, Elf64_Shdr* elf_sections, Elf64_Shdr* elf_sym_sh, Elf64_Sym* elf_sym){
    int total_symbols_entites = elf_sym_sh->sh_size/elf_sym_sh->sh_entsize;
    
    // STRTAB HEADER
    Elf64_Shdr* strtab = (Elf64_Shdr*) &elf_sections[elf_sym_sh->sh_link];
    char* strtabbase = (char*) elf_data + strtab->sh_offset;

    printf("\n=================================================================\n");
    printf("                         SYMBOL TABLE\n");
    printf("=================================================================\n");
    for (int i = 0; i < total_symbols_entites; i++) {
        printf("-----------------------------------------------------------------\n");
        printf("  Symbol [%3d]\n",i);
        printf("-----------------------------------------------------------------\n");
        printf("  %-24s: %lx\n","Value",elf_sym[i].st_size);
        printf("  %-24s: %ld\n","Size",elf_sym[i].st_size);
        printf("  %-24s:","Type/Bind");
        ereader_print_symbol_type(elf_sym+i);
        printf("\n");
        printf("  %-24s:","Visibility");
        erader_print_symbol_vis(elf_sym+i);
        printf("\n");
        printf("  %-24s: %d\n","Section index",elf_sym[i].st_shndx);
        printf("  %-24s: %s\n","Name",strtabbase + elf_sym[i].st_name);
    }
}

/* RELOCATIONS */

void ereader_print_rela_type(unsigned char rel_type){
    char* rel_type_str = "";
    switch (rel_type)
    {
    case R_X86_64_NONE:
        rel_type_str = RELA_NONE;
        break;

    case R_X86_64_64:
        rel_type_str = RELA_64;
        break;

    case R_X86_64_PC32:
        rel_type_str = RELA_PC32;
        break;

    case R_X86_64_GOT32:
        rel_type_str = RELA_GOT32;
        break;

    case R_X86_64_PLT32:
        rel_type_str = RELA_PLT32;
        break;

    case R_X86_64_COPY:
        rel_type_str = RELA_COPY;
        break;

    case R_X86_64_GLOB_DAT:
        rel_type_str = RELA_GLOB_DAT;
        break;

    case R_X86_64_JUMP_SLOT:
        rel_type_str = RELA_JUMP_SLOT;
        break;

    case R_X86_64_RELATIVE:
        rel_type_str = RELA_RELATIVE;
        break;

    case R_X86_64_GOTPCREL:
        rel_type_str = RELA_GOTPCREL;
        break;

    case R_X86_64_32:
        rel_type_str = RELA_32;
        break;

    case R_X86_64_32S:
        rel_type_str = RELA_32S;
        break;

    case R_X86_64_16:
        rel_type_str = RELA_16;
        break;

    case R_X86_64_PC16:
        rel_type_str = RELA_PC16;
        break;

    case R_X86_64_8:
        rel_type_str = RELA_8;
        break;

    case R_X86_64_PC8:
        rel_type_str = RELA_PC8;
        break;

    case R_X86_64_PC64:
        rel_type_str = RELA_PC64;
        break;

    case R_X86_64_GOTOFF64:
        rel_type_str = RELA_GOTOFF64;
        break;

    case R_X86_64_GOTPC32:
        rel_type_str = RELA_GOTPC32;
        break;

    case R_X86_64_SIZE32:
        rel_type_str = RELA_SIZE32;
        break;

    case R_X86_64_SIZE64:
        rel_type_str = RELA_SIZE64;
        break;

    case R_X86_64_GOTPCRELX:
        rel_type_str = RELA_GOTPCRELX;
        break;

    case R_X86_64_REX_GOTPCRELX:
        rel_type_str = RELA_REX_GOTPCRELX;
        break;

    default:
        rel_type_str = RELA_UNKNOWN;
        break;
    }
    printf("  Relocation type %-18s: %s\n","",rel_type_str);
}

void ereader_print_rela_section(char* section_name, void* elf_data, Elf64_Shdr* elf_sections, Elf64_Shdr* elf_symtab_sh , Elf64_Shdr* elf_rela_sh, Elf64_Rela* elf_rela, Elf64_Sym* elf_rela_sym){
    unsigned char syminfo;
    unsigned char rela_type;
    int total_rela_entries = elf_rela_sh->sh_size/elf_rela_sh->sh_entsize;
    
    // STRTAB HEADER
    Elf64_Shdr* strtab = (Elf64_Shdr*) &elf_sections[elf_symtab_sh->sh_link];
    char* strtabbase = (char*) (elf_data + strtab->sh_offset);

    printf("\n=================================================================\n");
    printf("              RELOCATIONS (%s)\n",section_name);
    printf("=================================================================\n");

    for (int i=0; i < total_rela_entries; i++) {
        syminfo = ELF64_R_SYM(elf_rela[i].r_info);
        rela_type = ELF64_R_TYPE(elf_rela[i].r_info);
        printf("-----------------------------------------------------------------\n");
        printf("  Relocation [%3d]\n",i);
        printf("-----------------------------------------------------------------\n");
        ereader_print_rela_type(rela_type);
        printf("  %-34s: %lx\n","Offset",elf_rela[i].r_offset);
        printf("  %-34s: %d\n","Symbol index",syminfo);
        printf("  %-34s: %lx\n","Symbol value",elf_rela_sym[syminfo].st_value);
        printf("  %-34s: %s\n","Symbol name",strtabbase + elf_rela_sym[syminfo].st_name);
        if(elf_rela[i].r_addend < 0) {
            printf("  %-34s: %ld\n","Addend",elf_rela[i].r_addend);
        } else {
            printf("  %-34s: %lx\n","Addend",elf_rela[i].r_addend);
        }
    }
}

/* ALL */

bool ereader_print_elf(char* filename){
    if(filename == NULL) return false;

    // Get file size
    struct stat file_stat;
    int stat_status = stat(filename,&file_stat);

    if (stat_status == -1) {
        return false;
    }

    void* elf_data = (void*) malloc(file_stat.st_size);

    if(ereader_read_file(filename,file_stat.st_size,elf_data)){
        // ELF HEADER
        ereader_print_elfheader(elf_data);

        // SECTIONS HEADER
        Elf64_Ehdr* elf_header = (Elf64_Ehdr*) elf_data;
        Elf64_Shdr* elf_sections = (Elf64_Shdr*) (elf_data+elf_header->e_shoff);
        ereader_print_elfsections(elf_data,elf_sections,elf_header);
        
        // at .symtab section
        int symtab_index = ereader_find_section_by_type(elf_sections, SHT_SYMTAB, elf_header->e_shnum);
        if (symtab_index != -1) {
            Elf64_Shdr* elf_symtab_sh = (Elf64_Shdr*) &elf_sections[symtab_index];
            Elf64_Sym* elf_symtab_sym = (Elf64_Sym*) (elf_data+elf_symtab_sh->sh_offset);
            ereader_print_symbols(elf_data, elf_header, elf_sections, elf_symtab_sh, elf_symtab_sym);        
        }

        // RELOCATION HEADER

        // at .rela.text section
        int relatext = ereader_find_section_by_name(elf_data,elf_header,elf_sections,".rela.text");
        
        if (relatext != -1) {
            Elf64_Shdr* elf_relatext_sh = (Elf64_Shdr*) &elf_sections[relatext];
            Elf64_Rela* elf_relatext_rela = (Elf64_Rela*) (elf_data + elf_relatext_sh->sh_offset);

            Elf64_Shdr* elf_symtab_sh = (Elf64_Shdr*) &elf_sections[symtab_index];
            Elf64_Sym* elf_symtab_sym = (Elf64_Sym*) (elf_data+elf_symtab_sh->sh_offset);

            ereader_print_rela_section(".rela.text",elf_data, elf_sections, elf_symtab_sh, elf_relatext_sh, elf_relatext_rela, elf_symtab_sym);
        }

        int reladatalocal = ereader_find_section_by_name(elf_data,elf_header,elf_sections,".rela.data.rel.local");

        if (reladatalocal != -1) {
            Elf64_Shdr* elf_reladatalocal_sh = (Elf64_Shdr*) &elf_sections[reladatalocal];
            Elf64_Rela* elf_reladatalocal_rela = (Elf64_Rela*) (elf_data + elf_reladatalocal_sh->sh_offset);

            Elf64_Shdr* elf_symtab_sh = (Elf64_Shdr*) &elf_sections[symtab_index];
            Elf64_Sym* elf_symtab_sym = (Elf64_Sym*) (elf_data+elf_symtab_sh->sh_offset);

            ereader_print_rela_section(".rela.data.rel.local",elf_data, elf_sections, elf_symtab_sh, elf_reladatalocal_sh, elf_reladatalocal_rela, elf_symtab_sym);
        }

        int relaeh_frame = ereader_find_section_by_name(elf_data,elf_header,elf_sections,".rela.eh_frame");

        if (reladatalocal != -1) {
            Elf64_Shdr* elf_relaeh_frame_sh = (Elf64_Shdr*) &elf_sections[relaeh_frame];
            Elf64_Rela* elf_relaeh_frame_rela = (Elf64_Rela*) (elf_data + elf_relaeh_frame_sh->sh_offset);

            Elf64_Shdr* elf_symtab_sh = (Elf64_Shdr*) &elf_sections[symtab_index];
            Elf64_Sym* elf_symtab_sym = (Elf64_Sym*) (elf_data+elf_symtab_sh->sh_offset);

            ereader_print_rela_section(".rela.eh_frame",elf_data, elf_sections, elf_symtab_sh, elf_relaeh_frame_sh, elf_relaeh_frame_rela, elf_symtab_sym);
        }

    }

    ereader_delete_elf_data(elf_data);
}

