#ifndef OS_OS_CFG_H
#define OS_OS_CFG_H

#define GDT_TABLE_SIZE      	256		
#define KERNEL_SELECTOR_CS		(1 * 8)		
#define KERNEL_SELECTOR_DS		(2 * 8)		
#define KERNEL_STACK_SIZE       (8*1024)   
#define SELECTOR_SYSCALL     	(3 * 8)	

#define OS_TICK_MS              10       	

#define OS_VERSION              "0.0.1"     

#define IDLE_STACK_SIZE       1024        

#define TASK_NR             128           

#define ROOT_DEV            DEV_DISK, 0xb1  

#endif //OS_OS_CFG_H
