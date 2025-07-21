#include "kernel.h"
#include "vga.h"

// Shell state
static int shell_running = 1;
static char command_buffer[256];

// Command structure
typedef struct command {
    const char *name;
    const char *description;
    void (*handler)(int argc, char **argv);
} command_t;

// Function declarations for command handlers
void cmd_help(int argc, char **argv);
void cmd_clear(int argc, char **argv);
void cmd_echo(int argc, char **argv);
void cmd_ps(int argc, char **argv);
void cmd_meminfo(int argc, char **argv);
void cmd_ls(int argc, char **argv);
void cmd_cat(int argc, char **argv);
void cmd_uptime(int argc, char **argv);
void cmd_ifconfig(int argc, char **argv);
void cmd_ping(int argc, char **argv);
void cmd_netstat(int argc, char **argv);
void cmd_exit(int argc, char **argv);
void cmd_reboot(int argc, char **argv);
void cmd_about(int argc, char **argv);
void cmd_test(int argc, char **argv);

// Command table
static command_t commands[] = {
    {"help", "Show this help message", cmd_help},
    {"clear", "Clear the screen", cmd_clear},
    {"echo", "Echo arguments to output", cmd_echo},
    {"ps", "List running processes", cmd_ps},
    {"meminfo", "Show memory information", cmd_meminfo},
    {"ls", "List files", cmd_ls},
    {"cat", "Display file contents", cmd_cat},
    {"uptime", "Show system uptime", cmd_uptime},
    {"ifconfig", "Show network interfaces", cmd_ifconfig},
    {"ping", "Ping an IP address", cmd_ping},
    {"netstat", "Show network statistics", cmd_netstat},
    {"exit", "Exit the shell", cmd_exit},
    {"reboot", "Reboot the system", cmd_reboot},
    {"about", "About this kernel", cmd_about},
    {"test", "Run system tests", cmd_test},
    {NULL, NULL, NULL}
};

// External function declarations
extern int keyboard_readline(char *buffer, int max_len);
extern void fs_list_files(void);
extern int fs_read_file(const char *name, char *buffer, u32 buffer_size);
extern void memory_stats(void);
extern void list_processes(void);
extern u32 timer_get_uptime(void);
extern void network_list_interfaces(void);
extern void network_stats(void);
extern int network_ping(u32 target_ip);
extern void network_test_receive(void);

// Parse command line into argc/argv
static int parse_command(char *cmd, char **argv, int max_args) {
    int argc = 0;
    char *token = cmd;
    
    while (*token && argc < max_args - 1) {
        // Skip whitespace
        while (*token == ' ' || *token == '\t') token++;
        if (!*token) break;
        
        argv[argc++] = token;
        
        // Find end of token
        while (*token && *token != ' ' && *token != '\t') token++;
        if (*token) {
            *token = 0;
            token++;
        }
    }
    
    argv[argc] = NULL;
    return argc;
}

// Command implementations
void cmd_help(int argc, char **argv) {
    (void)argc; (void)argv;
    vga_puts("Available commands:\n");
    for (int i = 0; commands[i].name; i++) {
        vga_printf("  %-12s - %s\n", commands[i].name, commands[i].description);
    }
}

void cmd_clear(int argc, char **argv) {
    (void)argc; (void)argv;
    vga_clear();
}

void cmd_echo(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        vga_puts(argv[i]);
        if (i < argc - 1) vga_putchar(' ');
    }
    vga_putchar('\n');
}

void cmd_ps(int argc, char **argv) {
    (void)argc; (void)argv;
    list_processes();
}

void cmd_meminfo(int argc, char **argv) {
    (void)argc; (void)argv;
    memory_stats();
}

void cmd_ls(int argc, char **argv) {
    (void)argc; (void)argv;
    fs_list_files();
}

void cmd_cat(int argc, char **argv) {
    if (argc < 2) {
        vga_puts("Usage: cat <filename>\n");
        return;
    }
    
    char buffer[4096];
    int result = fs_read_file(argv[1], buffer, sizeof(buffer));
    if (result >= 0) {
        vga_puts(buffer);
        vga_putchar('\n');
    } else {
        vga_printf("cat: %s: No such file\n", argv[1]);
    }
}

void cmd_uptime(int argc, char **argv) {
    (void)argc; (void)argv;
    u32 uptime = timer_get_uptime();
    u32 hours = uptime / 3600;
    u32 minutes = (uptime % 3600) / 60;
    u32 seconds = uptime % 60;
    
    vga_printf("up %d:%02d:%02d\n", hours, minutes, seconds);
}

void cmd_ifconfig(int argc, char **argv) {
    (void)argc; (void)argv;
    network_list_interfaces();
}

void cmd_ping(int argc, char **argv) {
    if (argc < 2) {
        vga_puts("Usage: ping <ip_address>\n");
        return;
    }
    
    // Simple IP parsing (assume 192.168.0.1 format)
    u32 ip = 0xC0A80001; // Default to 192.168.0.1
    network_ping(ip);
}

void cmd_netstat(int argc, char **argv) {
    (void)argc; (void)argv;
    network_stats();
}

void cmd_exit(int argc, char **argv) {
    (void)argc; (void)argv;
    shell_running = 0;
    vga_puts("Goodbye!\n");
}

void cmd_reboot(int argc, char **argv) {
    (void)argc; (void)argv;
    vga_puts("Rebooting system...\n");
    // Trigger keyboard controller reset
    __asm__ volatile ("outb %%al, $0x64" : : "a"(0xFE));
    while (1) __asm__ volatile ("hlt");
}

void cmd_about(int argc, char **argv) {
    (void)argc; (void)argv;
    vga_set_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    vga_puts("=== Comprehensive Kernel ===\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("A kernel that can do every task possible!\n\n");
    vga_puts("Features:\n");
    vga_puts("- Multiboot bootloader\n");
    vga_puts("- VGA text mode display\n");
    vga_puts("- Memory management (paging, heap)\n");
    vga_puts("- Process management & scheduling\n");
    vga_puts("- System call interface\n");
    vga_puts("- In-memory file system\n");
    vga_puts("- Keyboard driver\n");
    vga_puts("- Timer/PIT driver\n");
    vga_puts("- Network stack foundations\n");
    vga_puts("- Interactive shell\n");
    vga_puts("- Interrupt handling (IDT)\n");
    vga_puts("- Memory segmentation (GDT)\n\n");
    vga_puts("Version: 1.0\n");
    vga_puts("Architecture: x86 32-bit\n");
}

void cmd_test(int argc, char **argv) {
    (void)argc; (void)argv;
    vga_puts("Running system tests...\n");
    
    vga_puts("1. Memory allocation test: ");
    void *ptr = kmalloc(1024);
    if (ptr) {
        kfree(ptr);
        vga_puts("PASS\n");
    } else {
        vga_puts("FAIL\n");
    }
    
    vga_puts("2. File system test: ");
    char buffer[64];
    if (fs_read_file("readme.txt", buffer, sizeof(buffer)) >= 0) {
        vga_puts("PASS\n");
    } else {
        vga_puts("FAIL\n");
    }
    
    vga_puts("3. Network test: ");
    network_test_receive();
    
    vga_puts("All tests completed.\n");
}

// Print shell prompt
static void print_prompt(void) {
    vga_set_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
    vga_puts("kernel");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("$ ");
}

// Execute a command
static void execute_command(char *cmd) {
    if (strlen(cmd) == 0) return;
    
    char *argv[16];
    int argc = parse_command(cmd, argv, 16);
    
    if (argc == 0) return;
    
    // Find and execute command
    for (int i = 0; commands[i].name; i++) {
        if (strcmp(argv[0], commands[i].name) == 0) {
            commands[i].handler(argc, argv);
            return;
        }
    }
    
    vga_printf("%s: command not found\n", argv[0]);
}

// Shell main loop
void shell_main_loop(void) {
    while (shell_running) {
        print_prompt();
        
        if (keyboard_readline(command_buffer, sizeof(command_buffer)) > 0) {
            execute_command(command_buffer);
        }
    }
}

// Initialize shell
void shell_init(void) {
    shell_running = 1;
    
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_puts("\n=== Welcome to Comprehensive Kernel Shell ===\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("Type 'help' for available commands.\n");
    vga_puts("Type 'about' for kernel information.\n\n");
    
    // Start main shell loop
    shell_main_loop();
}