#include <stdio.h>
#include <elf.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdint.h>
#include <string.h>

int get_file_size(char *filename)
{
    struct stat st;
    if (stat(filename, &st) == 0)
    {
        return st.st_size;
    }
    return -1;
}

void print_elf_header(Elf64_Ehdr *header)
{
    printf("Magic: ");
    for (int i = 0; i < 16; i++)
    {
        printf("%02x ", header->e_ident[i]);
    }
    printf("\n");

    printf("Type: %u\n", header->e_type);
    printf("Machine: %u\n", header->e_machine);
    printf("Version: %u\n", header->e_version);
    printf("Entry point address: 0x%lx\n", header->e_entry);
    printf("Start of program headers: %lu\n", header->e_phoff);
    printf("Start of section headers: %lu\n", header->e_shoff);
    printf("Flags: 0x%x\n", header->e_flags);
    printf("Size of this header: %u\n", header->e_ehsize);
    printf("Size of program headers: %u\n", header->e_phentsize);
    printf("Number of program headers: %u\n", header->e_phnum);
    printf("Size of section headers: %u\n", header->e_shentsize);
    printf("Number of section headers: %u\n", header->e_shnum);
    printf("Section header string table index: %u\n", header->e_shstrndx);
    printf("----------------------------------------\n");
}

void print_elf_shdr(Elf64_Shdr *shdr)
{
    printf("Name offset (sh_name): %u\n", shdr->sh_name);
    printf("Type (sh_type): %u\n", shdr->sh_type);

    printf("Flags (sh_flags): 0x%lx\n", shdr->sh_flags);
    printf("Address (sh_addr): 0x%lx\n", shdr->sh_addr);

    printf("File offset (sh_offset): %lu\n", shdr->sh_offset);
    printf("Size in bytes (sh_size): %lu\n", shdr->sh_size);
    printf("Link (sh_link): %u\n", shdr->sh_link);
    printf("Info (sh_info): %u\n", shdr->sh_info);
    printf("Address alignment (sh_addralign): %lu\n", shdr->sh_addralign);
    printf("Entry size (sh_entsize): %lu\n", shdr->sh_entsize);
    printf("----------------------------------------\n");
}

void print_elf_sym(Elf64_Sym *sym)
{
    printf("Name offset (st_name): %u\n", sym->st_name);

    printf("Info (st_info): %u\n", sym->st_info);
    printf("Other/Visibility (st_other): %u\n", sym->st_other);

    printf("Section index (st_shndx): %u\n", sym->st_shndx);

    printf("Value (st_value): 0x%lx\n", sym->st_value);

    printf("Size (st_size): %lu\n", sym->st_size);
    printf("----------------------------------------\n");
}

#include <ctype.h> // Required for isprint()

void print_elf_sections(unsigned char *base, Elf64_Off offset, Elf64_Xword size, Elf64_Word type)
{
    if (type == 8) 
    {
        printf("  [ NOBITS section - no data on disk ]\n");
        return;
    }
    
    if (size == 0)
    {
        printf("  [ Section is empty ]\n");
        return;
    }

    unsigned char *data = base + offset;

    for (Elf64_Xword i = 0; i < size; i++)
    {
        if (i % 16 == 0)
        {
            printf("  %08lx  ", i);
        }

        printf("%02x ", data[i]);

        if (i % 16 == 15 || i == size - 1)
        {
            int padding = 15 - (i % 16);
            for (int p = 0; p < padding; p++)
            {
                printf("   "); 
            }

            printf(" |");

            Elf64_Xword start_of_row = i - (i % 16);
            for (Elf64_Xword j = start_of_row; j <= i; j++)
            {
                if (isprint(data[j]))
                {
                    printf("%c", data[j]);
                }
                else
                {
                    printf("."); 
                }
            }

            printf("|\n"); 
        }
    }
}
int main(int argc, char *argv[])
{
    char *file_name = argv[1];
    char *option = argv[2];

    FILE *fh = fopen(file_name, "rb");
    if (!fh)
        return 1;

    int file_size = get_file_size(file_name);

    unsigned char *base = (unsigned char *)malloc(file_size);
    if (!base)
        return 2;
    fread(base, file_size, 1, fh);
    fclose(fh);

    Elf64_Ehdr *header_base = (Elf64_Ehdr *)base;
    Elf64_Shdr *shdr_base = (Elf64_Shdr *)(base + header_base->e_shoff);

    long symtab_offset = 0;
    long strtab_offset = 0;
    int symtab_elements_count = 0;
    for (int i = 0; i < header_base->e_shnum; i++)
    {
        if (shdr_base[i].sh_type == 2)
        {
            symtab_offset = shdr_base[i].sh_offset;
            strtab_offset = shdr_base[shdr_base[i].sh_link].sh_offset;
            symtab_elements_count = shdr_base[i].sh_size / shdr_base[i].sh_entsize;
        }
    }

    Elf64_Sym *symtab_base = (Elf64_Sym *)(base + symtab_offset);
    unsigned char *strtab_base = (unsigned char *)(base + strtab_offset);
    unsigned char *shstrtab_base = (unsigned char *)(base + shdr_base[header_base->e_shstrndx].sh_offset);

    if (strcmp(option, "-h") == 0)
    {
        print_elf_header(header_base);
    }
    else if (strcmp(option, "-s") == 0)
    {
        for (int i = 0; i < header_base->e_shnum; i++)
        {
            printf("printing section %s\n", shstrtab_base + shdr_base[i].sh_name);
            print_elf_sections(base, shdr_base[i].sh_offset, shdr_base[i].sh_size, shdr_base[i].sh_type);
            printf("----------------------------------------\n");
        }
    }
    else if (strcmp(option, "-sh") == 0)
    {
        for (int i = 0; i < header_base->e_shnum; i++)
        {
            print_elf_shdr(&shdr_base[i]);
        }
    }
    else if (strcmp(option, "-shstrtab") == 0)
    {
        for (int i = 0; i < header_base->e_shnum; i++)
        {
            printf("%s\n", shstrtab_base + shdr_base[i].sh_name);
        }
    }
    else if (strcmp(option, "-symtab") == 0)
    {
        for (int i = 0; i < symtab_elements_count; i++)
        {
            print_elf_sym(&symtab_base[i]);
        }
    }
    else if (strcmp(option, "-strtab") == 0)
    {
        for (int i = 0; i < symtab_elements_count; i++)
        {
            printf("%s\n", strtab_base + symtab_base[i].st_name);
        }
    }

    return 0;
}
