#ifndef __ELF_PARSER_H__
#define __ELF_PARSER_H__

#include <elf.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct elf_file_obj_t elf_file_obj_t;

struct elf_file_obj_t {
    uint8_t*    data;
    size_t      size;
    Elf64_Ehdr  header;
    Elf64_Shdr* sections;
    size_t      sections_count;
    Elf64_Sym*  symbols;
    size_t      symbols_count;

    uint8_t*    symbols_str_name_base;
    uint8_t*    sections_str_name_base;
};

bool read_file(const char* path, elf_file_obj_t* elf_obj);

bool read_header(elf_file_obj_t* elf_obj);
bool read_sections(elf_file_obj_t* elf_obj);
bool read_symbols(elf_file_obj_t* elf_obj);

// decoders
const char* header_class_str(uint8_t value);
const char* header_data_str(uint8_t value);
const char* header_version_str(uint32_t value);
const char* header_osabi_str(uint8_t value);
const char* header_type_str(uint16_t value);
const char* header_machine_str(uint16_t value);

const char* section_type_str(Elf64_Word type);
void        section_flags_str(uint64_t flags, char* buf);

const char* symbol_type_str(uint8_t val);
const char* symbol_bind_str(uint8_t val);
const char* symbol_visibility_str(uint8_t val);
const char* symbol_shndx_str(uint16_t val);

const char* rela_type_str(uint32_t type);

// dump
void print_header(const elf_file_obj_t* elf_obj);
void print_sections(const elf_file_obj_t* elf_obj, const bool dump_content);
void print_symbols(const elf_file_obj_t* elf_obj);
void print_relocations(const elf_file_obj_t* elf_obj);

// helpers

const Elf64_Shdr* get_section_by_type(const elf_file_obj_t* elf_obj, const uint32_t type);
const char*       get_symbol_name(const elf_file_obj_t* elf_obj, const Elf64_Sym* sym);
#endif