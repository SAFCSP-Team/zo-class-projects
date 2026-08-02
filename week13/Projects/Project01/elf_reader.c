#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

#include "elf_reader.h"

static long file_size_of(FILE *fp)
{
    long cur = ftell(fp);
    if (cur < 0) return -1;

    if (fseek(fp, 0, SEEK_END) != 0) return -1;
    long sz = ftell(fp);

    fseek(fp, cur, SEEK_SET); 
    return sz;
}

elf_file_t *elf_file_create(const char *file)
{
    FILE *fp = fopen(file, "rb");

    if (!fp) {
        printf("file not found\n");
        return NULL;
    }

    elf_file_t *elf = malloc(sizeof(elf_file_t));

    if (!elf) {
        printf("memory allocation failed\n");
        fclose(fp);
        return NULL;
    }

    memset(elf, 0, sizeof(*elf));
    elf->file = fp;

    long sz = file_size_of(fp);
    if (sz < 0) {
        printf("could not determine file size\n");
        fclose(fp);
        free(elf);
        return NULL;
    }
    elf->file_size = (size_t)sz;

    if (fread(&elf->header, sizeof(Elf64_Ehdr), 1, fp) != 1) {
        printf("failed to read ELF header\n");
        fclose(fp);
        free(elf);
        return NULL;
    }
    if (memcmp(elf->header.e_ident, ELFMAG, SELFMAG) != 0) {
        printf("not a valid ELF file\n");
        fclose(fp);
        free(elf);
        return NULL;
    }

    if (elf->header.e_ident[EI_CLASS] != ELFCLASS64) {
        printf("only ELF64 files are supported\n");
        fclose(fp);
        free(elf);
        return NULL;
    }

    elf->program_header_count = elf->header.e_phnum;
    if (elf->program_header_count > 0) {
        elf->program_headers = malloc(elf->program_header_count * sizeof(Elf64_Phdr));
        if (!elf->program_headers) {
            printf("memory allocation failed (program headers)\n");
            elf_file_delete(elf);
            return NULL;
        }

        if (fseek(fp, elf->header.e_phoff, SEEK_SET) != 0 ||
            fread(elf->program_headers, sizeof(Elf64_Phdr), elf->program_header_count, fp)
                != elf->program_header_count) {
            printf("failed to read program headers\n");
            elf_file_delete(elf);
            return NULL;
        }
    }

    elf->section_count = elf->header.e_shnum;
    if (elf->section_count > 0) {
        elf->section_headers = malloc(elf->section_count * sizeof(Elf64_Shdr));
        if (!elf->section_headers) {
            printf("memory allocation failed (section headers)\n");
            elf_file_delete(elf);
            return NULL;
        }

        if (fseek(fp, elf->header.e_shoff, SEEK_SET) != 0 ||
            fread(elf->section_headers, sizeof(Elf64_Shdr), elf->section_count, fp)
                != elf->section_count) {
            printf("failed to read section headers\n");
            elf_file_delete(elf);
            return NULL;
        }
    }

    if (elf->section_headers && elf->header.e_shstrndx < elf->section_count) {
        Elf64_Shdr *shstrtab = &elf->section_headers[elf->header.e_shstrndx];

        elf->section_names = malloc(shstrtab->sh_size);
        if (!elf->section_names) {
            printf("memory allocation failed (section names)\n");
            elf_file_delete(elf);
            return NULL;
        }

        if (fseek(fp, shstrtab->sh_offset, SEEK_SET) != 0 ||
            fread(elf->section_names, 1, shstrtab->sh_size, fp) != shstrtab->sh_size) {
            printf("failed to read section name string table\n");
            elf_file_delete(elf);
            return NULL;
        }
    }

    
    if (elf->section_headers) {
        for (size_t i = 0; i < elf->section_count; i++) {
            Elf64_Shdr *sh = &elf->section_headers[i];

            if (sh->sh_type == SHT_SYMTAB || sh->sh_type == SHT_DYNSYM) {
                elf->symbol_count = sh->sh_size / sizeof(Elf64_Sym);

                elf->symbols = malloc(elf->symbol_count * sizeof(Elf64_Sym));
                if (!elf->symbols) {
                    printf("memory allocation failed (symbols)\n");
                    elf_file_delete(elf);
                    return NULL;
                }

                if (fseek(fp, sh->sh_offset, SEEK_SET) != 0 ||
                    fread(elf->symbols, sizeof(Elf64_Sym), elf->symbol_count, fp)
                        != elf->symbol_count) {
                    printf("failed to read symbol table\n");
                    elf_file_delete(elf);
                    return NULL;
                }

               
                if (sh->sh_link < elf->section_count) {
                    Elf64_Shdr *strtab = &elf->section_headers[sh->sh_link];

                    elf->symbol_names = malloc(strtab->sh_size);
                    if (!elf->symbol_names) {
                        printf("memory allocation failed (symbol names)\n");
                        elf_file_delete(elf);
                        return NULL;
                    }

                    if (fseek(fp, strtab->sh_offset, SEEK_SET) != 0 ||
                        fread(elf->symbol_names, 1, strtab->sh_size, fp) != strtab->sh_size) {
                        printf("failed to read symbol name string table\n");
                        elf_file_delete(elf);
                        return NULL;
                    }
                }

                break;
            }
        }
    }

    return elf;
}

void elf_file_delete(elf_file_t *elf)
{
    if (!elf) return;

    if (elf->file) {
        fclose(elf->file);
    }

    free(elf->program_headers);
    free(elf->section_headers);
    free(elf->symbols);
    free(elf->section_names);
    free(elf->symbol_names);

    free(elf);
}


void elf_file_print_header(const elf_file_t *elf)
{
    const Elf64_Ehdr *header = &elf->header;

    printf("ELF Header:\n");
    printf("  Magic:  ");
    for (int i = 0; i < EI_NIDENT; i++) {
        printf("%02x ", header->e_ident[i]);
    }
    printf("\n");

    printf("  Class:                             ");
    switch (header->e_ident[EI_CLASS]) {
        case ELFCLASS32: printf("ELF32\n"); break;
        case ELFCLASS64: printf("ELF64\n"); break;
        default:         printf("Invalid class\n"); break;
    }

    printf(" Data:");
    switch (header->e_ident[EI_DATA]) {
        case ELFDATA2LSB: printf("2's complement, little endian\n"); break;
        case ELFDATA2MSB: printf("2's complement, big endian\n"); break;
        default:          printf("Invalid data encoding\n"); break;
    }

    printf("Version%d", header->e_ident[EI_VERSION]);
    if (header->e_ident[EI_VERSION] == EV_CURRENT) printf(" (current)");
    printf("\n");

    printf(" OS/ABI:");
    switch (header->e_ident[EI_OSABI]) {
        case ELFOSABI_SYSV:    printf("UNIX - System V\n"); break;
        case ELFOSABI_LINUX:   printf("UNIX - Linux\n"); break;
        case ELFOSABI_FREEBSD: printf("UNIX - FreeBSD\n"); break;
        default: printf("Unknown (%d)\n", header->e_ident[EI_OSABI]); break;
    }

    printf("ABI Version:%d\n", header->e_ident[EI_ABIVERSION]);

    printf("Type:");
    switch (header->e_type) {
        case ET_NONE: printf("NONE (No file type)\n"); break;
        case ET_REL:  printf("REL (Relocatable file)\n"); break;
        case ET_EXEC: printf("EXEC (Executable file)\n"); break;
        case ET_DYN:  printf("DYN (Position-Independent Executable file)\n"); break;
        case ET_CORE: printf("CORE (Core file)\n"); break;
        default:      printf("Unknown\n"); break;
    }

    printf("  Machine:                           ");
    switch (header->e_machine) {
        case EM_X86_64:  printf("Advanced Micro Devices X86-64\n"); break;
        case EM_386:     printf("Intel 80386\n"); break;
        case EM_ARM:     printf("ARM\n"); break;
        case EM_AARCH64: printf("AArch64\n"); break;
        default: printf("Unknown (%d)\n", header->e_machine); break;
    }

    printf("Version 0x%x\n", header->e_version);
    printf("Entry point address:0x%lx\n", (unsigned long)header->e_entry);
    printf("Start of program headers:%lu (bytes into file)\n", (unsigned long)header->e_phoff);
    printf("Start of section headers:%lu (bytes into file)\n", (unsigned long)header->e_shoff);
    printf("Flags:0x%x\n", header->e_flags);
    printf("Size of this header:%u (bytes)\n", header->e_ehsize);
    printf("Size of program headers:%u (bytes)\n", header->e_phentsize);
    printf("Number of program headers:%u\n", header->e_phnum);
    printf("Size of section headers:%u (bytes)\n", header->e_shentsize);
    printf("Number of section headers:%u\n", header->e_shnum);
    printf("Section header string table index: %u\n", header->e_shstrndx);
}

static const char *section_type_name(Elf64_Word type)
{
    switch (type) {
        case SHT_NULL:     return "NULL";
        case SHT_PROGBITS: return "PROGBITS";
        case SHT_SYMTAB:   return "SYMTAB";
        case SHT_STRTAB:   return "STRTAB";
        case SHT_RELA:     return "RELA";
        case SHT_HASH:     return "HASH";
        case SHT_DYNAMIC:  return "DYNAMIC";
        case SHT_NOTE:     return "NOTE";
        case SHT_NOBITS:   return "NOBITS";
        case SHT_REL:      return "REL";
        case SHT_DYNSYM:   return "DYNSYM";
        default:           return "UNKNOWN";
    }
}

void elf_file_print_sections(const elf_file_t *elf)
{
    if (!elf->section_headers) {
        printf("No section headers found.\n");
        return;
    }

    printf("Section Headers:\n");
    printf("  [Nr] %-20s %-12s %-16s %-8s %s\n",
           "Name", "Type", "Address", "Size", "");

    for (size_t i = 0; i < elf->section_count; i++) {
        const Elf64_Shdr *sh = &elf->section_headers[i];
        const char *name = elf->section_names ? (elf->section_names + sh->sh_name) : "";

        printf("  [%2zu] %-20s %-12s %016lx %08lx\n",
               i, name, section_type_name(sh->sh_type),
               (unsigned long)sh->sh_addr, (unsigned long)sh->sh_size);
    }
}

void elf_file_print_symbols(const elf_file_t *elf)
{
    if (!elf->symbols) {
        printf("No symbol table found.\n");
        return;
    }

    printf("Symbol table (%zu entries):\n", elf->symbol_count);
    printf("  %-6s %-16s %-8s %-8s %s\n", "Num", "Value", "Size", "Type", "Name");

    for (size_t i = 0; i < elf->symbol_count; i++) {
        const Elf64_Sym *sym = &elf->symbols[i];
        const char *name = elf->symbol_names ? (elf->symbol_names + sym->st_name) : "";

        printf("  %-6zu %016lx %-8lu %-8u %s\n",
               i, (unsigned long)sym->st_value, (unsigned long)sym->st_size,
               ELF64_ST_TYPE(sym->st_info), name);
    }
}

static const char *segment_type_name(Elf64_Word type)
{
    switch (type) {
        case PT_NULL:    return "NULL";
        case PT_LOAD:    return "LOAD";
        case PT_DYNAMIC: return "DYNAMIC";
        case PT_INTERP:  return "INTERP";
        case PT_NOTE:    return "NOTE";
        case PT_PHDR:    return "PHDR";
        case PT_TLS:     return "TLS";
        default:         return "UNKNOWN";
    }
}

void elf_file_print_program_headers(const elf_file_t *elf)
{
    if (!elf->program_headers) {
        printf("No program headers found.\n");
        return;
    }

    printf("Program Headers:\n");
    printf("  %-10s %-16s %-16s %-10s %s\n", "Type", "Offset", "VirtAddr", "FileSize", "Flags");

    for (size_t i = 0; i < elf->program_header_count; i++) {
        const Elf64_Phdr *ph = &elf->program_headers[i];

        char flags[4] = "---";
        if (ph->p_flags & PF_R) flags[0] = 'R';
        if (ph->p_flags & PF_W) flags[1] = 'W';
        if (ph->p_flags & PF_X) flags[2] = 'E';

        printf("  %-10s 0x%014lx 0x%014lx 0x%08lx %s\n",
               segment_type_name(ph->p_type),
               (unsigned long)ph->p_offset,
               (unsigned long)ph->p_vaddr,
               (unsigned long)ph->p_filesz,
               flags);
    }
}