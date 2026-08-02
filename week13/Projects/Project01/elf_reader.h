#ifndef __ELF_READER_H__
#define __ELF_READER_H__

#include <stdio.h>
#include <elf.h>

typedef struct {
    size_t file_size;
    FILE *file;

    Elf64_Ehdr header;
    Elf64_Phdr *program_headers;
    Elf64_Shdr *section_headers;
    Elf64_Sym *symbols;

    char *section_names;   
    char *symbol_names;    

    size_t program_header_count;
    size_t section_count;
    size_t symbol_count;

} elf_file_t;


elf_file_t *elf_file_create(const char *file);

void elf_file_delete(elf_file_t *elf);

void elf_file_print_header(const elf_file_t *elf);

void elf_file_print_sections(const elf_file_t *elf);

void elf_file_print_symbols(const elf_file_t *elf);

void elf_file_print_program_headers(const elf_file_t *elf);

#endif