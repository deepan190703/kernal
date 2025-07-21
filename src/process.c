#include "kernel.h"
#include "vga.h"

// Process management
static process_t *current_process = NULL;
static process_t *process_list = NULL;
static u32 next_pid = 1;
static u32 process_count = 0;

// Simple round-robin scheduler
static process_t *ready_queue = NULL;

// Create a new process
process_t *create_process(const char *name, void (*entry_point)(void)) {
    process_t *process = (process_t *)kmalloc(sizeof(process_t));
    if (!process) return NULL;
    
    process->pid = next_pid++;
    process->state = PROCESS_READY;
    process->next = NULL;
    process->page_directory = NULL; // Simplified - using kernel space
    strcpy(process->name, name);
    
    // Allocate stack (4KB)
    void *stack = kmalloc(4096);
    if (!stack) {
        kfree(process);
        return NULL;
    }
    
    // Setup initial stack
    u32 *stack_top = (u32 *)((u8 *)stack + 4096);
    stack_top--; *stack_top = (u32)entry_point; // EIP
    stack_top--; *stack_top = 0x202; // EFLAGS (interrupts enabled)
    stack_top--; *stack_top = 0x08;  // CS
    stack_top--; *stack_top = 0;     // EAX
    stack_top--; *stack_top = 0;     // ECX
    stack_top--; *stack_top = 0;     // EDX
    stack_top--; *stack_top = 0;     // EBX
    stack_top--; *stack_top = (u32)stack_top + 16; // ESP
    stack_top--; *stack_top = (u32)stack_top + 20; // EBP
    stack_top--; *stack_top = 0;     // ESI
    stack_top--; *stack_top = 0;     // EDI
    
    process->esp = (u32)stack_top;
    process->ebp = (u32)stack_top + 20;
    process->eip = (u32)entry_point;
    
    // Add to process list
    if (!process_list) {
        process_list = process;
    } else {
        process_t *last = process_list;
        while (last->next) last = last->next;
        last->next = process;
    }
    
    // Add to ready queue
    if (!ready_queue) {
        ready_queue = process;
    } else {
        process_t *last = ready_queue;
        while (last->next) last = last->next;
        last->next = process;
    }
    
    process_count++;
    return process;
}

// Simple idle process
void idle_process(void) {
    while (1) {
        __asm__ volatile("hlt");
    }
}

// Simple test process
void test_process1(void) {
    static int counter = 0;
    while (1) {
        vga_printf("Process 1: %d\n", counter++);
        for (volatile int i = 0; i < 1000000; i++); // Simple delay
        if (counter > 5) break;
    }
}

void test_process2(void) {
    static int counter = 0;
    while (1) {
        vga_printf("Process 2: %d\n", counter++);
        for (volatile int i = 0; i < 1500000; i++); // Simple delay
        if (counter > 3) break;
    }
}

// Initialize process management
void process_init(void) {
    current_process = NULL;
    process_list = NULL;
    ready_queue = NULL;
    next_pid = 1;
    process_count = 0;
    
    // Create idle process
    create_process("idle", idle_process);
    
    // Create test processes
    create_process("test1", test_process1);
    create_process("test2", test_process2);
    
    vga_printf("Process management initialized with %d processes\n", process_count);
}

// Simple scheduler
void schedule(void) {
    if (!ready_queue) return;
    
    // Round-robin: move current to end of queue
    if (current_process && current_process->state == PROCESS_RUNNING) {
        current_process->state = PROCESS_READY;
        // Find current in ready queue and move to end
        if (ready_queue == current_process) {
            ready_queue = ready_queue->next;
            current_process->next = NULL;
            
            if (ready_queue) {
                process_t *last = ready_queue;
                while (last->next) last = last->next;
                last->next = current_process;
            } else {
                ready_queue = current_process;
            }
        }
    }
    
    // Select next process
    if (ready_queue) {
        current_process = ready_queue;
        current_process->state = PROCESS_RUNNING;
        // Context switch would go here in a real implementation
    }
}

// List all processes
void list_processes(void) {
    vga_printf("Process List:\n");
    vga_printf("PID\tName\t\tState\n");
    vga_printf("---\t----\t\t-----\n");
    
    process_t *proc = process_list;
    while (proc) {
        const char *state_str;
        switch (proc->state) {
            case PROCESS_READY: state_str = "READY"; break;
            case PROCESS_RUNNING: state_str = "RUNNING"; break;
            case PROCESS_BLOCKED: state_str = "BLOCKED"; break;
            case PROCESS_TERMINATED: state_str = "TERMINATED"; break;
            default: state_str = "UNKNOWN"; break;
        }
        
        vga_printf("%d\t%s\t\t%s\n", proc->pid, proc->name, state_str);
        proc = proc->next;
    }
    vga_printf("Total: %d processes\n", process_count);
}

// Get current process
process_t *get_current_process(void) {
    return current_process;
}

// Terminate a process
void terminate_process(u32 pid) {
    process_t *proc = process_list;
    process_t *prev = NULL;
    
    while (proc) {
        if (proc->pid == pid) {
            proc->state = PROCESS_TERMINATED;
            
            // Remove from process list
            if (prev) {
                prev->next = proc->next;
            } else {
                process_list = proc->next;
            }
            
            // Remove from ready queue
            if (ready_queue == proc) {
                ready_queue = proc->next;
            } else {
                process_t *q_prev = ready_queue;
                while (q_prev && q_prev->next != proc) {
                    q_prev = q_prev->next;
                }
                if (q_prev) {
                    q_prev->next = proc->next;
                }
            }
            
            process_count--;
            kfree(proc);
            return;
        }
        prev = proc;
        proc = proc->next;
    }
}