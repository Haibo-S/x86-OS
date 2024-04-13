#ifndef CPU_INSTR_H
#define CPU_INSTR_H

#include "types.h"

// in al, dl
static inline uint8_t inb(uint16_t  port) {
	uint8_t rv;
	__asm__ __volatile__("inb %[p], %[v]" : [v]"=a" (rv) : [p]"d"(port));
	return rv;
}

static inline uint16_t inw(uint16_t  port) {
	uint16_t rv;
	__asm__ __volatile__("in %1, %0" : "=a" (rv) : "dN" (port));
	return rv;
}

// or al, dl
static inline void outb(uint16_t port, uint8_t data) {
	__asm__ __volatile__("outb %[v], %[p]" : : [p]"d" (port), [v]"a" (data));
}

// Clear Interrupt 
static inline void cli(void){
    __asm__ __volatile__("cli");
}

// Set Interrupt 
static inline void sti(void){
    __asm__ __volatile__("sti");
}

// lgdt for GDT Table
static inline void lgdt(uint32_t start, uint32_t size) {
	struct {
		uint16_t limit;
		uint16_t start15_0;
		uint16_t start31_16;
	} gdt;

	gdt.start31_16 = start >> 16;
	gdt.start15_0 = start & 0xFFFF;
	gdt.limit = size - 1;

	__asm__ __volatile__("lgdt %[g]"::[g]"m"(gdt));
}

#endif