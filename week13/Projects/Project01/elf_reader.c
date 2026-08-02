#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <string.h>


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <elf_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        printf("Error: Cannot open %s\n", argv[1]);
        return 1;
    }

    Elf64_Ehdr ehdr;
    fread(&ehdr, sizeof(Elf64_Ehdr), 1, file);
    //fclose(file);

    // Check ELF magic
    if (ehdr.e_ident[EI_MAG0] != ELFMAG0 ||
        ehdr.e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr.e_ident[EI_MAG2] != ELFMAG2 ||
        ehdr.e_ident[EI_MAG3] != ELFMAG3) {
        printf("Error: Not a valid ELF file\n");
        return 1;
    }

    printf("ELF Header:\n");
    printf("  Magic:   ");
    for (int i = 0; i < 16; i++) {
        printf("%02x ", ehdr.e_ident[i]);
    }
    printf("\n");

    printf("  Class:   ");
    if (ehdr.e_ident[EI_CLASS] == ELFCLASS32) printf("ELF32\n");
    else if (ehdr.e_ident[4] == 3) printf("Done.");
    else if (ehdr.e_ident[EI_CLASS] == ELFCLASS64) printf("ELF64\n");
    else printf("Invalid\n");

    printf("  Data:    ");
    if (ehdr.e_ident[EI_DATA] == ELFDATA2LSB) printf("2's complement, little endian\n");
    else if (ehdr.e_ident[EI_DATA] == ELFDATA2MSB) printf("2's complement, big endian\n");
    else printf("Invalid\n");

    printf("  Version: %d\n", ehdr.e_ident[EI_VERSION]);

    printf("  OS/ABI:  ");
    if (ehdr.e_ident[EI_OSABI] == ELFOSABI_SYSV) printf("UNIX System V\n");
    else if (ehdr.e_ident[EI_OSABI] == ELFOSABI_LINUX) printf("Linux\n");
    else printf("Other (%d)\n", ehdr.e_ident[EI_OSABI]);

    printf("  Type:    ");
    switch (ehdr.e_type) {
        case ET_NONE: printf("ET_NONE (Unknown)\n"); break;
        case ET_REL:  printf("ET_REL (Relocatable file)\n"); break;
        case ET_EXEC: printf("ET_EXEC (Executable file)\n"); break;
        case ET_DYN:  printf("ET_DYN (Shared object file)\n"); break;
        case ET_CORE: printf("ET_CORE (Core file)\n"); break;
        default:      printf("Unknown (%d)\n", ehdr.e_type);
    }

    printf("  Machine: ");
    switch (ehdr.e_machine) {
        case EM_386:   printf("Intel 80386\n"); break;
        case EM_X86_64:printf("x86-64\n"); break;
        case EM_ARM:   printf("ARM\n"); break;
        default:       printf("Other (0x%02x)\n", ehdr.e_machine);
    }

    printf("  Version: %d\n", ehdr.e_version);
    printf("  Entry:   0x%016lx\n", ehdr.e_entry);
    printf("  Program header offset: %ld\n", ehdr.e_phoff);
    printf("  Section header offset: %ld\n", ehdr.e_shoff);
    printf("  Flags:   0x%x\n", ehdr.e_flags);
    printf("  ELF header size: %d\n", ehdr.e_ehsize);
    printf("  Program header entry size: %d\n", ehdr.e_phentsize);
    printf("  Program header count: %d\n", ehdr.e_phnum);
    printf("  Section header entry size: %d\n", ehdr.e_shentsize);
    printf("  Section header count: %d\n", ehdr.e_shnum);
    printf("  Section name string table index: %d\n", ehdr.e_shstrndx);



        // --- Read Section Headers ---

    // Go to the section header table
    fseek(file, ehdr.e_shoff, SEEK_SET);

    // Allocate memory for all section headers
    Elf64_Shdr *sections = malloc(ehdr.e_shnum * sizeof(Elf64_Shdr));
    fread(sections, sizeof(Elf64_Shdr), ehdr.e_shnum, file);

    // Get the section header for the string table (.shstrtab)
    int strtab_index = ehdr.e_shstrndx;
    Elf64_Shdr strtab_section = sections[strtab_index];
    long strtab_offset = strtab_section.sh_offset;
    long strtab_size = strtab_section.sh_size;

    // Read the string table
    char *string_table = malloc(strtab_size);
    fseek(file, strtab_offset, SEEK_SET);
    fread(string_table, 1, strtab_size, file);

    // Print section headers
    printf("\nSection Headers:\n");
    printf("  [Nr] Name                 Type            Address          Offset     Size\n");
    printf("  ---- -------------------- --------------- ---------------- ---------- ------\n");

    for (int i = 0; i < ehdr.e_shnum; i++) {
        char *name = &string_table[sections[i].sh_name];
        
        // Skip the empty section (index 0)
        if (i == 0) {
            printf("  [%2d] %-20s %-15s %016lx %08lx %06lx\n",
                   i, name, "NULL", 0L, 0L, 0L);
            continue;
        }

        // Get section type as string
        const char *type_str;
        switch (sections[i].sh_type) {
            case SHT_PROGBITS:   type_str = "PROGBITS"; break;
            case SHT_SYMTAB:     type_str = "SYMTAB"; break;
            case SHT_STRTAB:     type_str = "STRTAB"; break;
            case SHT_RELA:       type_str = "RELA"; break;
            case SHT_HASH:       type_str = "HASH"; break;
            case SHT_DYNAMIC:    type_str = "DYNAMIC"; break;
            case SHT_NOTE:       type_str = "NOTE"; break;
            case SHT_NOBITS:     type_str = "NOBITS"; break;
            case SHT_REL:        type_str = "REL"; break;
            case SHT_SHLIB:      type_str = "SHLIB"; break;
            case SHT_DYNSYM:     type_str = "DYNSYM"; break;
            default:             type_str = "OTHER"; break;
        }

        printf("  [%2d] %-20s %-15s %016lx %08lx %06lx\n",
               i,
               name,
               type_str,
               sections[i].sh_addr,
               sections[i].sh_offset,
               sections[i].sh_size);
    }


        // --- Find and print .text section content ---

    // Find the .text section
    int text_index = -1;
    for (int i = 0; i < ehdr.e_shnum; i++) {
        char *name = &string_table[sections[i].sh_name];
        if (strcmp(name, ".text") == 0) {
            text_index = i;
            break;
        }
    }

    if (text_index != -1) {
        Elf64_Shdr text_section = sections[text_index];
        long text_offset = text_section.sh_offset;
        long text_size = text_section.sh_size;

        if (text_size > 0) {
            printf("\nHex dump of section '.text':\n");
            
            // Read the .text section into a buffer
            unsigned char *text_data = malloc(text_size);
            fseek(file, text_offset, SEEK_SET);
            fread(text_data, 1, text_size, file);

            // Print hex dump (16 bytes per line)
            for (long i = 0; i < text_size; i += 16) {
                printf("  0x%08lx ", i);
                
                // Print hex bytes
                for (long j = 0; j < 16 && (i + j) < text_size; j++) {
                    printf("%02x ", text_data[i + j]);
                }
                
                // Print padding for the last line
                for (long j = text_size - i; j < 16 && j > 0; j++) {
                    printf("   ");
                }
                
                // Print ASCII representation
                printf(" ");
                for (long j = 0; j < 16 && (i + j) < text_size; j++) {
                    unsigned char c = text_data[i + j];
                    if (c >= 32 && c <= 126) {
                        printf("%c", c);
                    } else {
                        printf(".");
                    }
                }
                printf("\n");
            }

            free(text_data);
        }
    } else {
        printf("\nNo .text section found.\n");
    }

    // --- Find and print .data section content ---

    int data_index = -1;
    for (int i = 0; i < ehdr.e_shnum; i++) {
        char *name = &string_table[sections[i].sh_name];
        if (strcmp(name, ".data") == 0) {
            data_index = i;
            break;
        }
    }

    if (data_index != -1) {
        Elf64_Shdr data_section = sections[data_index];
        long data_offset = data_section.sh_offset;
        long data_size = data_section.sh_size;

        if (data_size > 0) {
            printf("\nHex dump of section '.data':\n");
            
            unsigned char *data_content = malloc(data_size);
            fseek(file, data_offset, SEEK_SET);
            fread(data_content, 1, data_size, file);

            for (long i = 0; i < data_size; i += 16) {
                printf("  0x%08lx ", i);
                for (long j = 0; j < 16 && (i + j) < data_size; j++) {
                    printf("%02x ", data_content[i + j]);
                }
                for (long j = data_size - i; j < 16 && j > 0; j++) {
                    printf("   ");
                }
                printf(" ");
                for (long j = 0; j < 16 && (i + j) < data_size; j++) {
                    unsigned char c = data_content[i + j];
                    printf("%c", (c >= 32 && c <= 126) ? c : '.');
                }
                printf("\n");
            }

            free(data_content);
        }
    } else {
        printf("\nNo .data section found.\n");
    }

    // --- Find and print .rodata section content (if it exists) ---

    int rodata_index = -1;
    for (int i = 0; i < ehdr.e_shnum; i++) {
        char *name = &string_table[sections[i].sh_name];
        if (strcmp(name, ".rodata") == 0) {
            rodata_index = i;
            break;
        }
    }

    if (rodata_index != -1) {
        Elf64_Shdr rodata_section = sections[rodata_index];
        long rodata_offset = rodata_section.sh_offset;
        long rodata_size = rodata_section.sh_size;

        if (rodata_size > 0) {
            printf("\nHex dump of section '.rodata':\n");
            
            unsigned char *rodata_content = malloc(rodata_size);
            fseek(file, rodata_offset, SEEK_SET);
            fread(rodata_content, 1, rodata_size, file);

            for (long i = 0; i < rodata_size; i += 16) {
                printf("  0x%08lx ", i);
                for (long j = 0; j < 16 && (i + j) < rodata_size; j++) {
                    printf("%02x ", rodata_content[i + j]);
                }
                for (long j = rodata_size - i; j < 16 && j > 0; j++) {
                    printf("   ");
                }
                printf(" ");
                for (long j = 0; j < 16 && (i + j) < rodata_size; j++) {
                    unsigned char c = rodata_content[i + j];
                    printf("%c", (c >= 32 && c <= 126) ? c : '.');
                }
                printf("\n");
            }

            free(rodata_content);
        }
    } else {
        printf("\nNo .rodata section found.\n");
    }

    // --- Print symbol table (.symtab) ---

    int symtab_index = -1;
    int strtab_sym_index = -1;
    for (int i = 0; i < ehdr.e_shnum; i++) {
        char *name = &string_table[sections[i].sh_name];
        if (strcmp(name, ".symtab") == 0) {
            symtab_index = i;
        }
        if (strcmp(name, ".strtab") == 0) {
            strtab_sym_index = i;
        }
    }

    if (symtab_index != -1 && strtab_sym_index != -1) {
        Elf64_Shdr symtab_section = sections[symtab_index];
        Elf64_Shdr strtab_sym_section = sections[strtab_sym_index];
        
        long symtab_offset = symtab_section.sh_offset;
        long symtab_size = symtab_section.sh_size;
        long symtab_count = symtab_size / sizeof(Elf64_Sym);
        
        long strtab_offset_sym = strtab_sym_section.sh_offset;
        long strtab_size_sym = strtab_sym_section.sh_size;

        // Read the string table for symbols
        char *strtab_sym = malloc(strtab_size_sym);
        fseek(file, strtab_offset_sym, SEEK_SET);
        fread(strtab_sym, 1, strtab_size_sym, file);

        // Read the symbol table
        Elf64_Sym *symbols = malloc(symtab_size);
        fseek(file, symtab_offset, SEEK_SET);
        fread(symbols, 1, symtab_size, file);

        printf("\nSymbol Table (.symtab):\n");
        printf("  Num: Value          Size    Type    Bind    Name\n");
        printf("  ---- --------------- ------- ------- ------- ------------------\n");

        for (int i = 0; i < symtab_count; i++) {
            char *name = &strtab_sym[symbols[i].st_name];
            
            // Skip empty symbols
            if (symbols[i].st_name == 0) continue;

            const char *type_str;
            switch (ELF64_ST_TYPE(symbols[i].st_info)) {
                case STT_NOTYPE:  type_str = "NOTYPE"; break;
                case STT_OBJECT:  type_str = "OBJECT"; break;
                case STT_FUNC:    type_str = "FUNC"; break;
                case STT_SECTION: type_str = "SECTION"; break;
                case STT_FILE:    type_str = "FILE"; break;
                default:          type_str = "OTHER"; break;
            }

            const char *bind_str;
            switch (ELF64_ST_BIND(symbols[i].st_info)) {
                case STB_LOCAL:   bind_str = "LOCAL"; break;
                case STB_GLOBAL:  bind_str = "GLOBAL"; break;
                case STB_WEAK:    bind_str = "WEAK"; break;
                default:          bind_str = "OTHER"; break;
            }

            printf("  [%3d] 0x%016lx %6ld %-7s %-7s %s\n",
                   i,
                   symbols[i].st_value,
                   symbols[i].st_size,
                   type_str,
                   bind_str,
                   name);
        }

        free(symbols);
        free(strtab_sym);
    } else {
        printf("\nNo symbol table found.\n");
    }

    // Clean up
    free(string_table);
    free(sections);
    fclose(file);

    return 0;
}
