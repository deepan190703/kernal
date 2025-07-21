#include "kernel.h"
#include "vga.h"

// IDT entry structure
struct idt_entry {
    u16 base_lo;    // Lower 16 bits of handler address
    u16 sel;        // Kernel segment selector
    u8  always0;    // Always 0
    u8  flags;      // Flags
    u16 base_hi;    // Upper 16 bits of handler address
} __attribute__((packed));

// IDT pointer structure
struct idt_ptr {
    u16 limit;
    u32 base;
} __attribute__((packed));

// IDT table with 256 entries
static struct idt_entry idt_entries[256];
static struct idt_ptr idt_pointer;

// External assembly functions
extern void idt_flush(u32);
extern void isr0(void);  extern void isr1(void);  extern void isr2(void);  extern void isr3(void);
extern void isr4(void);  extern void isr5(void);  extern void isr6(void);  extern void isr7(void);
extern void isr8(void);  extern void isr9(void);  extern void isr10(void); extern void isr11(void);
extern void isr12(void); extern void isr13(void); extern void isr14(void); extern void isr15(void);
extern void isr16(void); extern void isr17(void); extern void isr18(void); extern void isr19(void);
extern void isr20(void); extern void isr21(void); extern void isr22(void); extern void isr23(void);
extern void isr24(void); extern void isr25(void); extern void isr26(void); extern void isr27(void);
extern void isr28(void); extern void isr29(void); extern void isr30(void); extern void isr31(void);

extern void irq0(void);  extern void irq1(void);  extern void irq2(void);  extern void irq3(void);
extern void irq4(void);  extern void irq5(void);  extern void irq6(void);  extern void irq7(void);
extern void irq8(void);  extern void irq9(void);  extern void irq10(void); extern void irq11(void);
extern void irq12(void); extern void irq13(void); extern void irq14(void); extern void irq15(void);

// Interrupt handler typedef
typedef void (*interrupt_handler_t)(void);

// Interrupt handlers array
static interrupt_handler_t interrupt_handlers[256];

// Set up an IDT entry
void idt_set_gate(u8 num, u32 base, u16 sel, u8 flags) {
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;
    idt_entries[num].sel     = sel;
    idt_entries[num].always0 = 0;
    idt_entries[num].flags   = flags;
}

// Initialize the IDT
void idt_init(void) {
    idt_pointer.limit = sizeof(struct idt_entry) * 256 - 1;
    idt_pointer.base  = (u32)&idt_entries;

    memset(&idt_entries, 0, sizeof(struct idt_entry) * 256);
    memset(&interrupt_handlers, 0, sizeof(interrupt_handler_t) * 256);

    // Remap the IRQ table
    // Initialize PIC
    // ICW1 - begin initialization
    __asm__ volatile ("outb %%al, $0x20" : : "a"(0x11));
    __asm__ volatile ("outb %%al, $0xA0" : : "a"(0x11));
    
    // ICW2 - remap offset address of IDT
    __asm__ volatile ("outb %%al, $0x21" : : "a"(0x20)); // Master PIC offset
    __asm__ volatile ("outb %%al, $0xA1" : : "a"(0x28)); // Slave PIC offset
    
    // ICW3 - setup cascade
    __asm__ volatile ("outb %%al, $0x21" : : "a"(0x04));
    __asm__ volatile ("outb %%al, $0xA1" : : "a"(0x02));
    
    // ICW4 - environment info
    __asm__ volatile ("outb %%al, $0x21" : : "a"(0x01));
    __asm__ volatile ("outb %%al, $0xA1" : : "a"(0x01));
    
    // Mask interrupts
    __asm__ volatile ("outb %%al, $0x21" : : "a"(0x00));
    __asm__ volatile ("outb %%al, $0xA1" : : "a"(0x00));

    // Set up ISRs (Interrupt Service Routines)
    idt_set_gate(0,  (u32)isr0,  0x08, 0x8E);
    idt_set_gate(1,  (u32)isr1,  0x08, 0x8E);
    idt_set_gate(2,  (u32)isr2,  0x08, 0x8E);
    idt_set_gate(3,  (u32)isr3,  0x08, 0x8E);
    idt_set_gate(4,  (u32)isr4,  0x08, 0x8E);
    idt_set_gate(5,  (u32)isr5,  0x08, 0x8E);
    idt_set_gate(6,  (u32)isr6,  0x08, 0x8E);
    idt_set_gate(7,  (u32)isr7,  0x08, 0x8E);
    idt_set_gate(8,  (u32)isr8,  0x08, 0x8E);
    idt_set_gate(9,  (u32)isr9,  0x08, 0x8E);
    idt_set_gate(10, (u32)isr10, 0x08, 0x8E);
    idt_set_gate(11, (u32)isr11, 0x08, 0x8E);
    idt_set_gate(12, (u32)isr12, 0x08, 0x8E);
    idt_set_gate(13, (u32)isr13, 0x08, 0x8E);
    idt_set_gate(14, (u32)isr14, 0x08, 0x8E);
    idt_set_gate(15, (u32)isr15, 0x08, 0x8E);
    idt_set_gate(16, (u32)isr16, 0x08, 0x8E);
    idt_set_gate(17, (u32)isr17, 0x08, 0x8E);
    idt_set_gate(18, (u32)isr18, 0x08, 0x8E);
    idt_set_gate(19, (u32)isr19, 0x08, 0x8E);
    idt_set_gate(20, (u32)isr20, 0x08, 0x8E);
    idt_set_gate(21, (u32)isr21, 0x08, 0x8E);
    idt_set_gate(22, (u32)isr22, 0x08, 0x8E);
    idt_set_gate(23, (u32)isr23, 0x08, 0x8E);
    idt_set_gate(24, (u32)isr24, 0x08, 0x8E);
    idt_set_gate(25, (u32)isr25, 0x08, 0x8E);
    idt_set_gate(26, (u32)isr26, 0x08, 0x8E);
    idt_set_gate(27, (u32)isr27, 0x08, 0x8E);
    idt_set_gate(28, (u32)isr28, 0x08, 0x8E);
    idt_set_gate(29, (u32)isr29, 0x08, 0x8E);
    idt_set_gate(30, (u32)isr30, 0x08, 0x8E);
    idt_set_gate(31, (u32)isr31, 0x08, 0x8E);

    // Set up IRQs (Hardware Interrupts)
    idt_set_gate(32, (u32)irq0,  0x08, 0x8E);
    idt_set_gate(33, (u32)irq1,  0x08, 0x8E);
    idt_set_gate(34, (u32)irq2,  0x08, 0x8E);
    idt_set_gate(35, (u32)irq3,  0x08, 0x8E);
    idt_set_gate(36, (u32)irq4,  0x08, 0x8E);
    idt_set_gate(37, (u32)irq5,  0x08, 0x8E);
    idt_set_gate(38, (u32)irq6,  0x08, 0x8E);
    idt_set_gate(39, (u32)irq7,  0x08, 0x8E);
    idt_set_gate(40, (u32)irq8,  0x08, 0x8E);
    idt_set_gate(41, (u32)irq9,  0x08, 0x8E);
    idt_set_gate(42, (u32)irq10, 0x08, 0x8E);
    idt_set_gate(43, (u32)irq11, 0x08, 0x8E);
    idt_set_gate(44, (u32)irq12, 0x08, 0x8E);
    idt_set_gate(45, (u32)irq13, 0x08, 0x8E);
    idt_set_gate(46, (u32)irq14, 0x08, 0x8E);
    idt_set_gate(47, (u32)irq15, 0x08, 0x8E);

    idt_flush((u32)&idt_pointer);
}

// Register an interrupt handler
void register_interrupt_handler(u8 n, interrupt_handler_t handler) {
    interrupt_handlers[n] = handler;
}

// Common ISR handler
void isr_handler(u32 interrupt_number, u32 error_code) {
    if (interrupt_handlers[interrupt_number] != 0) {
        interrupt_handlers[interrupt_number]();
    } else {
        vga_printf("Unhandled ISR: %d, Error Code: %x\n", interrupt_number, error_code);
    }
}

// Common IRQ handler
void irq_handler(u32 interrupt_number) {
    // Send EOI to PICs
    if (interrupt_number >= 40) {
        __asm__ volatile ("outb %%al, $0xA0" : : "a"(0x20)); // Slave
    }
    __asm__ volatile ("outb %%al, $0x20" : : "a"(0x20)); // Master

    if (interrupt_handlers[interrupt_number] != 0) {
        interrupt_handlers[interrupt_number]();
    }
}