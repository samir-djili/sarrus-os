#include "memory.h"
#include "kernel.h"

/* Global memory management state */
static uint32_t *page_directory = NULL;
static uint32_t next_page_frame = 0x400000; /* Start allocating from 4MB */
static uint32_t total_memory = 0;
static uint32_t used_pages = 0;

/* Heap management */
static heap_block_t *heap_first = NULL;
static uint32_t heap_start = HEAP_VIRTUAL_START;
static uint32_t heap_end = HEAP_VIRTUAL_START;
static memory_stats_t mem_stats = {0};

/* Page frame stack for free pages */
static uint32_t *free_page_stack = NULL;
static uint32_t free_page_count = 0;
static uint32_t max_free_pages = 0;

/* Memory regions list */
static memory_region_t *memory_regions = NULL;

/* Assembly functions for paging */
extern void enable_paging(uint32_t page_directory);
extern void flush_tlb_single(uint32_t addr);

/* Physical Memory Manager Implementation */
void pmm_init(uint32_t mem_size) {
    total_memory = mem_size;
    
    /* Initialize free page stack (simple implementation) */
    max_free_pages = (mem_size - 0x400000) / PAGE_SIZE;
    free_page_stack = (uint32_t *)(KERNEL_VIRTUAL_BASE + 0x300000); /* Use 3MB mark for stack */
    free_page_count = 0;
    
    /* Add all available pages to free stack */
    for (uint32_t addr = 0x400000; addr < mem_size; addr += PAGE_SIZE) {
        if (free_page_count < max_free_pages) {
            free_page_stack[free_page_count++] = addr;
        }
    }
    
    terminal_writestring("Physical memory manager initialized\n");
}

uint32_t pmm_alloc_page(void) {
    if (free_page_count == 0) {
        return 0; /* Out of memory */
    }
    
    uint32_t page = free_page_stack[--free_page_count];
    used_pages++;
    mem_stats.used_physical += PAGE_SIZE;
    mem_stats.free_physical -= PAGE_SIZE;
    
    return page;
}

void pmm_free_page(uint32_t page) {
    if (free_page_count >= max_free_pages) {
        return; /* Stack full */
    }
    
    free_page_stack[free_page_count++] = page;
    used_pages--;
    mem_stats.used_physical -= PAGE_SIZE;
    mem_stats.free_physical += PAGE_SIZE;
}

uint32_t pmm_get_free_pages(void) {
    return free_page_count;
}

/* Virtual Memory Manager Implementation */
void vmm_init(void) {
    /* Allocate page directory */
    uint32_t page_dir_phys = pmm_alloc_page();
    page_directory = (uint32_t *)(page_dir_phys + KERNEL_VIRTUAL_BASE);
    
    /* Clear page directory */
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0;
    }
    
    /* Identity map first 4MB for kernel */
    for (uint32_t addr = 0; addr < 0x400000; addr += PAGE_SIZE) {
        vmm_map_page(addr + KERNEL_VIRTUAL_BASE, addr, PAGE_PRESENT | PAGE_WRITABLE);
    }
    
    /* Enable paging */
    enable_paging(page_dir_phys);
    
    terminal_writestring("Virtual memory manager initialized\n");
}

void vmm_map_page(uint32_t virt_addr, uint32_t phys_addr, uint32_t flags) {
    uint32_t page_dir_index = virt_addr >> 22;
    uint32_t page_table_index = (virt_addr >> 12) & 0x3FF;
    
    /* Get or create page table */
    if (!(page_directory[page_dir_index] & PAGE_PRESENT)) {
        uint32_t page_table_phys = pmm_alloc_page();
        if (!page_table_phys) return;
        
        page_directory[page_dir_index] = page_table_phys | PAGE_PRESENT | PAGE_WRITABLE | (flags & PAGE_USER);
        
        /* Clear the page table */
        uint32_t *page_table = (uint32_t *)(page_table_phys + KERNEL_VIRTUAL_BASE);
        for (int i = 0; i < 1024; i++) {
            page_table[i] = 0;
        }
    }
    
    /* Map the page */
    uint32_t page_table_phys = page_directory[page_dir_index] & ~0xFFF;
    uint32_t *page_table = (uint32_t *)(page_table_phys + KERNEL_VIRTUAL_BASE);
    page_table[page_table_index] = phys_addr | flags;
    
    /* Flush TLB for this page */
    flush_tlb_single(virt_addr);
}

void vmm_unmap_page(uint32_t virt_addr) {
    uint32_t page_dir_index = virt_addr >> 22;
    uint32_t page_table_index = (virt_addr >> 12) & 0x3FF;
    
    if (!(page_directory[page_dir_index] & PAGE_PRESENT)) {
        return;
    }
    
    uint32_t page_table_phys = page_directory[page_dir_index] & ~0xFFF;
    uint32_t *page_table = (uint32_t *)(page_table_phys + KERNEL_VIRTUAL_BASE);
    
    if (page_table[page_table_index] & PAGE_PRESENT) {
        uint32_t physical = page_table[page_table_index] & ~0xFFF;
        pmm_free_page(physical);
        page_table[page_table_index] = 0;
        flush_tlb_single(virt_addr);
    }
}

uint32_t vmm_get_physical(uint32_t virt_addr) {
    uint32_t page_dir_index = virt_addr >> 22;
    uint32_t page_table_index = (virt_addr >> 12) & 0x3FF;
    uint32_t offset = virt_addr & 0xFFF;
    
    if (!(page_directory[page_dir_index] & PAGE_PRESENT)) {
        return 0;
    }
    
    uint32_t page_table_phys = page_directory[page_dir_index] & ~0xFFF;
    uint32_t *page_table = (uint32_t *)(page_table_phys + KERNEL_VIRTUAL_BASE);
    
    if (!(page_table[page_table_index] & PAGE_PRESENT)) {
        return 0;
    }
    
    return (page_table[page_table_index] & ~0xFFF) + offset;
}

int vmm_is_mapped(uint32_t virt_addr) {
    return vmm_get_physical(virt_addr) != 0;
}

/* Robust Heap Implementation */
void heap_init(void) {
    /* Reset heap to virtual address space */
    heap_start = HEAP_VIRTUAL_START;
    heap_end = heap_start;
    
    /* Map initial heap pages (64KB to start) */
    uint32_t initial_pages = 16;
    for (uint32_t i = 0; i < initial_pages; i++) {
        uint32_t physical = pmm_alloc_page();
        if (physical) {
            vmm_map_page(heap_start + (i * PAGE_SIZE), physical, PAGE_PRESENT | PAGE_WRITABLE);
            heap_end += PAGE_SIZE;
        }
    }
    
    /* Initialize first heap block */
    heap_first = (heap_block_t *)heap_start;
    heap_first->magic = HEAP_MAGIC_FREE;
    heap_first->size = (initial_pages * PAGE_SIZE) - sizeof(heap_block_t);
    heap_first->is_free = 1;
    heap_first->next = NULL;
    heap_first->prev = NULL;
    heap_first->file = NULL;
    heap_first->line = 0;
    
    mem_stats.heap_size = initial_pages * PAGE_SIZE;
    mem_stats.heap_free = heap_first->size;
    
    terminal_writestring("Kernel heap initialized with virtual memory\n");
}

static heap_block_t *find_free_block(size_t size) {
    heap_block_t *current = heap_first;
    
    while (current) {
        if (current->magic != HEAP_MAGIC_FREE && current->magic != HEAP_MAGIC_ALLOC) {
            terminal_writestring("HEAP CORRUPTION DETECTED!\n");
            return NULL;
        }
        
        if (current->is_free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

static void split_block(heap_block_t *block, size_t size) {
    if (block->size > size + sizeof(heap_block_t) + 32) { /* Minimum split size */
        heap_block_t *new_block = (heap_block_t *)((uint8_t *)block + sizeof(heap_block_t) + size);
        
        new_block->magic = HEAP_MAGIC_FREE;
        new_block->size = block->size - size - sizeof(heap_block_t);
        new_block->is_free = 1;
        new_block->next = block->next;
        new_block->prev = block;
        new_block->file = NULL;
        new_block->line = 0;
        
        if (block->next) {
            block->next->prev = new_block;
        }
        
        block->size = size;
        block->next = new_block;
    }
}

static void merge_free_blocks(heap_block_t *block) {
    /* Merge with next block */
    while (block->next && block->next->is_free) {
        heap_block_t *next = block->next;
        block->size += next->size + sizeof(heap_block_t);
        block->next = next->next;
        if (next->next) {
            next->next->prev = block;
        }
    }
    
    /* Merge with previous block */
    if (block->prev && block->prev->is_free) {
        heap_block_t *prev = block->prev;
        prev->size += block->size + sizeof(heap_block_t);
        prev->next = block->next;
        if (block->next) {
            block->next->prev = prev;
        }
    }
}

void *kmalloc(size_t size) {
    if (size == 0) return NULL;
    
    /* Align size to 8-byte boundary */
    size = (size + 7) & ~7;
    
    heap_block_t *block = find_free_block(size);
    if (!block) {
        /* In basic mode, we cannot expand the heap */
        terminal_writestring("Heap exhausted - no expansion in basic mode\n");
        return NULL;
    }
    
    block->magic = HEAP_MAGIC_ALLOC;
    block->is_free = 0;
    block->file = NULL;
    block->line = 0;
    
    split_block(block, size);
    
    mem_stats.allocation_count++;
    mem_stats.heap_used += size;
    mem_stats.heap_free -= size;
    
    return (uint8_t *)block + sizeof(heap_block_t);
}

void kfree(void *ptr) {
    if (!ptr) return;
    
    heap_block_t *block = (heap_block_t *)((uint8_t *)ptr - sizeof(heap_block_t));
    
    if (block->magic != HEAP_MAGIC_ALLOC) {
        terminal_writestring("DOUBLE FREE OR CORRUPTION DETECTED!\n");
        return;
    }
    
    block->magic = HEAP_MAGIC_FREE;
    block->is_free = 1;
    
    mem_stats.free_count++;
    mem_stats.heap_used -= block->size;
    mem_stats.heap_free += block->size;
    
    merge_free_blocks(block);
}

void *kcalloc(size_t count, size_t size) {
    size_t total_size = count * size;
    void *ptr = kmalloc(total_size);
    if (ptr) {
        memset(ptr, 0, total_size);
    }
    return ptr;
}

void *krealloc(void *ptr, size_t size) {
    if (!ptr) return kmalloc(size);
    if (size == 0) {
        kfree(ptr);
        return NULL;
    }
    
    heap_block_t *block = (heap_block_t *)((uint8_t *)ptr - sizeof(heap_block_t));
    if (block->magic != HEAP_MAGIC_ALLOC) {
        return NULL;
    }
    
    if (block->size >= size) {
        return ptr; /* Already big enough */
    }
    
    void *new_ptr = kmalloc(size);
    if (new_ptr) {
        memcpy(new_ptr, ptr, block->size);
        kfree(ptr);
    }
    
    return new_ptr;
}

/* Enhanced memory utilities */
void *memset(void *ptr, int value, size_t size) {
    uint8_t *bytes = (uint8_t *)ptr;
    uint8_t val = (uint8_t)value;
    
    /* Optimize for aligned access */
    while (size >= 4 && ((uintptr_t)bytes & 3) == 0) {
        uint32_t val32 = val | (val << 8) | (val << 16) | (val << 24);
        *(uint32_t *)bytes = val32;
        bytes += 4;
        size -= 4;
    }
    
    while (size--) {
        *bytes++ = val;
    }
    
    return ptr;
}

void *memcpy(void *dest, const void *src, size_t size) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    
    /* Optimize for aligned access */
    while (size >= 4 && ((uintptr_t)d & 3) == 0 && ((uintptr_t)s & 3) == 0) {
        *(uint32_t *)d = *(const uint32_t *)s;
        d += 4;
        s += 4;
        size -= 4;
    }
    
    while (size--) {
        *d++ = *s++;
    }
    
    return dest;
}

void *memmove(void *dest, const void *src, size_t size) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    
    if (d < s) {
        return memcpy(dest, src, size);
    } else {
        /* Copy backwards to handle overlap */
        d += size - 1;
        s += size - 1;
        while (size--) {
            *d-- = *s--;
        }
    }
    
    return dest;
}

int memcmp(const void *ptr1, const void *ptr2, size_t size) {
    const uint8_t *a = (const uint8_t *)ptr1;
    const uint8_t *b = (const uint8_t *)ptr2;
    
    for (size_t i = 0; i < size; i++) {
        if (a[i] != b[i]) {
            return a[i] - b[i];
        }
    }
    return 0;
}

/* Memory statistics and debugging */
void memory_get_stats(memory_stats_t *stats) {
    *stats = mem_stats;
    stats->total_physical = total_memory;
    stats->free_physical = free_page_count * PAGE_SIZE;
}

void memory_print_stats(void) {
    terminal_writestring("Memory Statistics:\n");
    terminal_writestring("  Physical: ");
    // TODO: Add number printing
    terminal_writestring(" total, ");
    terminal_writestring(" free\n");
    terminal_writestring("  Heap: ");
    terminal_writestring(" size, ");
    terminal_writestring(" used, ");
    terminal_writestring(" free\n");
    terminal_writestring("  Allocations: ");
    terminal_writestring(" allocs, ");
    terminal_writestring(" frees\n");
}

/* Safe memory system initialization with proper sequencing */
void memory_init(void) {
    terminal_writestring("Initializing memory management system...\n");
    
    /* Phase 1: Start with basic heap in physical memory */
    terminal_writestring("Setting up basic heap...\n");
    
    /* Use physical addresses initially (before paging) */
    heap_start = 0x500000; /* 5MB mark - safe area after kernel */
    heap_end = heap_start + (64 * 1024); /* 64KB initial heap */
    
    /* Initialize first heap block */
    heap_first = (heap_block_t *)heap_start;
    heap_first->magic = HEAP_MAGIC_FREE;
    heap_first->size = (64 * 1024) - sizeof(heap_block_t);
    heap_first->is_free = 1;
    heap_first->next = NULL;
    heap_first->prev = NULL;
    heap_first->file = NULL;
    heap_first->line = 0;
    
    /* Initialize basic statistics */
    mem_stats.heap_size = 64 * 1024;
    mem_stats.heap_free = heap_first->size;
    mem_stats.heap_used = 0;
    mem_stats.allocation_count = 0;
    mem_stats.free_count = 0;
    
    /* Set some reasonable defaults for other stats */
    mem_stats.total_physical = 32 * 1024 * 1024; /* Assume 32MB */
    mem_stats.used_physical = 5 * 1024 * 1024;   /* First 5MB used */
    mem_stats.free_physical = 27 * 1024 * 1024;  /* Rest available */
    mem_stats.total_virtual = 0; /* No virtual memory yet */
    
    terminal_writestring("Basic memory management initialized\n");
    terminal_writestring("Note: Advanced features (paging) will be enabled later\n");
}

/* Advanced memory system initialization - call this after basic system is stable */
void memory_init_advanced(void) {
    terminal_writestring("Initializing advanced memory management...\n");
    
    /* This function can be called later to enable paging and virtual memory */
    /* For now, we'll keep it simple and just report that it would be enabled */
    
    terminal_writestring("Advanced memory management available but not enabled\n");
    terminal_writestring("(Paging will be implemented in future versions)\n");
}

/* Basic memory test */
void memory_test(void) {
    terminal_writestring("Running memory tests...\n");
    
    /* Test allocation and freeing */
    void *ptr1 = kmalloc(1024);
    void *ptr2 = kmalloc(2048);
    void *ptr3 = kmalloc(512);
    
    if (ptr1 && ptr2 && ptr3) {
        terminal_writestring("Allocation test: PASSED\n");
        
        /* Test writing to allocated memory */
        memset(ptr1, 0xAA, 1024);
        memset(ptr2, 0xBB, 2048);
        memset(ptr3, 0xCC, 512);
        
        /* Test memory contents */
        uint8_t *test = (uint8_t *)ptr1;
        if (test[0] == 0xAA && test[1023] == 0xAA) {
            terminal_writestring("Memory write test: PASSED\n");
        } else {
            terminal_writestring("Memory write test: FAILED\n");
        }
        
        kfree(ptr1);
        kfree(ptr2);
        kfree(ptr3);
        terminal_writestring("Free test: COMPLETED\n");
    } else {
        terminal_writestring("Allocation test: FAILED\n");
    }
}
