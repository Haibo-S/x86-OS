# LBA48 Mode
#### Haibo

## Overview
LBA48 mode addresses all hardware disk sectors linearly, which removes the conventional concept of tracks, sectors, and cylinders for hard disk access. This method simplifies accessing disk contents by starting from sector 0.

## I/O Register Offsets

| Offset from "I/O" base | Direction | Function                    |
|------------------------|-----------|-----------------------------|
| 0                      | R/W       | Data Register               |
| 1                      | R         | Error Register              |
| 1                      | W         | Features Register           |
| 2                      | R/W       | Sector Count Register       |
| 3                      | R/W       | Sector Number Register (LBAlo) |
| 4                      | R/W       | Cylinder Low Register / (LBAmid) |
| 5                      | R/W       | Cylinder High Register / (LBAhi) |
| 6                      | R/W       | Drive / Head Register       |
| 7                      | R         | Status Register             |
| 7                      | W         | Command Register            |

## 0x1F6 Register Specification (Drive / Head Register)

| Bit   | Abbreviation | Function                                          |
|-------|--------------|---------------------------------------------------|
| 0-3   |              | bits 24 to 27 of the block number |
| 4     | DRV          | drive number                        |
| 5     | 1            |                                    |
| 6     | LBA          | For LBA mode, always set |
| 7     | 1            |                                 |

## Status Register (I/O base + 7)

| Bit   | Abbreviation | Function                                          |
|-------|--------------|---------------------------------------------------|
| 0     | ERR          | error |
| 1     | IDX          |                   |
| 2     | CORR         |            |
| 3     | DRQ          | ready bit |
| 4     | SRV          |            |
| 5     | DF           |          |
| 6     | RDY          | |
| 7     | BSY          | busy|


### References:
1. https://wiki.osdev.org/ATA_PIO_Mode#:~:text=According%20to%20the%20ATA%20specs,bus%20(not%20the%20memory).

