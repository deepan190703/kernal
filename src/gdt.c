#include "kernel.h"

// GDT entry structure
struct gdt_entry {
    u16 limit_low;
    u16 base_low;
    u8  base_middle;
    u8  access;
    u8  granularity;
    u8  base_high;
} __attribute__((packed));

// GDT pointer structure
struct gdt_ptr {
    u16 limit;
    u32 base;
} __attribute__((packed));

// GDT table with 5 entries
static struct gdt_entry gdt_entries[5];
static struct gdt_ptr gdt_pointer;

// External assembly function to load GDT
extern void gdt_flush(u32);

// Set up a GDT entry
static void gdt_set_gate(s32 num, u32 base, u32 limit, u8 access, u8 gran) {
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

// Initialize the GDT
void gdt_init(void) {
    gdt_pointer.limit = (sizeof(struct gdt_entry) * 5) - 1;
    gdt_pointer.base  = (u32)&gdt_entries;

    // NULL descriptor
    gdt_set_gate(0, 0, 0, 0, 0);
    
    // Kernel code segment - base 0, limit 4GB, present, ring 0, executable, readable
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    
    // Kernel data segment - base 0, limit 4GB, present, ring 0, writable
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    
    // User code segment - base 0, limit 4GB, present, ring 3, executable, readable
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    
    // User data segment - base 0, limit 4GB, present, ring 3, writable
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    gdt_flush((u32)&gdt_pointer);
}