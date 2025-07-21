#include "kernel.h"
#include "vga.h"

// Timer state
static u32 timer_ticks = 0;
static u32 timer_frequency = 100; // 100 Hz default

// External function declarations
extern void register_interrupt_handler(u8 n, void (*handler)(void));

// Timer interrupt handler
void timer_handler(void) {
    timer_ticks++;
    
    // Simple scheduler trigger every 10 ticks (0.1 seconds at 100Hz)
    if (timer_ticks % 10 == 0) {
        schedule();
    }
}

// Initialize timer (Programmable Interval Timer)
void timer_init(void) {
    // Register timer interrupt handler (IRQ0 = interrupt 32)
    register_interrupt_handler(32, timer_handler);
    
    // Calculate divisor for desired frequency
    u32 divisor = 1193180 / timer_frequency;
    
    // Send command byte to PIT
    __asm__ volatile ("outb %%al, $0x43" : : "a"(0x36));
    
    // Send divisor
    __asm__ volatile ("outb %%al, $0x40" : : "a"((u8)(divisor & 0xFF)));
    __asm__ volatile ("outb %%al, $0x40" : : "a"((u8)((divisor >> 8) & 0xFF)));
    
    vga_printf("Timer initialized at %d Hz\n", timer_frequency);
}

// Get current timer ticks
u32 timer_get_ticks(void) {
    return timer_ticks;
}

// Get uptime in seconds
u32 timer_get_uptime(void) {
    return timer_ticks / timer_frequency;
}

// Sleep for specified ticks
void timer_sleep(u32 ticks) {
    u32 target = timer_ticks + ticks;
    while (timer_ticks < target) {
        __asm__ volatile ("hlt");
    }
}

// Sleep for specified milliseconds
void timer_sleep_ms(u32 ms) {
    u32 ticks = (ms * timer_frequency) / 1000;
    timer_sleep(ticks);
}

// Set timer frequency
void timer_set_frequency(u32 frequency) {
    if (frequency < 18) frequency = 18;   // Minimum safe frequency
    if (frequency > 1193180) frequency = 1193180; // Maximum frequency
    
    timer_frequency = frequency;
    
    // Recalculate and set divisor
    u32 divisor = 1193180 / frequency;
    
    __asm__ volatile ("outb %%al, $0x43" : : "a"(0x36));
    __asm__ volatile ("outb %%al, $0x40" : : "a"((u8)(divisor & 0xFF)));
    __asm__ volatile ("outb %%al, $0x40" : : "a"((u8)((divisor >> 8) & 0xFF)));
}