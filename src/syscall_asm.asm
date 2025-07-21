; System call assembly handler

[BITS 32]

extern syscall_dispatcher

global syscall_handler

syscall_handler:
    ; Save all registers
    pushad
    
    ; Save data segments
    push ds
    push es
    push fs
    push gs
    
    ; Load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Push system call arguments (from user registers)
    push edx    ; arg3
    push ecx    ; arg2
    push ebx    ; arg1
    push eax    ; syscall number
    
    ; Call C dispatcher
    call syscall_dispatcher
    
    ; Clean up arguments
    add esp, 16
    
    ; Restore data segments
    pop gs
    pop fs
    pop es
    pop ds
    
    ; Restore all registers (except eax which contains return value)
    mov [esp + 28], eax  ; Store return value in saved eax position
    popad
    
    ; Return to user space
    iret