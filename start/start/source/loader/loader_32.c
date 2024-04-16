#include "loader.h"

// LBA mode that reads disk
static void read_disk(int sector, int sector_count, uint8_t * buf) {
    outb(0x1F6, (uint8_t) (0xE0));

	outb(0x1F2, (uint8_t) (sector_count >> 8));
    outb(0x1F3, (uint8_t) (sector >> 24));		
    outb(0x1F4, (uint8_t) (0));					
    outb(0x1F5, (uint8_t) (0));					

    outb(0x1F2, (uint8_t) (sector_count));
	outb(0x1F3, (uint8_t) (sector));			
	outb(0x1F4, (uint8_t) (sector >> 8));		
	outb(0x1F5, (uint8_t) (sector >> 16));		

	outb(0x1F7, (uint8_t) 0x24);

    // we can only read 16 bits each time
	uint16_t *data_buf = (uint16_t*) buf;
	while (sector_count-- > 0) {

        // Polling loop that if data is ready and no error
		while ((inb(0x1F7) & 0x88) != 0x8) {}

        // 512 bytes, each time we could only read 2 byte, so read 512 / 2 times
		for (int i = 0; i < SECTOR_SIZE / 2; i++) {
            // 0x1F0 where the data is
			*data_buf++ = inw(0x1F0);
		}
	}
}

void load_kernel(void){
    read_disk(100, 500, (uint8_t *)SYS_KERNEL_LOAD_ADDR);
    ((void (*)(boot_info_t *))0x10000)(&boot_info);
    for(;;){}
}