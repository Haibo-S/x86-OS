#include "init.h"
#include "comm/boot_info.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "dev/time.h"
#include "tools/log.h"
#include "os_cfg.h"
#include "tools/klib.h"
#include "core/task.h"

static task_t first_task;
static task_t init_task;

void kernel_init (boot_info_t * boot_info) {
    cpu_init();
    log_init();
    irq_init();
    time_init();
}

void init_task_entry(void){
    int count = 0;
    for(;;){
        log_printf("init task: %d", count++);
    }
}

void init_main(void){
    log_printf("Kernel is Running.......");
    log_printf("Version: %s", OS_VERSION);
    log_printf("%d %d %x %c", 123, -123456, 0x12345, 'a');

    task_init(&init_task, (uint32_t)init_task_entry, 0);
    task_init(&first_task, 0, 0);
    
    int count = 0;
    for(;;){
        log_printf("int main: %d", count++);
    }
}