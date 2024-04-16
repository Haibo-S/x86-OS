#include "loader.h"
#include "comm/elf.h"

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

static void die (int code) {
    for (;;) {
    }
}

static uint32_t reload_elf_file(uint8_t * file_buffer){
    Elf32_Ehdr * elf_hdr = (Elf32_Ehdr *)file_buffer;
    if((elf_hdr->e_ident[0] != 0x7F) || (elf_hdr->e_ident[1] != 'E')
    || (elf_hdr->e_ident[2] != 'L') || (elf_hdr->e_ident[3] != 'F')){
        return 0;
    }

    for (int i = 0; i < elf_hdr->e_phnum; i++) {
        Elf32_Phdr * phdr = (Elf32_Phdr *)(file_buffer + elf_hdr->e_phoff) + i;
        if (phdr->p_type != PT_LOAD) {
            continue;
        }

        uint8_t * src = file_buffer + phdr->p_offset;
        uint8_t * dest = (uint8_t *)phdr->p_paddr;
        for (int j = 0; j < phdr->p_filesz; j++) {
            *dest++ = *src++;
        }

		dest= (uint8_t *)phdr->p_paddr + phdr->p_filesz;
		for (int j = 0; j < phdr->p_memsz - phdr->p_filesz; j++) {
			*dest++ = 0;
		}
    }

    return elf_hdr->e_entry;
}

void load_kernel(void){
    read_disk(100, 500, (uint8_t *)SYS_KERNEL_LOAD_ADDR);
    uint32_t kernel_entry = reload_elf_file((uint8_t *)SYS_KERNEL_LOAD_ADDR);
	if (kernel_entry == 0) {
		die(-1);
	}
    ((void (*)(boot_info_t *))kernel_entry)(&boot_info);
}