# Linker File and Disassembly
#### Haibo

##

The compiler, by default, organizes the code and data of the project into different segments or sections. Here is a summary of what each section typically contains:

- `.text`: Stores the instruction set and code.
- `.rodata`: Stores constants and string literals.
- `.data`: Stores initialized data, whether global or static.
- `.bss`: Stores uninitialized data (i.e., initially zero), whether global or static.
- `stack`: Stores local variables.

For each data type or chunk of code, the compiler automatically combines these into their respective sections.

## Section Headers

| Nr | Name       | Type      | Addr      | Off      | Size     | ES | Flg | Lk | Inf | Al |
|----|------------|-----------|-----------|----------|----------|----|-----|----|-----|----|
| 0  |        |     NULL      | 00000000  | 00000000 | 00000000 | 00 |     |  0 |   0 |  0 |
| 1  | .text      | PROGBITS  | 00008000  | 001000   | 0004f3   | 00 | AX  |  0 |   0 |  1 |
| 2  | .rodata    | PROGBITS  | 000084f3  | 0014f3   | 000039   | 00 | A   |  0 |   0 |  1 |
| 3  | .data      | PROGBITS  | 0000952c  | 00152c   | 000018   | 00 | WA  |  0 |   0 |  4 |
| 4  | .bss       | NOBITS    | 00009560  | 001544   | 000054   | 00 | WA  |  0 |   0 | 32 |
| ...| (other)    | ...       | ...       | ...      | ...      | .. | ..  | .. | ... | .. |

## Symbol Table:

| Nr   | Value    | Sec Nr | Type | Section Header Type | Addr|Name                  |
|------|----------|--------|------|------|----------|-------------|
| ...  | ...      | ...    | ...  | ...  | ...                   |
| 43   | 000084c5 | g      | F    | .text| 0000002e  | load_kernel |
| 44   | 00009560 | g      | O    | .bss | 00000054 | boot_info   |
| ...  | ...      | ...    | ...  | ...  | ...                   |
| 46   | 00008000 | g      |      | .text| ...      | _start       |
| ...  | ...      | ...    | ...  | ...  | ...                   |


