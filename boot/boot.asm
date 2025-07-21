; Multiboot boot loader for comprehensive kernel
; This loader complies with Multiboot specification

MBALIGN  equ  1 << 0              ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1              ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO   ; multiboot 'flag' field
MAGIC    equ  0x1BADB002          ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)     ; checksum of above, to prove we are multiboot

; Declare a multiboot header
section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

; Declare a stack for the initial thread
section .bss
align 16
stack_bottom:
resb 16384  ; 16 KiB stack
stack_top:

; Kernel entry point
section .text
global start:function (start.end - start)
start:
    ; Setup stack pointer
    mov esp, stack_top

    ; Save multiboot info pointer and magic number
    push ebx    ; Multiboot info structure
    push eax    ; Multiboot magic number

    ; Call kernel main function
    extern kernel_main
    call kernel_main

    ; If kernel returns, halt the CPU
    cli
.hang:
    hlt
    jmp .hang
.end: