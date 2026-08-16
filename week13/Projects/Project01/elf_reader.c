#include "elf_reader.h"

bool read_elf_file (FILE* file_path){
    if(!file_path) return false;

    struct stat fileInfo;
    fstat(fileno(file_path), &fileInfo);

    ELF_BASE = malloc(fileInfo.st_size);
    if(!ELF_BASE) return false;
    fread(ELF_BASE,fileInfo.st_size,1,file_path);
    fclose(file_path);

    ELF_HEADER64  = (Elf64_Ehdr*)ELF_BASE;
    ELF_SHEADER64 = (Elf64_Shdr*)(ELF_HEADER64 + ELF_HEADER64->e_shoff);
    ELF_PHEADER64 = (Elf64_Phdr*)(ELF_HEADER64 + ELF_HEADER64->e_phoff);

}
bool delete_elf_file (uint8_t* ELF_BASE){
    if(!ELF_BASE) return false;
    free(ELF_BASE);
    return true;
}

char* get_elf_magic(Elf64_Ehdr* ELF_HEADER64){
    if(!ELF_HEADER64){
        printf("ELF File doesn't exist");
        exit(1);
    }

    static char magic [EI_NIDENT * 3 + 1];

    for (int i = 0; i < EI_NIDENT; i++) {
        sprintf(magic + (i * 3), "%02x ", ELF_HEADER64->e_ident[i]);
    }
    magic[EI_NIDENT * 3] = '\0';   // make sure it's terminated

    return magic;
}

char* get_elf_class(Elf64_Ehdr* ELF_HEADER64){
    if(!ELF_HEADER64){
        printf("ELF File doesn't exist");
        exit(1);
    }

    char *class = NULL;
        if(ELF_HEADER64->e_ident[EI_CLASS] == ELFCLASS64)
            class = "ELF64";
        else if(ELF_HEADER64->e_ident[EI_CLASS] == ELFCLASS32)
            class = "ELF32";
        else
            class = "the class is unknown";
    
    return class;
}

char* get_elf_data(Elf64_Ehdr* ELF_HEADER64){
    if(!ELF_HEADER64){
        printf("ELF File doesn't exist");
        exit(1);
    }

    char *data = NULL;
        if(ELF_HEADER64->e_ident[EI_DATA] == ELFDATA2LSB)
            data = "Two's complement, little-endian.";
        else if (ELF_HEADER64->e_ident[EI_DATA] == ELFDATA2MSB)
            data = "Two's complement, big-endian.";
        else
            data = "Unknown data format.";
    
    return data;
}

char* get_elf_os(Elf64_Ehdr* ELF_HEADER64){
    if(!ELF_HEADER64){
        printf("ELF File doesn't exist");
        exit(1);
    }

    char *os = NULL;
        switch (ELF_HEADER64->e_ident[EI_OSABI])
        {
        case ELFOSABI_SYSV:
            os = "UNIX System V ABI";
            break;
        case ELFOSABI_HPUX:
            os = "HP-UX ABI";
            break;
        case ELFOSABI_NETBSD:
            os = "NetBSD ABI";
            break;
        case ELFOSABI_LINUX:
            os = "Linux ABI";
            break;
        case ELFOSABI_SOLARIS:
            os = "Solaris ABI";
            break;
        case ELFOSABI_IRIX:
            os = "IRIX ABI";
            break;
        case ELFOSABI_FREEBSD:
            os = "FreeBSD ABI";
            break;;
        case ELFOSABI_TRU64:
            os = "TRU64 UNIX ABI";
            break;
        case ELFOSABI_ARM:
            os = "ARM architecture ABI";
            break;
        case ELFOSABI_STANDALONE:
            os = "Stand-alone (embedded) ABI";
            break;
        }
    
    return os;
}

char* get_elf_type(Elf64_Ehdr* ELF_HEADER64){
    if(!ELF_HEADER64){
        printf("ELF File doesn't exist");
        exit(1);
    }

    char *type = NULL;
        switch (ELF_HEADER64->e_type){
        case ET_NONE:
            type = "An unknown type.";
            break;
        case ET_REL:
            type = "A relocatable file.";
            break;
        case ET_EXEC:
            type = "An executable file.";
            break;
        case ET_DYN:
            type = "A shared object.";
            break;
        case ET_CORE:
            type = "A core file.";
            break;
        }

    return type;
}

char* get_elf_machine(Elf64_Ehdr* ELF_HEADER64){
    if(!ELF_HEADER64){
        printf("ELF File doesn't exist");
        exit(1);
    }

    char *machine = NULL;
        switch (ELF_HEADER64->e_machine){
        case EM_386:
            machine = "Intel 80386";
        break;
        case EM_860:
            machine = "Intel 80860";
        break;
        case EM_PPC:
            machine = "PowerPC";
        break;
        case EM_PPC64:
            machine = "PowerPC 64-bit";
        break;
        case EM_ARM:
            machine = "Advanced RISC Machines";
        break;
        case EM_IA_64:
            machine = "Intel Itanium";
        break;
        case EM_X86_64:
            machine = "AMD x86-64";
        break;
        default:
            machine = "An unknown machine";
        break;
        }
    
    return machine;
}

bool print_elf_header(Elf64_Ehdr* ELF_HEADER64){
    char* magic = get_elf_magic(ELF_HEADER64);

    char *class = get_elf_class(ELF_HEADER64);

    char *data = get_elf_data(ELF_HEADER64);
    
    char *os = get_elf_os(ELF_HEADER64);
    
    char *type = get_elf_type(ELF_HEADER64);

    char *machine = get_elf_machine(ELF_HEADER64);

    unsigned long entry_point_address = ELF_HEADER64->e_entry;

    unsigned long start_of_sheaders  = ELF_HEADER64->e_shoff;
    unsigned int  size_of_sheaders   = ELF_HEADER64->e_shentsize;
    unsigned int  number_of_sheaders = ELF_HEADER64->e_shnum;

    unsigned long start_of_pheaders  = ELF_HEADER64->e_phoff;
    unsigned int  size_of_pheaders   = ELF_HEADER64->e_phentsize;
    unsigned int  number_of_pheaders = ELF_HEADER64->e_phnum;

    unsigned int  section_strtab_index = ELF_HEADER64->e_shstrndx;

    printf("Magic : %s\n" "Class : %s\n" "Data : %s\n" "OS/ABI : %s\n" 
            "Type : %s\n" "Machine : %s\n" "Entry point address : %lx\n"
            "Start of program headers : %lx (bytes into file)\n"
            "Start of section headers : %lx (bytes into file)\n"
            "Size of program headers : %x (bytes)\n"
            "Number of program headers : %x\n"
            "Size of section headers : %x (bytes)\n"
            "Number of section headers : %x\n"
            "Section header string table index : %x\n",
            magic, class, data, os, type, machine,
            entry_point_address, start_of_pheaders, start_of_sheaders, size_of_pheaders,
            number_of_pheaders, size_of_sheaders, number_of_sheaders, section_strtab_index);
    
    return true;
}

int main(void){

    FILE *FILE = fopen("main","rb");
    
    if(read_elf_file(FILE)){
        print_elf_header(ELF_HEADER64);
        delete_elf_file(ELF_BASE);
        return 0;
    }
    printf("smth went wrong ");
    return 0;
}
// bool print_elf_sheader(Elf64_Ehdr* ELF_SHEADER64, Elf64_Half num);
// bool print_elf_pheader(Elf64_Ehdr* ELF_PHEADER64, Elf64_Half num);