#ifndef IRQ_H
#define IRQ_H

#define IRQ0_DE             0

typedef struct _exception_frame_t {
    int gs, fs, es, ds;
    int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    int num;
    int error_code;
    int eip, cs, eflags;
}exception_frame_t;

typedef void(*irq_handler_t)(void);

void irq_init (void);
int irq_install(int irq_num, irq_handler_t handler);

void exception_handler_unknown (void);
void exception_handler_divider (void);

#endif
