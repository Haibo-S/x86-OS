#include "init.h"
#include "comm/boot_info.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "dev/time.h"
#include "tools/log.h"
#include "os_cfg.h"

void kernel_init (boot_info_t * boot_info) {
    cpu_init();
    log_init();
    irq_init();
    time_init();
}

void init_main(void){
    log_printf("Kernel is Running.......");
    log_printf("Version: %s", OS_VERSION);
    log_printf("%d %d %x %c", 123, -123456, 0x12345, 'a');
    int a = 3 / 0;
    // irq_enable_global();
    for(;;){}
}