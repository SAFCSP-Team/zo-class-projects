#ifndef __EREAD_H__
#define __EREAD_H__

#include <stdio.h>
#include <elf.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#define BIT_64 "64-bit"
#define BIT_32 "32-bit"
#define NONE_CLASS "invalid"

#define ELF_LITTLE_ENDIAN "Two's complement, little-endian."
#define ELF_BIG_ENDIAN "Two's complement, big-endian."
#define UNKNOWN_DATA "Unknown data format."

#define ELF_VERSION_NONE "Invalid version."
#define ELF_VERSION_CURRENT "1 (Current)"

#define OSABI_UNIX "UNIX system V ABI"
#define OSABI_HPUX "HP-UX ABI"
#define OSABI_NETBSD "NetBSD ABI"
#define OSABI_LINUX "Linux ABI"
#define OSABI_SOLARIS "Solaris ABI"
#define OSABI_IRIX "IRIX ABI"
#define OSABI_FREEBSD "FreeBSD ABI"
#define OSABI_TRU64 "TRU64 UNIX ABI"
#define OSABI_ARM "ARM architecture ABI"
#define OSABI_STANDALONE "Stand-alone (embedded) ABI"

#define MACH_NONE "No machine."
#define MACH_M32 "AT&T WE 32100."
#define MACH_SPARC "SPARC."
#define MACH_386 "Intel 80386."
#define MACH_68K "Motorola 68000."
#define MACH_88K "Motorola 88000."
#define MACH_860 "Intel 80860."
#define MACH_MIPS "MIPS R3000."
#define MACH_S370 "IBM System/370."
#define MACH_PARISC "HP PA-RISC."
#define MACH_PPC "PowerPC."
#define MACH_PPC64 "PowerPC 64-bit."
#define MACH_S390 "IBM S/390."
#define MACH_ARM "ARM."
#define MACH_X86_64 "Advanced Micro Devices X86-64."
#define MACH_AARCH64 "AArch64."

#define SEC_NULL "SHT_NULL"
#define SEC_PROGBITS "SHT_PROGBITS"
#define SEC_SYMTAB "SHT_SYMTAB"
#define SEC_STRTAB "SHT_STRTAB"
#define SEC_RELA "SHT_RELA"
#define SEC_HASH "SHT_HASH"
#define SEC_DYNAMIC "SHT_DYNAMIC"
#define SEC_NOTE "SHT_NOTE"
#define SEC_NOBITS "SHT_NOBITS"
#define SEC_REL "SHT_REL"
#define SEC_SHLIB "SHT_SHLIB"
#define SEC_DYNSYM "SHT_DYNSYM"
#define SEC_LOPROC "SHT_LOPROC"
#define SEC_HIPROC "SHT_HIPROC"
#define SEC_LOUSER "SHT_LOUSER"
#define SEC_HIUSER "SHT_HIUSER"
#define SEC_UNKNOWN "Unknown type."

#define STT_NOTYPE_SYM "STT_NOTYPE"
#define STT_OBJECT_SYM "STT_OBJECT"
#define STT_FUNC_SYM "STT_FUNC"
#define STT_SECTION_SYM "STT_SECTION"
#define STT_FILE_SYM "STT_FILE"
#define STT_LOPROC_SYM "STT_LOPROC"
#define STT_HIPROC_SYM "STT_HIPROC"
#define STT_UNKNOWN "Unknown symbol type."

#define STB_LOCAL_SYM "STB_LOCAL"
#define STB_GLOBAL_SYM "STB_GLOBAL"
#define STB_WEAK_SYM "STB_WEAK"
#define STB_LOPROC_SYM "STB_LOPROC"
#define STB_HIPROC_SYM "STB_HIPROC"
#define STB_UNKNOWN "Unknown binding."

#define STV_DEFAULT_SYM "STV_DEFAULT"
#define STV_INTERNAL_SYM "STV_INTERNAL"
#define STV_HIDDEN_SYM "STV_HIDDEN"
#define STV_PROTECTED_SYM "STV_PROTECTED"
#define STV_UNKNOWN "Unknown visibility."

#define OB_UNKNOWN "Unknown type."
#define OB_REL "Relocatable Object File."
#define OB_EXEC "Executable Object File."
#define OB_SHR "Shared Object File."
#define OB_CORE "Core Object File."

#define RELA_NONE "R_X86_64_NONE"
#define RELA_64 "R_X86_64_64"
#define RELA_PC32 "R_X86_64_PC32"
#define RELA_GOT32 "R_X86_64_GOT32"
#define RELA_PLT32 "R_X86_64_PLT32"
#define RELA_COPY "R_X86_64_COPY"
#define RELA_GLOB_DAT "R_X86_64_GLOB_DAT"
#define RELA_JUMP_SLOT "R_X86_64_JUMP_SLOT"
#define RELA_RELATIVE "R_X86_64_RELATIVE"
#define RELA_GOTPCREL "R_X86_64_GOTPCREL"
#define RELA_32 "R_X86_64_32"
#define RELA_32S "R_X86_64_32S"
#define RELA_16 "R_X86_64_16"
#define RELA_PC16 "R_X86_64_PC16"
#define RELA_8 "R_X86_64_8"
#define RELA_PC8 "R_X86_64_PC8"
#define RELA_PC64 "R_X86_64_PC64"
#define RELA_GOTOFF64 "R_X86_64_GOTOFF64"
#define RELA_GOTPC32 "R_X86_64_GOTPC32"
#define RELA_SIZE32 "R_X86_64_SIZE32"
#define RELA_SIZE64 "R_X86_64_SIZE64"
#define RELA_GOTPCRELX "R_X86_64_GOTPCRELX"
#define RELA_REX_GOTPCRELX "R_X86_64_REX_GOTPCRELX"
#define RELA_UNKNOWN "Unknown relocation type."

bool ereader_read_file(char* ,size_t, void*);
bool ereader_delete_elf_data(void*);
bool ereader_print_elf(char*);
int ereader_find_section_by_type(Elf64_Shdr*,int,int);
int ereader_find_section_by_name(void*,Elf64_Ehdr*,Elf64_Shdr*,char*);

/* ELF HEADER */

void ereader_print_elfheader_ident_magic(Elf64_Ehdr* elf_header);
void ereader_print_elfheader_ident_class(Elf64_Ehdr* elf_header);
void ereader_print_elfheader_ident_endian(Elf64_Ehdr* elf_header);
void ereader_print_elfheader_ident_version(Elf64_Ehdr* elf_header);
void ereader_print_elfheader_ident_abi_type(Elf64_Ehdr* elf_header);
void ereader_print_elfheader_ident_abi_version(Elf64_Ehdr* elf_header);

void ereader_print_elfheader_type(Elf64_Ehdr* elf_header);
void ereader_print_elfheader_machine(Elf64_Ehdr* elf_header);
void ereader_print_elfheader_file_version(Elf64_Ehdr* elf_header);

void ereader_print_elfheader_entry_vaddress(Elf64_Ehdr* elf_header);
void ereader_print_elfheader_prhd_offset(Elf64_Ehdr* elf_header);
void ereader_print_elfheader_sechd_offset(Elf64_Ehdr* elf_header);

void ereader_print_elfheader_machine_flags(Elf64_Ehdr* elf_header);
void ereader_print_elfheader_size(Elf64_Ehdr* elf_header);

void ereader_print_elfheader_prhd_entry_size(Elf64_Ehdr* elf_header);
void ereader_print_elfheader_prhd_entries_size(Elf64_Ehdr* elf_header);

void ereader_print_elfheader_sechd_entry_size(Elf64_Ehdr* elf_header);
void ereader_print_elfheader_sechd_entries_size(Elf64_Ehdr* elf_header);

void ereader_print_elfheader_sechd_index(Elf64_Ehdr* elf_header);

void ereader_print_elfheader(Elf64_Ehdr* elf_header);

/* SECTIONS HEADER */

void ereader_print_section_type(Elf64_Shdr* elf_sec);
void ereader_print_section_flags(Elf64_Shdr* elf_sec);
void ereader_print_sections(void* elf_data, Elf64_Shdr* elf_sec, Elf64_Ehdr* elf_header);

/* SYMBOL TABLE */

void ereader_print_symbol_type(Elf64_Sym* elf_sym);
void erader_print_symbol_vis(Elf64_Sym* elf_sym);
void ereader_print_symbols(void* elf_data, Elf64_Ehdr* elf_header, Elf64_Shdr* elf_sections, Elf64_Shdr* elf_sym_sh, Elf64_Sym* elf_sym);

/* RELOCATIONS SECTIONS */

void ereader_print_rela_type(unsigned char);

void ereader_print_rela_section(char* section_name, void* elf_data, Elf64_Shdr* elf_sections,  Elf64_Shdr* elf_symtab_sh, Elf64_Shdr* elf_rela_sh, Elf64_Rela* elf_rela, Elf64_Sym* elf_rela_sym);
void ereader_print_rel_section(char* section_name, void* elf_data, Elf64_Shdr* elf_sections,  Elf64_Shdr* elf_symtab_sh, Elf64_Shdr* elf_rela_sh, Elf64_Rel* elf_rel, Elf64_Sym* elf_rela_sym);


#endif