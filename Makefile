# Kernel Makefile - Build system for comprehensive kernel
# Targets: x86 32-bit architecture

CC = gcc
AS = nasm
LD = ld

# Compiler flags
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
         -nostartfiles -nodefaultlibs -Wall -Wextra -c -ffreestanding
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld

# Directories
SRCDIR = src
INCDIR = include
BUILDDIR = build
BOOTDIR = boot

# Source files
BOOT_ASM = $(BOOTDIR)/boot.asm
KERNEL_C = $(SRCDIR)/kernel.c \
           $(SRCDIR)/vga.c \
           $(SRCDIR)/gdt.c \
           $(SRCDIR)/idt.c \
           $(SRCDIR)/memory.c \
           $(SRCDIR)/process.c \
           $(SRCDIR)/syscall.c \
           $(SRCDIR)/filesystem.c \
           $(SRCDIR)/keyboard.c \
           $(SRCDIR)/timer.c \
           $(SRCDIR)/shell.c \
           $(SRCDIR)/network.c

KERNEL_ASM = $(SRCDIR)/idt_asm.asm \
             $(SRCDIR)/gdt_asm.asm \
             $(SRCDIR)/syscall_asm.asm

# Object files
BOOT_OBJ = $(BUILDDIR)/boot.o
KERNEL_C_OBJ = $(KERNEL_C:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
KERNEL_ASM_OBJ = $(KERNEL_ASM:$(SRCDIR)/%.asm=$(BUILDDIR)/%.o)

# Output
KERNEL_BIN = $(BUILDDIR)/kernel.bin
ISO_FILE = $(BUILDDIR)/kernel.iso

.PHONY: all clean run

all: $(ISO_FILE)

# Create build directory
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Compile C source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

# Assemble ASM files
$(BUILDDIR)/%.o: $(SRCDIR)/%.asm | $(BUILDDIR)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILDDIR)/%.o: $(BOOTDIR)/%.asm | $(BUILDDIR)
	$(AS) $(ASFLAGS) $< -o $@

# Link kernel
$(KERNEL_BIN): $(BOOT_OBJ) $(KERNEL_C_OBJ) $(KERNEL_ASM_OBJ) linker.ld
	$(LD) $(LDFLAGS) $(BOOT_OBJ) $(KERNEL_C_OBJ) $(KERNEL_ASM_OBJ) -o $@

# Create ISO for QEMU
$(ISO_FILE): $(KERNEL_BIN)
	mkdir -p $(BUILDDIR)/iso/boot/grub
	cp $(KERNEL_BIN) $(BUILDDIR)/iso/boot/
	echo 'set timeout=0' > $(BUILDDIR)/iso/boot/grub/grub.cfg
	echo 'set default=0' >> $(BUILDDIR)/iso/boot/grub/grub.cfg
	echo 'menuentry "MyKernel" {' >> $(BUILDDIR)/iso/boot/grub/grub.cfg
	echo '  multiboot /boot/kernel.bin' >> $(BUILDDIR)/iso/boot/grub/grub.cfg
	echo '  boot' >> $(BUILDDIR)/iso/boot/grub/grub.cfg
	echo '}' >> $(BUILDDIR)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO_FILE) $(BUILDDIR)/iso 2>/dev/null || echo "GRUB not available - kernel.bin ready for testing"

# Run in QEMU
run: $(KERNEL_BIN)
	qemu-system-i386 -kernel $(KERNEL_BIN) || echo "QEMU not available - use $(KERNEL_BIN) with your emulator"

# Clean build files
clean:
	rm -rf $(BUILDDIR)

# Help target
help:
	@echo "Available targets:"
	@echo "  all     - Build kernel ISO"
	@echo "  run     - Run kernel in QEMU"
	@echo "  clean   - Clean build files"
	@echo "  help    - Show this help"