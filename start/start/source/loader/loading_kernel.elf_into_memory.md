# Loading kernel.elf into Memory

#### Haibo

The ELF file structure is divided into two views for the linking and execution processes, as shown below:

## ELF File Format Views

|           | Linking View         | Execution View     |
|-----------|----------------------|--------------------|
|           | ELF header           | ELF header         |
|   | Program header table | Program header table |
|           | Section 1            | Segment 1          |
|           | ...                  | Segment 2          |
|           | Section n            | ...                |
|   | Section header table | Section header table |

This file includes sections such as `.text`, `.rodata`, `.data`, `.bss`, etc. During the compilation process, the program header table is typically generated, allowing the corresponding code and data to be read. Using this information, the virtual address space for the code is constructed, which completes the final loading process into memory. The ELF header contains global configuration information about the file and points to the section and segment tables located at the beginning of the file.

The `Elf32_Ehdr` structure is defined as follows:

```c
typedef struct {
    char e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} Elf32_Ehdr;
```

The program header table, specified by `e_phoff` in the ELF header, is a table containing elements defined by the `Elf32_Phdr` structure. The number of elements is determined by `e_phnum`.

```c
typedef struct {
    Elf32_Word p_type;
    Elf32_Off p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} Elf32_Phdr;
```

The structure describes the program information that needs to be loaded into memory, and we can parse and load according to the configuration in it. Based on the following analysis, the specific loading process is as follows:

- The initial loading of the elf header is comprehensive.
- Located the program header table through the elf `header -> e_phoff`, and loaded each segment according to the number of `elf header -> e_phnum`.
- Data of size filesz is read from the file position at `p_offset` and written into the memory location at paddr.
- If `p_filesz < p_memsz`, the remaining part of the memory is zeroed out (bss section initialization).
- Based on the `elf header -> e_entry`, the program jumps to that address to run.
