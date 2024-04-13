# Transition from Real Mode to Protected Mode
#### Haibo

## Real Mode
When the CPU powers on and resets, it initially sets and enters real mode. This mode provides BIOS services but has several limitations:
- **Memory Access**: Only up to 1MB of memory can be accessed.
- **Register Width**: All registers are at most 16 bits wide.
- **Protection**: There is no protection mechanism.
- **Privilege Levels**: No concept of privilege levels exists.
- **Paging**: There is no paging mechanism and no support for isolating memory.

## Protected Mode
Protected mode enhances the capabilities of the system, supporting the operation of operating systems and applications:
- **Register Bit Width**: Extended to 32 bits, allowing up to 4GB of memory.
- **Privilege Levels**: Provides four levels of privilege, with the operating system operating at the highest level and applications at the lowest.
- **Virtual Memory**: Supports virtual memory and enables a paging mechanism.

## Transition Procedures
To switch from real mode to protected mode, follow these steps:
1. **Disable All Interrupts**: This prevents exceptions during the transition.
2. **Open A20 Port**: Essential for accessing beyond 1MB of memory.
3. **Load GDT Table**: Configures necessary data for protected mode.
4. **Configure CR0**: Set the Protection Enable (PE) flag in CR0 to enter protected mode.
5. **Clear Pipeline**: Remove any remaining 16-bit instructions.
6. **Enter Protected Mode**: Activate the new mode settings.

## How to Open A20 Address Port
To enable addressing beyond 1MB, the A20 line must be enabled using the following x86 assembly code:
```assembly
in al, 0x92
or al, 2
out 0x92, al
```

This sequence manipulates the system's hardware directly, facilitating the transition to protected mode by allowing access to extended memory regions.

### References:
1. https://wiki.osdev.org/Real_Mode
2. https://wiki.osdev.org/A20_Line