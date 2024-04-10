// Lets CPU to execute in 16 bits option instead of 32 bits
// Protected Mode
__asm__(".code16gcc");

void loader_entry(void) {
    for(;;) {}
}
