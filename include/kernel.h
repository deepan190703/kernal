#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stddef.h>

// Basic type definitions
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

// Multiboot information structure
struct multiboot_info {
    u32 flags;
    u32 mem_lower;
    u32 mem_upper;
    u32 boot_device;
    u32 cmdline;
    u32 mods_count;
    u32 mods_addr;
    u32 syms[4];
    u32 mmap_length;
    u32 mmap_addr;
    u32 drives_length;
    u32 drives_addr;
    u32 config_table;
    u32 boot_loader_name;
    u32 apm_table;
    u32 vbe_control_info;
    u32 vbe_mode_info;
    u16 vbe_mode;
    u16 vbe_interface_seg;
    u16 vbe_interface_off;
    u16 vbe_interface_len;
};

// System constants
#define KERNEL_VIRTUAL_BASE 0xC0000000
#define KERNEL_PAGE_NUMBER  (KERNEL_VIRTUAL_BASE >> 22)

// Memory constants
#define PAGE_SIZE 4096
#define HEAP_START 0x00100000
#define HEAP_INITIAL_SIZE 0x00100000

// Process states
typedef enum {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED
} process_state_t;

// Process control block
typedef struct process {
    u32 pid;
    u32 esp;
    u32 ebp;
    u32 eip;
    process_state_t state;
    struct process *next;
    void *page_directory;
    char name[64];
} process_t;

// Function declarations
void kernel_main(u32 multiboot_magic, struct multiboot_info *mbi);
void kernel_panic(const char *message);

// Component initialization functions
void vga_init(void);
void gdt_init(void);
void idt_init(void);
void memory_init(struct multiboot_info *mbi);
void process_init(void);
void syscall_init(void);
void filesystem_init(void);
void keyboard_init(void);
void timer_init(void);
void shell_init(void);
void network_init(void);

// Utility functions
void *memset(void *dest, int val, size_t len);
void *memcpy(void *dest, const void *src, size_t len);
int memcmp(const void *s1, const void *s2, size_t n);
size_t strlen(const char *str);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);

#endif // KERNEL_H