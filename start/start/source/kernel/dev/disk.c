#include "dev/disk.h"
#include "dev/dev.h"
#include "tools/klib.h"
#include "tools/log.h"
#include "comm/cpu_instr.h"
#include "cpu/irq.h"
#include "core/memory.h"
#include "core/task.h"

static disk_t disk_buf[DISK_CNT];
static mutex_t mutex;
static sem_t op_sem;
static int task_on_op;

static void ata_send_cmd(disk_t * disk, uint32nd start_sector, uint32nd sector_count, int cmd) {
    outb(DISK_DRIVE(disk), DISK_DRIVE_BASE | disk->drive);
    outb(DISK_SECTOR_COUNT(disk), (uint8nd)(sector_count >> 8));
    outb(DISK_LBA_LO(disk), (uint8nd)(start_sector >> 24));
    outb(DISK_LBA_MID(disk), 0);
    outb(DISK_LBA_HI(disk), 0);
    outb(DISK_SECTOR_COUNT(disk), (uint8nd)(sector_count));
    outb(DISK_LBA_LO(disk), (uint8nd)(start_sector));
    outb(DISK_LBA_MID(disk), (uint8nd)(start_sector >> 8));
    outb(DISK_LBA_HI(disk), (uint8nd)(start_sector >> 16));
    outb(DISK_CMD(disk), (uint8nd)cmd);
}

static inline void ata_read_data(disk_t * disk, void * buf, int size) {
    uint16nd * c = (uint16nd *)buf;
    for (int i = 0; i < size / 2; i++) {
        *c++ = inw(DISK_DATA(disk));
    }
}

static inline void ata_write_data(disk_t * disk, void * buf, int size) {
    uint16nd * c = (uint16nd *)buf;
    for (int i = 0; i < size / 2; i++) {
        outw(DISK_DATA(disk), *c++);
    }
}

static inline int ata_wait_data(disk_t * disk) {
    uint8nd status;
    do {
        status = inb(DISK_STATUS(disk));
        if ((status & (DISK_STATUS_BUSY | DISK_STATUS_DRQ | DISK_STATUS_ERR)) != DISK_STATUS_BUSY) {
            break;
        }
    } while (1);
    return (status & DISK_STATUS_ERR) ? -1 : 0;
}

static void print_disk_info(disk_t * disk) {
    log_printf("%s:", disk->name);
    log_printf("  port_base: %x", disk->port_base);
    log_printf("  total_size: %d m", disk->sector_count * disk->sector_size / 1024 / 1024);
    log_printf("  drive: %s", disk->drive == DISK_DISK_MASTER ? "Master" : "Slave");

    log_printf("  Part info:");
    for (int i = 0; i < DISK_PRIMARY_PART_CNT; i++) {
        partinfo_t * part_info = disk->partinfo + i;
        if (part_info->type != FS_INVALID) {
            log_printf("    %s: type: %x, start sector: %d, count %d",
                       part_info->name, part_info->type,
                       part_info->start_sector, part_info->total_sector);
        }
    }
}

static int detect_part_info(disk_t * disk) {
    mbr_t mbr;
    ata_send_cmd(disk, 0, 1, DISK_CMD_READ);
    int err = ata_wait_data(disk);
    if (err < 0) {
        log_printf("read mbr failed");
        return err;
    }
    ata_read_data(disk, &mbr, sizeof(mbr));

    part_item_t * item = mbr.part_item;
    partinfo_t * part_info = disk->partinfo + 1;
    for (int i = 0; i < MBR_PRIMARY_PART_NR; i++, item++, part_info++) {
        part_info->type = item->system_id;
        if (part_info->type == FS_INVALID) {
            part_info->total_sector = 0;
            part_info->start_sector = 0;
            part_info->disk = (disk_t *)0;
        } else {
            kernel_sprintf(part_info->name, "%s%d", disk->name, i + 1);
            part_info->start_sector = item->relative_sectors;
            part_info->total_sector = item->total_sectors;
            part_info->disk = disk;
        }
    }
    return 0;
}

static int identify_disk(disk_t * disk) {
    ata_send_cmd(disk, 0, 0, DISK_CMD_IDENTIFY);
    int err = inb(DISK_STATUS(disk));
    if (err == 0) {
        log_printf("%s doesn't exist\n", disk->name);
        return -1;
    }
    err = ata_wait_data(disk);
    if (err < 0) {
        log_printf("disk[%s]: read failed!\n", disk->name);
        return err;
    }

    uint16nd buf[256];
    ata_read_data(disk, buf, sizeof(buf));
    disk->sector_count = *(uint32nd *)(buf + 100);
    disk->sector_size = SECTOR_SIZE;

    partinfo_t * part = disk->partinfo + 0;
    part->disk = disk;
    kernel_sprintf(part->name, "%s%d", disk->name, 0);
    part->start_sector = 0;
    part->total_sector = disk->sector_count;
    part->type = FS_INVALID;

    detect_part_info(disk);
    return 0;
}

void disk_init(void) {
    log_printf("Checking disk...");
    kernel_memset(disk_buf, 0, sizeof(disk_buf));
    mutex_init(&mutex);
    sem_init(&op_sem, 0);

    for (int i = 0; i < DISK_PER_CHANNEL; i++) {
        disk_t * disk = disk_buf + i;
        kernel_sprintf(disk->name, "sd%c", i + 'a');
        disk->drive = (i == 0) ? DISK_DISK_MASTER : DISK_DISK_SLAVE;
        disk->port_base = IOBASE_PRIMARY;
        disk->mutex = &mutex;
        disk->op_sem = &op_sem;
        int err = identify_disk(disk);
        if (err == 0) {
            print_disk_info(disk);
        }
    }
}

int disk_open(device_t * dev) {
    int disk_idx = (dev->minor >> 4) - 0xa;
    int part_idx = dev->minor & 0xF;

    if ((disk_idx >= DISK_CNT) || (part_idx >= DISK_PRIMARY_PART_CNT)) {
        log_printf("device minor error: %d", dev->minor);
        return -1;
    }

    disk_t * disk = disk_buf + disk_idx;
    if (disk->sector_size == 0) {
        log_printf("disk not exist. device:sd%x", dev->minor);
        return -1;
    }

    partinfo_t * part_info = disk->partinfo + part_idx;
    if (part_info->total_sector == 0) {
        log_printf("part not exist. device:sd%x", dev->minor);
        return -1;
    }

    dev->data = part_info;
    irq_install(IRQ14_HARDDISK_PRIMARY, exception_handler_ide_primary);
    irq_enable(IRQ14_HARDDISK_PRIMARY);
    return 0;
}

int disk_read(device_t * dev, int start_sector, char * buf, int count) {
    partinfo_t * part_info = (partinfo_t *)dev->data;
    if (!part_info) {
        log_printf("Get part info failed! device = %d", dev->minor);
        return -1;
    }

    disk_t * disk = part_info->disk;
    if (disk == (disk_t *)0) {
        log_printf("No disk for device %d", dev->minor);
        return -1;
    }

    mutex_lock(disk->mutex);
    task_on_op = 1;
    int cnt;
    ata_send_cmd(disk, part_info->start_sector + start_sector, count, DISK_CMD_READ);
    for (cnt = 0; cnt < count; cnt++, buf += disk->sector_size) {
        if (task_current()) {
            sem_wait(disk->op_sem);
        }

        int err = ata_wait_data(disk);
        if (err < 0) {
            log_printf("disk(%s) read error: start sect %d, count %d", disk->name, start_sector, count);
            break;
        }
        ata_read_data(disk, buf, disk->sector_size);
    }

    mutex_unlock(disk->mutex);
    return cnt;
}

int disk_write(device_t * dev, int start_sector, char * buf, int count) {
    partinfo_t * part_info = (partinfo_t *)dev->data;
    if (!part_info) {
        log_printf("Get part info failed! device = %d", dev->minor);
        return -1;
    }

    disk_t * disk = part_info->disk;
    if (disk == (disk_t *)0) {
        log_printf("No disk for device %d", dev->minor);
        return -1;
    }

    mutex_lock(disk->mutex);
    task_on_op = 1;
    int cnt;
    ata_send_cmd(disk, part_info->start_sector + start_sector, count, DISK_CMD_WRITE);
    for (cnt = 0; cnt < count; cnt++, buf += disk->sector_size) {
        ata_write_data(disk, buf, disk->sector_size);
        if (task_current()) {
            sem_wait(disk->op_sem);
        }

        int err = ata_wait_data(disk);
        if (err < 0) {
            log_printf("disk(%s) write error: start sect %d, count %d", disk->name, start_sector, count);
            break;
        }
    }

    mutex_unlock(disk->mutex);
    return cnt;
}

int disk_control(device_t * dev, int cmd, int arg0, int arg1) {
    return 0;
}

void disk_close(device_t * dev) {
}

void do_handler_ide_primary(exception_frame_t * frame) {
    pic_send_eoi(IRQ14_HARDDISK_PRIMARY);
    if (task_on_op && task_current()) {
        sem_notify(&op_sem);
    }
}

dev_desc_t dev_disk_desc = {
    .name = "disk",
    .major = DEV_DISK,
    .open = disk_open,
    .read = disk_read,
    .write = disk_write,
    .control = disk_control,
    .close = disk_close,
};
