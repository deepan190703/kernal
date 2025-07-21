#include "kernel.h"
#include "vga.h"

// Memory management structures
static u32 *heap_start = (u32 *)HEAP_START;
static u32 heap_size = HEAP_INITIAL_SIZE;
static u32 heap_used = 0;

// Simple memory block header
typedef struct mem_block {
    u32 size;
    u32 used;
    struct mem_block *next;
} mem_block_t;

static mem_block_t *first_block = NULL;

// Page directory and tables (simplified)
static u32 page_directory[1024] __attribute__((aligned(4096)));
static u32 page_table[1024] __attribute__((aligned(4096)));

// Initialize memory management
void memory_init(struct multiboot_info *mbi) {
    // Initialize heap
    first_block = (mem_block_t *)heap_start;
    first_block->size = heap_size - sizeof(mem_block_t);
    first_block->used = 0;
    first_block->next = NULL;
    
    // Setup basic paging (identity mapping for now)
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0x00000002; // Not present, writable, supervisor
    }
    
    // Map first 4MB
    for (int i = 0; i < 1024; i++) {
        page_table[i] = (i * 0x1000) | 3; // Present, writable
    }
    
    page_directory[0] = ((u32)page_table) | 3; // Present, writable
    
    // Load page directory
    __asm__ volatile("mov %0, %%cr3" :: "r"(&page_directory));
    
    // Enable paging
    u32 cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; // Set PG bit
    __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));
    
    vga_printf("Memory: %d KB lower, %d KB upper\n", 
               mbi->mem_lower, mbi->mem_upper);
    vga_printf("Heap initialized at 0x%x, size %d KB\n", 
               (u32)heap_start, heap_size / 1024);
}

// Simple malloc implementation
void *kmalloc(u32 size) {
    if (size == 0) return NULL;
    
    // Align size to 4 bytes
    size = (size + 3) & ~3;
    
    mem_block_t *current = first_block;
    
    while (current) {
        if (!current->used && current->size >= size) {
            // Split block if necessary
            if (current->size > size + sizeof(mem_block_t)) {
                mem_block_t *new_block = (mem_block_t *)((u8 *)current + sizeof(mem_block_t) + size);
                new_block->size = current->size - size - sizeof(mem_block_t);
                new_block->used = 0;
                new_block->next = current->next;
                current->next = new_block;
                current->size = size;
            }
            
            current->used = 1;
            heap_used += size;
            return (void *)((u8 *)current + sizeof(mem_block_t));
        }
        current = current->next;
    }
    
    return NULL; // Out of memory
}

// Simple free implementation
void kfree(void *ptr) {
    if (!ptr) return;
    
    mem_block_t *block = (mem_block_t *)((u8 *)ptr - sizeof(mem_block_t));
    block->used = 0;
    heap_used -= block->size;
    
    // Merge with next block if possible
    if (block->next && !block->next->used) {
        block->size += sizeof(mem_block_t) + block->next->size;
        block->next = block->next->next;
    }
    
    // Merge with previous block if possible
    mem_block_t *current = first_block;
    while (current && current->next != block) {
        current = current->next;
    }
    
    if (current && !current->used) {
        current->size += sizeof(mem_block_t) + block->size;
        current->next = block->next;
    }
}

// Get memory usage statistics
void memory_stats(void) {
    u32 total_free = 0;
    u32 total_used = 0;
    u32 block_count = 0;
    
    mem_block_t *current = first_block;
    while (current) {
        if (current->used) {
            total_used += current->size;
        } else {
            total_free += current->size;
        }
        block_count++;
        current = current->next;
    }
    
    vga_printf("Memory Statistics:\n");
    vga_printf("  Total heap: %d KB\n", heap_size / 1024);
    vga_printf("  Used: %d KB\n", total_used / 1024);
    vga_printf("  Free: %d KB\n", total_free / 1024);
    vga_printf("  Blocks: %d\n", block_count);
}