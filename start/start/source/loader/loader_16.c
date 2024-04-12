// Lets CPU to execute in 16 bits option instead of 32 bits
// Protected Mode
__asm__(".code16gcc");

#include "loader.h"

static boot_info_t boot_info;

// Inline Assembly
// Used for Showing Messages with INT Software Interrupt
static void show_msg(const char* msg){
    char c;
    while((c = *msg++) != '\0'){
        __asm__ __volatile__(
            "mov $0xe, %%ah\n\t"
            "mov %[ch], %%al\n\t"
            "int $0x10"::[ch]"r"(c)
        );
    }
}


void loader_entry(void) {
    show_msg("....Loading....");
    for(;;) {}
}
