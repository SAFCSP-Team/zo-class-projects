#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h> /* Uses system ELF definition structs and constants */

void print_hex_dump(FILE *file, Elf64_Shdr *shdrs, char *shstrtab, int shnum, const char *target_sec) {
    int found = 0;
    for (int i = 0; i < shnum; i++) {
        char *sec_name = shstrtab + shdrs[i].sh_name;
        if (strcmp(sec_name, target_sec) == 0) {
            found = 1;
            printf("\nHex dump of section '%s' (%ld bytes):\n", sec_name, (long)shdrs[i].sh_size);

            if (shdrs[i].sh_size == 0) {
                printf("  (Section is empty)\n");
                break;
            }

            unsigned char *buffer = malloc(shdrs[i].sh_size);
            fseek(file, shdrs[i].sh_offset, SEEK_SET);
            fread(buffer, 1, shdrs[i].sh_size, file);

            for (size_t j = 0; j < shdrs[i].sh_size; j++) {
                if (j % 16 == 0) printf("  0x%04zx: ", j);
                printf("%02x ", buffer[j]);
                if ((j + 1) % 16 == 0 || j + 1 == shdrs[i].sh_size) printf("\n");
            }
            free(buffer);
            break;
        }
    }
    if (!found) {
        printf("\nSection '%s' was not found in this ELF file.\n", target_sec);
    }
}

int main(int argc, char *argv[]){
    char *filename = NULL;
    char *dump_section = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-x") == 0 && i + 1 < argc) {
            dump_section = argv[++i];
        } else {
            filename = argv[i];
        }
    }

    if (!filename) {
        printf("Usage: %s [-x section_name] <elf_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    Elf64_Ehdr ehdr;
    if (fread(&ehdr, 1, sizeof(Elf64_Ehdr), file) < sizeof(Elf64_Ehdr)) {
        printf("Error: Could not read full ELF header.\n");
        fclose(file);
        return 1;
    }

    if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0) {
        printf("Error: Not a valid ELF file\n");
        fclose(file);
        return 255;
    }

    printf("ELF Header:\n");
    printf("  Magic:   ");
    for (int i = 0; i < EI_NIDENT; i++) {
        printf("%02x ", ehdr.e_ident[i]);
    }
    printf("\n");

    printf("  Class:                             ");
    if (ehdr.e_ident[EI_CLASS] == ELFCLASS32) printf("ELF32\n");
    else if (ehdr.e_ident[EI_CLASS] == ELFCLASS64) printf("ELF64\n");
    else printf("Invalid\n");

    printf("  Data:                              ");
    if (ehdr.e_ident[EI_DATA] == ELFDATA2LSB) printf("2's complement, little endian\n");
    else if (ehdr.e_ident[EI_DATA] == ELFDATA2MSB) printf("2's complement, big endian\n");
    else printf("Invalid\n");

    printf("  Version:                           %d (current)\n", ehdr.e_ident[EI_VERSION]);

    printf("  OS/ABI:                            ");
    if (ehdr.e_ident[EI_OSABI] == ELFOSABI_SYSV) printf("UNIX - System V\n");
    else if (ehdr.e_ident[EI_OSABI] == ELFOSABI_LINUX) printf("UNIX - Linux\n");
    else printf("Other (%d)\n", ehdr.e_ident[EI_OSABI]);

    printf("  Type:                              ");
    switch (ehdr.e_type) {
        case ET_NONE: printf("ET_NONE (Unknown)\n"); break;
        case ET_REL:  printf("ET_REL (Relocatable file)\n"); break;
        case ET_EXEC: printf("ET_EXEC (Executable file)\n"); break;
        case ET_DYN:  printf("ET_DYN (Shared object file)\n"); break;
        case ET_CORE: printf("ET_CORE (Core file)\n"); break;
        default:      printf("Unknown (%d)\n", ehdr.e_type); break;
    }

    printf("  Machine:                           ");
    switch (ehdr.e_machine) {
        case EM_386:    printf("Intel 80386\n"); break;
        case EM_X86_64: printf("Advanced Micro Devices X86-64\n"); break;
        case EM_ARM:    printf("ARM\n"); break;
        default:        printf("Other (0x%02x)\n", ehdr.e_machine); break;
    }

    printf("  Version:                           0x%x\n", ehdr.e_version);
    printf("  Entry point address:               0x%lx\n", (unsigned long)ehdr.e_entry);
    printf("  Start of program headers:          %ld (bytes into file)\n", (long)ehdr.e_phoff);
    printf("  Start of section headers:          %ld (bytes into file)\n", (long)ehdr.e_shoff);
    printf("  Flags:                             0x%x\n", ehdr.e_flags);
    printf("  Size of this header:               %d (bytes)\n", ehdr.e_ehsize);
    printf("  Size of program headers:           %d (bytes)\n", ehdr.e_phentsize);
    printf("  Number of program headers:         %d\n", ehdr.e_phnum);
    printf("  Size of section headers:           %d (bytes)\n", ehdr.e_shentsize);
    printf("  Number of section headers:         %d\n", ehdr.e_shnum);
    printf("  Section header string table index: %d\n", ehdr.e_shstrndx);

    Elf64_Shdr *shdrs = malloc(ehdr.e_shnum * sizeof(Elf64_Shdr));
    fseek(file, ehdr.e_shoff, SEEK_SET);
    fread(shdrs, sizeof(Elf64_Shdr), ehdr.e_shnum, file);

    Elf64_Shdr strtab_hdr = shdrs[ehdr.e_shstrndx];
    char *shstrtab = malloc(strtab_hdr.sh_size);
    fseek(file, strtab_hdr.sh_offset, SEEK_SET);
    fread(shstrtab, 1, strtab_hdr.sh_size, file);

    printf("\nSection Headers:\n");
    printf("  [Nr] Name                 Type       Address          Offset       Size\n");
    for (int i = 0; i < ehdr.e_shnum; i++) {
        printf("  [%2d] %-20s 0x%-8x 0x%016lx 0x%08lx 0x%lx\n",
            i,
            shstrtab + shdrs[i].sh_name,
            shdrs[i].sh_type,
            (unsigned long)shdrs[i].sh_addr,
            (unsigned long)shdrs[i].sh_offset,
            (unsigned long)shdrs[i].sh_size);
    }

    if (dump_section) {
        print_hex_dump(file, shdrs, shstrtab, ehdr.e_shnum, dump_section);
    }

    free(shdrs);
    free(shstrtab);
    fclose(file);
    return 0;
}