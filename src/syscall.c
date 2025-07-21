#include "kernel.h"
#include "vga.h"

// System call numbers
#define SYS_EXIT    1
#define SYS_WRITE   2
#define SYS_READ    3
#define SYS_GETPID  4
#define SYS_MALLOC  5
#define SYS_FREE    6
#define SYS_PS      7
#define SYS_MEMINFO 8

// System call handler
extern void syscall_handler(void);

// System call implementations
static int sys_exit(int status) {
    process_t *current = get_current_process();
    if (current) {
        vga_printf("Process %d (%s) exiting with status %d\n", 
                   current->pid, current->name, status);
        terminate_process(current->pid);
    }
    return 0;
}

static int sys_write(int fd, const char *buf, int len) {
    (void)fd; // Ignore file descriptor for now
    if (buf && len > 0) {
        for (int i = 0; i < len; i++) {
            vga_putchar(buf[i]);
        }
        return len;
    }
    return -1;
}

static int sys_read(int fd, char *buf, int len) {
    (void)fd; (void)buf; (void)len; // Not implemented yet
    return -1;
}

static int sys_getpid(void) {
    process_t *current = get_current_process();
    return current ? current->pid : 0;
}

static void *sys_malloc(u32 size) {
    return kmalloc(size);
}

static int sys_free(void *ptr) {
    kfree(ptr);
    return 0;
}

static int sys_ps(void) {
    list_processes();
    return 0;
}

static int sys_meminfo(void) {
    memory_stats();
    return 0;
}

// Main system call dispatcher
int syscall_dispatcher(int syscall_num, int arg1, int arg2, int arg3) {
    switch (syscall_num) {
        case SYS_EXIT:
            return sys_exit(arg1);
        case SYS_WRITE:
            return sys_write(arg1, (const char *)arg2, arg3);
        case SYS_READ:
            return sys_read(arg1, (char *)arg2, arg3);
        case SYS_GETPID:
            return sys_getpid();
        case SYS_MALLOC:
            return (int)sys_malloc((u32)arg1);
        case SYS_FREE:
            return sys_free((void *)arg1);
        case SYS_PS:
            return sys_ps();
        case SYS_MEMINFO:
            return sys_meminfo();
        default:
            vga_printf("Unknown system call: %d\n", syscall_num);
            return -1;
    }
}

// Initialize system calls
void syscall_init(void) {
    // Set up system call interrupt (int 0x80)
    extern void idt_set_gate(u8 num, u32 base, u16 sel, u8 flags);
    idt_set_gate(0x80, (u32)syscall_handler, 0x08, 0x8E);
    
    vga_printf("System calls initialized (interrupt 0x80)\n");
}

// User space wrappers (these would normally be in libc)
int exit(int status) {
    int result;
    __asm__ volatile (
        "movl $1, %%eax\n"    // SYS_EXIT
        "movl %1, %%ebx\n"    // status
        "int $0x80\n"         // syscall
        "movl %%eax, %0\n"    // result
        : "=m" (result)
        : "m" (status)
        : "eax", "ebx"
    );
    return result;
}

int write(int fd, const char *buf, int len) {
    int result;
    __asm__ volatile (
        "movl $2, %%eax\n"    // SYS_WRITE
        "movl %1, %%ebx\n"    // fd
        "movl %2, %%ecx\n"    // buf
        "movl %3, %%edx\n"    // len
        "int $0x80\n"         // syscall
        "movl %%eax, %0\n"    // result
        : "=m" (result)
        : "m" (fd), "m" (buf), "m" (len)
        : "eax", "ebx", "ecx", "edx"
    );
    return result;
}

int getpid(void) {
    int result;
    __asm__ volatile (
        "movl $4, %%eax\n"    // SYS_GETPID
        "int $0x80\n"         // syscall
        "movl %%eax, %0\n"    // result
        : "=m" (result)
        :
        : "eax"
    );
    return result;
}

void *malloc(u32 size) {
    void *result;
    __asm__ volatile (
        "movl $5, %%eax\n"    // SYS_MALLOC
        "movl %1, %%ebx\n"    // size
        "int $0x80\n"         // syscall
        "movl %%eax, %0\n"    // result
        : "=m" (result)
        : "m" (size)
        : "eax", "ebx"
    );
    return result;
}

int free(void *ptr) {
    int result;
    __asm__ volatile (
        "movl $6, %%eax\n"    // SYS_FREE
        "movl %1, %%ebx\n"    // ptr
        "int $0x80\n"         // syscall
        "movl %%eax, %0\n"    // result
        : "=m" (result)
        : "m" (ptr)
        : "eax", "ebx"
    );
    return result;
}

int ps(void) {
    int result;
    __asm__ volatile (
        "movl $7, %%eax\n"    // SYS_PS
        "int $0x80\n"         // syscall
        "movl %%eax, %0\n"    // result
        : "=m" (result)
        :
        : "eax"
    );
    return result;
}

int meminfo(void) {
    int result;
    __asm__ volatile (
        "movl $8, %%eax\n"    // SYS_MEMINFO
        "int $0x80\n"         // syscall
        "movl %%eax, %0\n"    // result
        : "=m" (result)
        :
        : "eax"
    );
    return result;
}