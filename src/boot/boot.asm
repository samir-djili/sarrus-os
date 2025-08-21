; Sarrus OS Bootloader
; Basic multiboot-compliant bootloader

MBALIGN  equ  1<<0              ; align loaded modules on page boundaries
MEMINFO  equ  1<<1              ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot

; Declare multiboot header
section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

; Reserve stack space
section .bss
align 16
stack_bottom:
resb 16384 ; 16 KiB
stack_top:

; Kernel entry point
section .text
global start:function (start.end - start)
start:
    ; Set up stack
    mov esp, stack_top

    ; Call kernel main function
    extern kernel_main
    call kernel_main

    ; Disable interrupts and halt
    cli
.hang:
    hlt
    jmp .hang
.end:
