#include "init.h"
#include "comm/boot_info.h"
#include "cpu/cpu.h"

void init_main(void){
    for(;;){}
}


void kernel_init (boot_info_t * boot_info) {
    cpu_init();

}