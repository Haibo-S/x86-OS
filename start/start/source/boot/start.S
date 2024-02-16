#include "boot.h"
.code16
.text
.global _start
	
_start:
	mov $0, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	mov $_start, %esp
	jmp .
	
	
	.section boot_end, "ax"
boot_sig: .byte 0x55, 0xaa
