#include "vga.h"
#include "kernel.h"

// VGA state
static u16 *vga_buffer;
static size_t vga_row;
static size_t vga_column;
static u8 vga_color;

// Helper function to create VGA entry
static inline u16 vga_entry(unsigned char uc, u8 color) {
    return (u16)uc | (u16)color << 8;
}

// Helper function to create VGA color
static inline u8 vga_entry_color(vga_color_t fg, vga_color_t bg) {
    return fg | bg << 4;
}

// Initialize VGA
void vga_init(void) {
    vga_row = 0;
    vga_column = 0;
    vga_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_buffer = (u16 *)VGA_MEMORY;
    vga_clear();
}

// Clear the screen
void vga_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            vga_buffer[index] = vga_entry(' ', vga_color);
        }
    }
    vga_row = 0;
    vga_column = 0;
}

// Set foreground and background colors
void vga_set_color(vga_color_t fg, vga_color_t bg) {
    vga_color = vga_entry_color(fg, bg);
}

// Put character at specific position
static void vga_putentryat(char c, u8 color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    vga_buffer[index] = vga_entry(c, color);
}

// Scroll screen up by one line
void vga_scroll(void) {
    // Move all lines up by one
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[(y - 1) * VGA_WIDTH + x] = vga_buffer[y * VGA_WIDTH + x];
        }
    }
    
    // Clear the last line
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', vga_color);
    }
    
    if (vga_row > 0) {
        vga_row--;
    }
}

// Put a single character
void vga_putchar(char c) {
    if (c == '\n') {
        vga_column = 0;
        if (++vga_row == VGA_HEIGHT) {
            vga_scroll();
        }
        return;
    }
    
    if (c == '\t') {
        vga_column = (vga_column + 8) & ~7; // Align to 8-character boundaries
        if (vga_column >= VGA_WIDTH) {
            vga_column = 0;
            if (++vga_row == VGA_HEIGHT) {
                vga_scroll();
            }
        }
        return;
    }
    
    if (c == '\r') {
        vga_column = 0;
        return;
    }
    
    if (c == '\b') {
        if (vga_column > 0) {
            vga_column--;
            vga_putentryat(' ', vga_color, vga_column, vga_row);
        }
        return;
    }
    
    vga_putentryat(c, vga_color, vga_column, vga_row);
    
    if (++vga_column == VGA_WIDTH) {
        vga_column = 0;
        if (++vga_row == VGA_HEIGHT) {
            vga_scroll();
        }
    }
}

// Put a string
void vga_puts(const char *str) {
    while (*str) {
        vga_putchar(*str++);
    }
}

// Simple printf implementation
void vga_printf(const char *format, ...) {
    char buffer[1024];
    char *str;
    int num;
    unsigned int unum;
    
    // Simple va_list simulation for basic printf
    u32 *args = (u32 *)(&format + 1);
    int arg_index = 0;
    
    while (*format) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 's':
                    str = (char *)args[arg_index++];
                    vga_puts(str ? str : "(null)");
                    break;
                    
                case 'd':
                    num = (int)args[arg_index++];
                    if (num < 0) {
                        vga_putchar('-');
                        num = -num;
                    }
                    // Convert to string and print
                    {
                        char numstr[16];
                        int i = 0;
                        if (num == 0) {
                            numstr[i++] = '0';
                        } else {
                            while (num > 0) {
                                numstr[i++] = '0' + (num % 10);
                                num /= 10;
                            }
                        }
                        // Reverse the string
                        for (int j = i - 1; j >= 0; j--) {
                            vga_putchar(numstr[j]);
                        }
                    }
                    break;
                    
                case 'x':
                    unum = args[arg_index++];
                    vga_puts("0x");
                    // Convert to hex string
                    {
                        char hexstr[16];
                        int i = 0;
                        if (unum == 0) {
                            hexstr[i++] = '0';
                        } else {
                            while (unum > 0) {
                                int digit = unum % 16;
                                hexstr[i++] = digit < 10 ? '0' + digit : 'a' + digit - 10;
                                unum /= 16;
                            }
                        }
                        // Reverse the string
                        for (int j = i - 1; j >= 0; j--) {
                            vga_putchar(hexstr[j]);
                        }
                    }
                    break;
                    
                case 'c':
                    vga_putchar((char)args[arg_index++]);
                    break;
                    
                case '%':
                    vga_putchar('%');
                    break;
                    
                default:
                    vga_putchar('%');
                    vga_putchar(*format);
                    break;
            }
        } else {
            vga_putchar(*format);
        }
        format++;
    }
}