#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#define MULTIBOOT_HEADER_MAGIC 0x1BADB002

typedef struct multiboot_header {
    unsigned long magic;
    unsigned long flags;
    unsigned long checksum;
} multiboot_header_t;

#endif //MULTIBOOT_H
