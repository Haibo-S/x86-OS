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

## Testing Verification
To verify the proper function of the clear interrupt instruction in the `enter_protect_mode(void)` function, follow these testing steps:

1. **Set a Breakpoint**:
   - Set up a breakpoint at the line `cli()` within the function.

2. **Start the Emulator**:
   - Build the project and press F5 to start QEMU.

3. **Monitor the EFLAGS Register**:
   - At the breakpoint, open the registers/CPU section in the right side of the gdb debug monitor window. Initially, the `eflags` register should display `0x206`.
   - Press "Step over" at this breakpoint. After stepping over, the `eflags` register should change to `0x6`, demonstrating the impact of the `cli()` command on the `eflags` register.

4. **Continue Execution**:
   - Step over subsequent instructions until you reach `lgdt((uint32_t)gdt_table, sizeof(gdt_table));`.

5. **Verify GDT Setup**:
   - In the QEMU window, go to the top panel, select "View", and choose `compatmonitor()`. A new window will appear.
   - Type `info registers` to view the register values. Before stepping over `lgdt()`, the GDT register should show `00000000 00000000`.
   - Step over the `lgdt()` instruction and type `info registers` again. In the VS Code debug section, under "Watch", add a new watch expression for '&gdt_table'. The value displayed for the GDT register in QEMU should match the '&gdt_table' value shown in VS Code, confirming that the Global Descriptor Table (GDT) has been set up correctly.

6.  **Verify Jump to Protected Mode Entry**:
   - To confirm that the system can correctly jump to the `protected_mode_entry` in the `start.S` file, and subsequently to the `load_kernel` function in `loader_32.c`, set a breakpoint at `far_jump(8, (uint32_t)protected_mode_entry);`.
   - Step through this breakpoint. You should arrive directly at the `load_kernel()` function in `loader_32.c`. If you are unable to reach this function, ensure that the `far_jump` function in `cpu_instr.c` is configured correctly:

     ```c
     static inline void far_jump(uint32_t selector, uint32_t offset) {
         uint32_t addr[] = {offset, selector};
         __asm__ __volatile__("ljmpl *(%[a])"::[a]"r"(addr));
     }
     ```

7. **Verify Register Setup for 32-bit Mode**:
   - To ensure that all the essential x86 registers (ES, CS, SS, DS, FS, GS) are correctly configured for 32-bit operation, set a breakpoint at the `load_kernel()` function within `loader32.c`.
   - Launch QEMU, navigate to "View" -> "compactmonitor" and type `info registers` to inspect the register states. 
   - Verify that you see the value `0x00cf9300` for all these registers, specifically `CS32` for the CS register and the same value for DS, ES, SS, FS, and GS registers. This setup indicates that the registers are properly initialized for 32-bit protected mode.


### References:
1. https://wiki.osdev.org/Real_Mode
2. https://wiki.osdev.org/A20_Line