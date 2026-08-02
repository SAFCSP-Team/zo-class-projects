#include "../include/elf_parser.h"

// header
const char* header_class_str(uint8_t value) {
    switch (value) {
        case ELFCLASS32: return "ELF32";
        case ELFCLASS64: return "ELF64";
        default: return "invalid";
    }
}

const char* header_data_str(uint8_t value) {
    switch (value) {
        case ELFDATA2LSB: return "Two's complement, little-endian";
        case ELFDATA2MSB: return "Two's complement, big-endian";
        default: return "unknown";
    }
}

const char* header_version_str(uint32_t value) {
    switch (value) {
        case EV_NONE: return "invalid";
        case EV_CURRENT: return "current";
        default: return "unknown";
    }
}

const char* header_osabi_str(uint8_t value) {
    switch (value) {
        case ELFOSABI_SYSV: return "UNIX - System V";
        case ELFOSABI_HPUX: return "UNIX - HP-UX";
        case ELFOSABI_NETBSD: return "UNIX - NetBSD";
        case ELFOSABI_GNU: return "UNIX - GNU";
        case ELFOSABI_SOLARIS: return "UNIX - Solaris";
        case ELFOSABI_AIX: return "UNIX - AIX";
        case ELFOSABI_IRIX: return "UNIX - IRIX";
        case ELFOSABI_FREEBSD: return "UNIX - FreeBSD";
        case ELFOSABI_TRU64: return "UNIX - TRU64";
        case ELFOSABI_MODESTO: return "Novell - Modesto";
        case ELFOSABI_OPENBSD: return "UNIX - OpenBSD";
        case ELFOSABI_ARM_AEABI: return "ARM EABI";
        case ELFOSABI_ARM: return "ARM";
        case ELFOSABI_STANDALONE: return "standalone (embedded)";
        default: return "unknown";
    }
}

const char* header_type_str(uint16_t value) {
    switch (value) {
        case ET_NONE: return "NONE (unknown type)";
        case ET_REL: return "REL (relocatable file)";
        case ET_EXEC: return "EXEC (executable file)";
        case ET_DYN: return "DYN (shared object)";
        case ET_CORE: return "CORE (core file)";
        default: return "unknown";
    }
}

const char* header_machine_str(uint16_t value) {
    switch (value) {
        case EM_M32: return "AT&T WE 32100";
        case EM_386: return "Intel 80386";
        case EM_X86_64: return "AMD x86-64";
        case EM_ARM: return "ARM";
        case EM_AARCH64: return "AArch64";
        case EM_RISCV: return "RISC-V";
        default: return "unknown";
    }
}

// section

const char* section_type_str(Elf64_Word type) {
    switch (type) {
        case SHT_NULL: return "NULL";
        case SHT_PROGBITS: return "PROGBITS";
        case SHT_SYMTAB: return "SYMTAB";
        case SHT_STRTAB: return "STRTAB";
        case SHT_RELA: return "RELA";
        case SHT_HASH: return "HASH";
        case SHT_DYNAMIC: return "DYNAMIC";
        case SHT_NOTE: return "NOTE";
        case SHT_NOBITS: return "NOBITS";
        case SHT_REL: return "REL";
        case SHT_SHLIB: return "SHLIB";
        case SHT_DYNSYM: return "DYNSYM";
        default: return "unknown";
    }
}

void section_flags_str(uint64_t flags, char* buf) {
    int i = 0;

    if (flags & SHF_WRITE)
        buf[i++] = 'W';

    if (flags & SHF_ALLOC)
        buf[i++] = 'A';

    if (flags & SHF_EXECINSTR)
        buf[i++] = 'X';

    if (flags & SHF_MERGE)
        buf[i++] = 'M';

    if (flags & SHF_INFO_LINK)
        buf[i++] = 'I';

    buf[i++] = '\0';
}

// symbol
const char* symbol_type_str(uint8_t val) {
    switch (ELF64_ST_TYPE(val)) {
        case STT_NOTYPE: return "NOTYPE";
        case STT_OBJECT: return "OBJECT";
        case STT_FUNC: return "FUNC";
        case STT_SECTION: return "SECTION";
        case STT_FILE: return "FILE";
        case STT_COMMON: return "COMMON";
        case STT_TLS: return "TLS";
        default: return "UNKNOWN";
    }
}

const char* symbol_bind_str(uint8_t val) {
    switch (ELF64_ST_BIND(val)) {
        case STB_LOCAL: return "LOCAL";
        case STB_GLOBAL: return "GLOBAL";
        case STB_WEAK: return "WEAK";
        default: return "UNKNOWN";
    }
}

const char* symbol_visibility_str(uint8_t val) {
    switch (ELF64_ST_VISIBILITY(val)) {
        case STV_DEFAULT: return "DEFAULT";
        case STV_INTERNAL: return "INTERNAL";
        case STV_HIDDEN: return "HIDDEN";
        case STV_PROTECTED: return "PROTECTED";
        default: return "UNKNOWN";
    }
}

const char* symbol_shndx_str(uint16_t val) {
    switch (val) {
        case SHN_UNDEF: return "UND";
        case SHN_ABS: return "ABS";
        case SHN_COMMON: return "COM";
        default: return NULL;
    }
}

// relocations

const char* rela_type_str(uint32_t type) {
    switch (type) {
        case R_X86_64_NONE: return "R_X86_64_NONE";
        case R_X86_64_64: return "R_X86_64_64";
        case R_X86_64_PC32: return "R_X86_64_PC32";
        case R_X86_64_GOT32: return "R_X86_64_GOT32";
        case R_X86_64_PLT32: return "R_X86_64_PLT32";
        case R_X86_64_32: return "R_X86_64_32";
        case R_X86_64_32S: return "R_X86_64_32S";
        case R_X86_64_TPOFF32: return "R_X86_64_TPOFF32";
        default: return "UNKNOWN";
    }
}