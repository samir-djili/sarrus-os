#ifndef SARRUS_MEMORY_H
#define SARRUS_MEMORY_H

#include <stddef.h>
#include <stdint.h>
/* Virtual Memory Manager (VMM) */
void vmm_init(void);
void vmm_map_page(uint32_t virt_addr, uint32_t phys_addr, uint32_t flags);
void vmm_unmap_page(uint32_t virt_addr);
uint32_t vmm_get_physical(uint32_t virt_addr);
int vmm_is_mapped(uint32_t virt_addr);

/* Memory management constants */
#define PAGE_SIZE 4096
#define PAGE_ALIGN_DOWN(addr) ((addr) & ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_UP(addr) (((addr) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

/* Physical memory constants */
#define KERNEL_VIRTUAL_BASE 0xC0000000  /* 3GB mark */
#define KERNEL_PHYSICAL_BASE 0x100000   /* 1MB mark */
#define HEAP_VIRTUAL_START 0xD0000000   /* Kernel heap start */
#define HEAP_VIRTUAL_END   0xDFFFFFFF   /* Kernel heap end (256MB) */

/* Page directory and table entries */
#define PAGE_PRESENT    0x001
#define PAGE_WRITABLE   0x002
#define PAGE_USER       0x004
#define PAGE_ACCESSED   0x020
#define PAGE_DIRTY      0x040

/* Memory regions */
typedef struct memory_region {
    uint32_t start;
    uint32_t length;
    uint32_t type;
    struct memory_region *next;
} memory_region_t;

/* Page frame allocator */
typedef struct page_frame {
    struct page_frame *next;
    uint32_t ref_count;
    uint32_t flags;
} page_frame_t;

/* Virtual memory mapping */
typedef struct vm_area {
    uint32_t start;
    uint32_t end;
    uint32_t flags;
    struct vm_area *next;
} vm_area_t;

/* Advanced heap block with debugging */
typedef struct heap_block {
    uint32_t magic;        /* Magic number for corruption detection */
    size_t size;           /* Size of the block */
    int is_free;           /* Free flag */
    struct heap_block *next;
    struct heap_block *prev;
    const char *file;      /* File where allocated (debug) */
    int line;              /* Line where allocated (debug) */
} heap_block_t;

/* Memory statistics */
typedef struct memory_stats {
    size_t total_physical;
    size_t used_physical;
    size_t free_physical;
    size_t total_virtual;
    size_t used_virtual;
    size_t heap_size;
    size_t heap_used;
    size_t heap_free;
    uint32_t allocation_count;
    uint32_t free_count;
} memory_stats_t;

/* Physical memory management */
void pmm_init(uint32_t mem_size);
uint32_t pmm_alloc_page(void);
void pmm_free_page(uint32_t page);
uint32_t pmm_get_free_pages(void);

/* Virtual memory management */
void vmm_init(void);
void vmm_map_page(uint32_t virtual, uint32_t physical, uint32_t flags);
void vmm_unmap_page(uint32_t virtual);
uint32_t vmm_get_physical(uint32_t virtual);
int vmm_is_mapped(uint32_t virtual);

/* Page directory/table management */
void paging_init(void);
void switch_page_directory(uint32_t *page_dir);
void flush_tlb(void);

/* Kernel heap management */
void heap_init(void);
void *kmalloc(size_t size);
void *kmalloc_a(size_t size);  /* Page-aligned allocation */
void *kmalloc_ap(size_t size, uint32_t *physical); /* Get physical address */
void *kcalloc(size_t count, size_t size);
void *krealloc(void *ptr, size_t size);
void kfree(void *ptr);

/* Debug allocations */
#ifdef DEBUG_MEMORY
#define kmalloc_debug(size) kmalloc_debug_impl(size, __FILE__, __LINE__)
#define kfree_debug(ptr) kfree_debug_impl(ptr, __FILE__, __LINE__)
void *kmalloc_debug_impl(size_t size, const char *file, int line);
void kfree_debug_impl(void *ptr, const char *file, int line);
void heap_dump(void);
void heap_check_integrity(void);
#endif

/* Memory utilities */
void *memset(void *ptr, int value, size_t size);
void *memcpy(void *dest, const void *src, size_t size);
void *memmove(void *dest, const void *src, size_t size);
int memcmp(const void *ptr1, const void *ptr2, size_t size);

/* Memory region management */
void memory_region_add(uint32_t start, uint32_t length, uint32_t type);
memory_region_t *memory_region_find(uint32_t addr);
void memory_region_print(void);

/* Memory statistics and debugging */
void memory_init(void);
void memory_init_advanced(void);
void memory_get_stats(memory_stats_t *stats);
void memory_print_stats(void);
void memory_test(void);

/* Memory protection */
void memory_protect_kernel(void);
int memory_check_access(uint32_t addr, size_t size, int write);

/* Constants for memory types */
#define MEMORY_TYPE_AVAILABLE   1
#define MEMORY_TYPE_RESERVED    2
#define MEMORY_TYPE_ACPI        3
#define MEMORY_TYPE_KERNEL      4

/* Magic numbers for heap corruption detection */
#define HEAP_MAGIC_ALLOC   0xDEADBEEF
#define HEAP_MAGIC_FREE    0xFEEDFACE

#endif /* SARRUS_MEMORY_H */
