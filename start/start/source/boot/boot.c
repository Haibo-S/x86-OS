__asm__(".code16gcc");

#include "boot.h"

#define LOADER_START_ADDR   0x8000

/**
 * Boot Entering into C Environment
 * Finds loader file from the hard disk and load into stack, then jump to where the loader starts
 */
void boot_entry(void) {
    ((void (*)(void))LOADER_START_ADDR)();
} 

