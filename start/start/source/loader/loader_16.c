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

// memory_detect.md
// First Pass：EDX=0x534D4150,EAX=0xE820,ECX=24,INT 0x15, EBX=0
// Further Passes：EAX=0xE820,ECX=24
// Done：EBX=0
static void detect_memory(void) {
	uint32_t contID = 0;
	SMAP_entry_t smap_entry;
	int signature, bytes;

    show_msg("Trying to detect memory:\n\r");

	boot_info.ram_region_count = 0;
	for (int i = 0; i < BOOT_RAM_REGION_MAX; i++) {
		SMAP_entry_t* entry = &smap_entry;

		__asm__ __volatile__("int  $0x15"
			: "=a"(signature), "=c"(bytes), "=b"(contID)
			: "a"(0xE820), "b"(contID), "c"(24), "d"(0x534D4150), "D"(entry));
		if (signature != 0x534D4150) {
            show_msg("failed.\r\n");
			return;
		}

		if (bytes > 20 && (entry->ACPI & 0x0001) == 0){
			continue;
		}

        // Store RAM Info. 32 bits. No need to consider greater volume.
        if (entry->Type == 1) {
            boot_info.ram_region_cfg[boot_info.ram_region_count].start = entry->BaseL;
            boot_info.ram_region_cfg[boot_info.ram_region_count].size = entry->LengthL;
            boot_info.ram_region_count++;
        }

		if (contID == 0) {
			break;
		}
	}
    show_msg("Success.\n\r");
}

// GDT Table
uint16_t gdt_table[][4] = {
    {0, 0, 0, 0},
    {0xFFFF, 0x0000, 0x9A00, 0x00CF},
    {0xFFFF, 0x0000, 0x9200, 0x00CF},
};

static void enter_protect_mode(void){
	cli();
	uint8_t v = inb(0x92);
	outb(0x92, v|0x2);

	lgdt((uint32_t)gdt_table, sizeof(gdt_table));

}


void loader_entry(void) {
    show_msg("....Loading....\n\r");
    detect_memory();
	enter_protect_mode();
    for(;;) {}
}
