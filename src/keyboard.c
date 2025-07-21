#include "kernel.h"
#include "vga.h"

// Keyboard scan codes
static char scancode_to_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Keyboard state
static int shift_pressed = 0;
static int ctrl_pressed = 0;
static int alt_pressed = 0;

// Keyboard buffer
#define KEYBOARD_BUFFER_SIZE 256
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static int buffer_start = 0;
static int buffer_end = 0;
static int buffer_count = 0;

// External function declarations
extern void register_interrupt_handler(u8 n, void (*handler)(void));

// Add character to keyboard buffer
static void keyboard_buffer_add(char c) {
    if (buffer_count < KEYBOARD_BUFFER_SIZE) {
        keyboard_buffer[buffer_end] = c;
        buffer_end = (buffer_end + 1) % KEYBOARD_BUFFER_SIZE;
        buffer_count++;
    }
}

// Get character from keyboard buffer
char keyboard_getchar(void) {
    if (buffer_count == 0) return 0;
    
    char c = keyboard_buffer[buffer_start];
    buffer_start = (buffer_start + 1) % KEYBOARD_BUFFER_SIZE;
    buffer_count--;
    return c;
}

// Check if keyboard buffer has data
int keyboard_has_data(void) {
    return buffer_count > 0;
}

// Keyboard interrupt handler
void keyboard_handler(void) {
    u8 scancode;
    
    // Read scan code from keyboard controller
    __asm__ volatile ("inb $0x60, %0" : "=a"(scancode));
    
    // Handle key release (bit 7 set)
    if (scancode & 0x80) {
        scancode &= 0x7F; // Remove release bit
        
        // Handle modifier keys
        switch (scancode) {
            case 0x2A: // Left shift
            case 0x36: // Right shift
                shift_pressed = 0;
                break;
            case 0x1D: // Ctrl
                ctrl_pressed = 0;
                break;
            case 0x38: // Alt
                alt_pressed = 0;
                break;
        }
        return;
    }
    
    // Handle key press
    switch (scancode) {
        case 0x2A: // Left shift
        case 0x36: // Right shift
            shift_pressed = 1;
            break;
        case 0x1D: // Ctrl
            ctrl_pressed = 1;
            break;
        case 0x38: // Alt
            alt_pressed = 1;
            break;
        default:
            if (scancode < 128) {
                char c = scancode_to_ascii[scancode];
                if (c) {
                    // Handle shift modifier
                    if (shift_pressed) {
                        if (c >= 'a' && c <= 'z') {
                            c = c - 'a' + 'A';
                        } else {
                            // Handle special characters with shift
                            switch (c) {
                                case '1': c = '!'; break;
                                case '2': c = '@'; break;
                                case '3': c = '#'; break;
                                case '4': c = '$'; break;
                                case '5': c = '%'; break;
                                case '6': c = '^'; break;
                                case '7': c = '&'; break;
                                case '8': c = '*'; break;
                                case '9': c = '('; break;
                                case '0': c = ')'; break;
                                case '-': c = '_'; break;
                                case '=': c = '+'; break;
                                case '[': c = '{'; break;
                                case ']': c = '}'; break;
                                case '\\': c = '|'; break;
                                case ';': c = ':'; break;
                                case '\'': c = '"'; break;
                                case '`': c = '~'; break;
                                case ',': c = '<'; break;
                                case '.': c = '>'; break;
                                case '/': c = '?'; break;
                            }
                        }
                    }
                    
                    // Handle ctrl modifier
                    if (ctrl_pressed && c >= 'a' && c <= 'z') {
                        c = c - 'a' + 1; // Ctrl+A = 1, etc.
                    }
                    
                    keyboard_buffer_add(c);
                }
            }
            break;
    }
}

// Initialize keyboard driver
void keyboard_init(void) {
    buffer_start = 0;
    buffer_end = 0;
    buffer_count = 0;
    shift_pressed = 0;
    ctrl_pressed = 0;
    alt_pressed = 0;
    
    // Register keyboard interrupt handler (IRQ1 = interrupt 33)
    register_interrupt_handler(33, keyboard_handler);
    
    vga_printf("Keyboard driver initialized\n");
}

// Read a line of input
int keyboard_readline(char *buffer, int max_len) {
    int pos = 0;
    
    while (pos < max_len - 1) {
        if (keyboard_has_data()) {
            char c = keyboard_getchar();
            
            if (c == '\n' || c == '\r') {
                buffer[pos] = 0;
                vga_putchar('\n');
                return pos;
            } else if (c == '\b') {
                if (pos > 0) {
                    pos--;
                    vga_putchar('\b');
                }
            } else if (c >= 32 && c <= 126) { // Printable characters
                buffer[pos++] = c;
                vga_putchar(c);
            }
        } else {
            // Yield CPU while waiting for input
            __asm__ volatile ("hlt");
        }
    }
    
    buffer[pos] = 0;
    return pos;
}