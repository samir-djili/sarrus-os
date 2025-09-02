; paging.asm - Assembly functions for x86 paging support

section .text
global enable_paging
global flush_tlb_single
global get_cr2
global get_cr3

; Enable paging with the given page directory
; void enable_paging(uint32_t page_directory_physical)
enable_paging:
    push ebp
    mov ebp, esp
    
    ; Load page directory address into CR3
    mov eax, [ebp + 8]  ; page_directory parameter
    mov cr3, eax
    
    ; Enable paging by setting bit 31 in CR0
    mov eax, cr0
    or eax, 0x80000000  ; Set PG bit
    mov cr0, eax
    
    ; Flush TLB by reloading CR3
    mov eax, cr3
    mov cr3, eax
    
    pop ebp
    ret

; Flush a single page from TLB
; void flush_tlb_single(uint32_t virtual_address)
flush_tlb_single:
    push ebp
    mov ebp, esp
    
    mov eax, [ebp + 8]  ; virtual_address parameter
    invlpg [eax]        ; Invalidate page in TLB
    
    pop ebp
    ret

; Get CR2 register (page fault address)
; uint32_t get_cr2(void)
get_cr2:
    mov eax, cr2
    ret

; Get CR3 register (page directory address)
; uint32_t get_cr3(void)
get_cr3:
    mov eax, cr3
    ret
