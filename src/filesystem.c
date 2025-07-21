#include "kernel.h"
#include "vga.h"

// Simple in-memory file system
#define MAX_FILES 64
#define MAX_FILENAME 32
#define MAX_FILE_SIZE 4096

typedef struct file {
    char name[MAX_FILENAME];
    u8 *data;
    u32 size;
    u8 used;
} file_t;

static file_t files[MAX_FILES];
static u32 file_count = 0;

// Initialize file system
void filesystem_init(void) {
    for (int i = 0; i < MAX_FILES; i++) {
        files[i].used = 0;
        files[i].data = NULL;
        files[i].size = 0;
        memset(files[i].name, 0, MAX_FILENAME);
    }
    
    // Create some default files
    fs_create_file("readme.txt", "Welcome to the comprehensive kernel!\nThis is a simple in-memory file system.\n", 77);
    fs_create_file("version.txt", "Kernel Version 1.0\nBuilt with love and assembly!\n", 50);
    fs_create_file("help.txt", "Available commands:\nls - list files\ncat <file> - show file contents\nps - list processes\nmeminfo - memory stats\n", 113);
    
    vga_printf("File system initialized with %d files\n", file_count);
}

// Create a new file
int fs_create_file(const char *name, const char *content, u32 size) {
    if (file_count >= MAX_FILES) return -1;
    if (size > MAX_FILE_SIZE) return -2;
    
    // Check if file already exists
    for (u32 i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, name) == 0) {
            return -3; // File exists
        }
    }
    
    // Find free slot
    for (u32 i = 0; i < MAX_FILES; i++) {
        if (!files[i].used) {
            files[i].used = 1;
            strcpy(files[i].name, name);
            files[i].size = size;
            files[i].data = (u8 *)kmalloc(size + 1);
            
            if (files[i].data) {
                memcpy(files[i].data, content, size);
                files[i].data[size] = 0; // Null terminate
                file_count++;
                return 0;
            } else {
                files[i].used = 0;
                return -4; // Out of memory
            }
        }
    }
    
    return -5; // No free slots
}

// Read a file
int fs_read_file(const char *name, char *buffer, u32 buffer_size) {
    for (u32 i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, name) == 0) {
            u32 copy_size = files[i].size < buffer_size ? files[i].size : buffer_size - 1;
            memcpy(buffer, files[i].data, copy_size);
            buffer[copy_size] = 0;
            return copy_size;
        }
    }
    return -1; // File not found
}

// Delete a file
int fs_delete_file(const char *name) {
    for (u32 i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, name) == 0) {
            kfree(files[i].data);
            files[i].used = 0;
            files[i].data = NULL;
            files[i].size = 0;
            memset(files[i].name, 0, MAX_FILENAME);
            file_count--;
            return 0;
        }
    }
    return -1; // File not found
}

// List all files
void fs_list_files(void) {
    vga_printf("Files in system:\n");
    vga_printf("Name\t\t\tSize (bytes)\n");
    vga_printf("----\t\t\t------------\n");
    
    for (u32 i = 0; i < MAX_FILES; i++) {
        if (files[i].used) {
            vga_printf("%-20s\t%d\n", files[i].name, files[i].size);
        }
    }
    vga_printf("Total: %d files\n", file_count);
}

// Get file info
file_t *fs_get_file_info(const char *name) {
    for (u32 i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, name) == 0) {
            return &files[i];
        }
    }
    return NULL;
}

// Write to file (overwrite)
int fs_write_file(const char *name, const char *content, u32 size) {
    if (size > MAX_FILE_SIZE) return -2;
    
    for (u32 i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, name) == 0) {
            // File exists, update it
            kfree(files[i].data);
            files[i].data = (u8 *)kmalloc(size + 1);
            if (files[i].data) {
                memcpy(files[i].data, content, size);
                files[i].data[size] = 0;
                files[i].size = size;
                return 0;
            } else {
                files[i].used = 0;
                file_count--;
                return -4; // Out of memory
            }
        }
    }
    
    // File doesn't exist, create it
    return fs_create_file(name, content, size);
}

// Get file system statistics
void fs_stats(void) {
    u32 total_size = 0;
    for (u32 i = 0; i < MAX_FILES; i++) {
        if (files[i].used) {
            total_size += files[i].size;
        }
    }
    
    vga_printf("File System Statistics:\n");
    vga_printf("  Files: %d / %d\n", file_count, MAX_FILES);
    vga_printf("  Total size: %d bytes\n", total_size);
    vga_printf("  Max file size: %d bytes\n", MAX_FILE_SIZE);
}