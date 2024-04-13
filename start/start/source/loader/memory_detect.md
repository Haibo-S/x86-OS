# Memory Detection Mechanism: INT 0x15, EAX = 0xE820
#### Haibo

## Overview
This document describes the use of the `INT 0x15` interrupt with `EAX` set to `0xE820` to detect memory layout using the x86 architecture's registers.

### General x86 Registers
The following registers are used in the memory detection mechanism:

- **EAX (Temporary Variable)**: Used for temporary storage of data.
- **EBX (Base Pointer/Register)**: Serves as a base pointer for memory addresses.
- **ECX (Counter Register)**: Utilized as a counter in loops or functions.
- **EDX (Data Register)**: Employed for data manipulation and storage.

The prefix "E" denotes that these are 32-bit registers. The "R" prefix would indicate 64-bit registers.

### Indexes and Pointers
- **ES:DI**: These are destination index registers used for addressing.

## Mechanism for Detecting Memory

### Initial Pass
1. **Prepare Registers**:
   - Clear EBX and set it to 0.
   - Set EDX to `0x534D4150`.
   - Set EAX to `0xE820`.
   - Set ECX to 24.

2. **Execute Interrupt**:
   - Execute `INT 0x15`.

3. **Returned Result**:
   - EAX should return `0x534D4150`.
   - The Carry Flag (CF) should be cleared indicating success.
   - EBX is set to a new value, preparing for the next execution.
   - CL holds the number of bytes read.

### Subsequent Passes
Repeat the steps from the initial pass:

1. Set EDX to `0x534D4150`.
2. Set EAX to `0xE820`.
3. Set ECX to 24.
4. Execute `INT 0x15`.

- **Check EBX**: If EBX equals 0, memory reading is complete. Otherwise, the current memory information is valid and another pass is necessary.

### Memory Information Structure
```c
// Struct For Storing Memory Detection Information
typedef struct SMAP_entry {
    uint32_t BaseL; // base address uint64_t
    uint32_t BaseH;
    uint32_t LengthL; // length uint64_t
    uint32_t LengthH;
    uint32_t Type; // entry Type, if the value is 1 that means we have available RAM space
    uint32_t ACPI; // extended, if bit 0 is 1 then that means we should ignore this information
} __attribute__((packed)) SMAP_entry_t;
```

## Testing
In the file `loader16.c`, a breakpoint is set at line 60 within the function `void detect_memory(void);` where it executes `show_msg("Success.\r\n");`. To test:

1. Build the project.
2. Start QEMU for Windows by pressing F5 or use VS Code to navigate to Run -> Debug.
3. Continue execution up to the set breakpoint.
4. Examine the data in the `boot_info` variable previously defined.

Expected results:
- For `boot_info`'s `ram_region_cfg` field:
  - Element 0 should have `start` variable with 0 and `size` variable with 654336.
  - Element 1 should have `start` variable with 1048576 and `size` variable with 133038080.

**Explanation**:
The `qemu-debug-win.bat` script sets the QEMU system with the following configuration:
```bash
start qemu-system-i386 -m 128M -s -S -drive file=disk1.vhd,index=0,media=disk,format=raw -drive file=disk2.vhd,index=1,media=disk,format=raw -d pcall,page,mmu,cpu_reset,guest_errors,page,trace:ps2_keyboard_set_translation
```
This script specifies a memory size of 128MB. It's important to note that due to protected regions in RAM, not all of this memory is accessible, which explains the observed memory sizes and configurations in the boot_info structure.

### References:
1. https://www.eecg.utoronto.ca/~amza/www.mindsec.com/files/x86regs.html
2. https://wiki.osdev.org/Detecting_Memory_(x86)
