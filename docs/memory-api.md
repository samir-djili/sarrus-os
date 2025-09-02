# Memory Management API Reference

## Current Implementation (Phase 1)

### Core Memory Functions

#### `void memory_init(void)`
- **Purpose**: Initialize the memory management system
- **Implementation**: Sets up 64KB heap at 5MB physical address
- **Current behavior**: Uses direct physical memory addressing
- **Called by**: `kernel_main()` during system initialization
- **Thread safety**: Single-threaded initialization only

#### `void *kmalloc(size_t size)`
- **Purpose**: Allocate memory from the kernel heap
- **Parameters**: `size` - Number of bytes to allocate (0 returns NULL)
- **Returns**: Pointer to allocated memory, or NULL if allocation fails
- **Alignment**: All allocations are 8-byte aligned
- **Overhead**: 32 bytes per allocation for metadata
- **Current limitations**: 64KB total heap size, no expansion

**Usage Example:**
```c
void *buffer = kmalloc(1024);
if (buffer) {
    memset(buffer, 0, 1024);
    // Use buffer...
    kfree(buffer);
} else {
    // Handle allocation failure
    terminal_writestring("Out of memory\n");
}
```

#### `void kfree(void *ptr)`
- **Purpose**: Free previously allocated memory
- **Parameters**: `ptr` - Pointer returned by kmalloc()
- **Safety**: Validates magic numbers, detects double-free
- **Behavior**: Automatically coalesces adjacent free blocks
- **Error handling**: Logs corruption errors but continues execution

**Safety Features:**
```c
// Automatic corruption detection
if (block->magic != HEAP_MAGIC_ALLOC) {
    terminal_writestring("ERROR: Invalid magic number in kfree\n");
    return;
}
```

### Memory Utilities (Current)

#### `void *memset(void *ptr, int value, size_t size)`
- **Purpose**: Set memory region to specific byte value
- **Implementation**: Optimized for 32-bit aligned access when possible
- **Performance**: ~4x faster than byte-by-byte for aligned data

#### `void *memcpy(void *dest, const void *src, size_t size)`
- **Purpose**: Copy memory region from source to destination
- **Safety**: No overlap checking (use memmove for overlapping regions)
- **Implementation**: Optimized for word-aligned transfers

#### `void *memmove(void *dest, const void *src, size_t size)`
- **Purpose**: Copy memory with overlap handling
- **Implementation**: Chooses copy direction based on pointer relationship

#### `int memcmp(const void *ptr1, const void *ptr2, size_t size)`
- **Purpose**: Compare two memory regions
- **Returns**: 0 if equal, <0 if ptr1 < ptr2, >0 if ptr1 > ptr2

### Debug and Statistics (Current)

#### `void get_memory_stats(memory_stats_t *stats)`
- **Purpose**: Retrieve current memory usage statistics
- **Information provided**:
  - Heap size and usage
  - Allocation/deallocation counts
  - Free memory available

#### `void heap_dump(void)`
- **Purpose**: Display detailed heap block information
- **Output**: Block-by-block allocation status and sizes
- **Use case**: Debugging memory issues and fragmentation

## Planned Advanced API (Phase 2 - Currently Disabled)

### Physical Memory Manager (PMM) - Ready but Disabled

#### `void pmm_init(uint32_t mem_size)`
- **Purpose**: Initialize the physical memory manager
- **Parameters**: `mem_size` - Total physical memory in bytes
- **Implementation**: Sets up free page stack and populates with available pages
- **Status**: ⏸️ Implemented but disabled for boot stability

#### `uint32_t pmm_alloc_page(void)`
- **Purpose**: Allocate a single 4KB physical page
- **Returns**: Physical address of allocated page, or 0 if out of memory
- **Performance**: O(1) stack-based allocation
- **Status**: ⏸️ Ready for Phase 2 activation

#### `void pmm_free_page(uint32_t page)`
- **Purpose**: Free a previously allocated physical page
- **Parameters**: `page` - Physical address of page to free
- **Performance**: O(1) operation
- **Status**: ⏸️ Ready for Phase 2 activation

### Virtual Memory Manager (VMM) - Ready but Disabled

#### `void vmm_init(void)`
- **Purpose**: Initialize virtual memory management and enable paging
- **Side effects**: Enables x86 paging, sets up initial kernel mappings
- **Requirements**: PMM must be initialized first
- **Status**: ⏸️ Implemented but disabled due to boot complexity

#### `void vmm_map_page(uint32_t virt_addr, uint32_t phys_addr, uint32_t flags)`
- **Purpose**: Map a virtual page to a physical page
- **Parameters**:
  - `virt_addr` - Virtual address (4KB aligned)
  - `phys_addr` - Physical address (4KB aligned)  
  - `flags` - Page permissions (PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER)
- **Status**: ⏸️ Implemented, ready for Phase 2

#### `void vmm_unmap_page(uint32_t virt_addr)`
- **Purpose**: Remove virtual to physical mapping
- **Parameters**: `virt_addr` - Virtual address to unmap
- **Side effects**: Frees underlying physical page, flushes TLB
- **Status**: ⏸️ Ready for Phase 2 activation

#### `uint32_t vmm_get_physical(uint32_t virt_addr)`
- **Purpose**: Translate virtual address to physical address
- **Returns**: Physical address, or 0 if not mapped
- **Use case**: Debugging, DMA setup, hardware interaction
- **Status**: ⏸️ Ready for Phase 2 activation

### Advanced Heap Features (Planned for Phase 2)

When Phase 2 is activated, the heap will gain these additional capabilities:

#### `void heap_init_virtual(void)`
- **Purpose**: Initialize kernel heap in virtual memory space
- **Requirements**: VMM must be initialized and paging enabled
- **Implementation**: Maps initial pages, creates expandable heap
- **Expansion**: Automatic page allocation when heap grows

#### Enhanced kmalloc (Phase 2)
- **Dynamic expansion**: Automatically allocates new pages when heap fills
- **Virtual memory**: Operates in kernel virtual address space
- **Better algorithms**: Best-fit allocation for reduced fragmentation

## Current Error Codes and Handling

### Return Values
```c
// Current function return patterns:
// kmalloc() - Returns NULL on failure
// kfree()   - Logs errors but continues execution
// memset()  - Always succeeds, returns dest pointer
// memcpy()  - Always succeeds, returns dest pointer
```

### Error Detection (Current)
```c
// Magic number validation
#define HEAP_MAGIC_ALLOC 0xDEADBEEF  // Allocated block
#define HEAP_MAGIC_FREE  0xFEEDFACE  // Free block

// Corruption detection in kfree:
if (block->magic != HEAP_MAGIC_ALLOC) {
    terminal_writestring("ERROR: Invalid magic number in kfree\n");
    return; // Safe failure - continue execution
}
```

### Memory Layout Constants (Current)
```c
#define PAGE_SIZE 4096
#define HEAP_START 0x500000           // 5MB mark - safe after kernel  
#define INITIAL_HEAP_SIZE (64 * 1024) // 64KB fixed size
#define HEAP_ALIGN 8                  // 8-byte alignment for all allocations
```

## API Usage Patterns (Current Implementation)

### Basic Memory Management
```c
// Initialize memory system (called once during boot)
memory_init();

// Typical allocation pattern
void *data = kmalloc(size);
if (data) {
    // Use allocated memory
    memset(data, 0, size);
    
    // Always free when done
    kfree(data);
} else {
    // Handle out-of-memory condition
    terminal_writestring("Allocation failed\n");
}
```

### Memory Utilities Usage
```c
// Fast memory operations
memset(buffer, 0, size);           // Zero memory
memcpy(dest, src, size);           // Copy memory
int result = memcmp(buf1, buf2, size); // Compare memory

// Debug and monitoring
memory_stats_t stats;
get_memory_stats(&stats);
heap_dump(); // Display heap state
```

## Implementation Status Summary

### ✅ Current Working Features (Phase 1)
- Basic heap allocation (kmalloc/kfree)
- Memory utilities (memset, memcpy, memcmp, memmove)
- Corruption detection with magic numbers
- Memory statistics and debugging
- 64KB heap at 5MB physical address
- 8-byte alignment for all allocations

### ⏸️ Ready but Disabled (Phase 2)  
- Physical Memory Manager (PMM)
- Virtual Memory Manager (VMM)
- x86 paging support
- Dynamic heap expansion
- Advanced allocation strategies

### 📋 Planned (Phase 3+)
- Memory pools for specific allocation patterns
- NUMA awareness
- Memory compression
- Guard pages
- Advanced debugging and profiling
- **Side effects**: Coalesces adjacent free blocks

**`void *kcalloc(size_t count, size_t size)`**
- **Purpose**: Allocate and zero-initialize memory
- **Parameters**: `count` - Number of elements, `size` - Size per element
- **Returns**: Pointer to zeroed memory
- **Implementation**: Calls kmalloc + memset

**`void *krealloc(void *ptr, size_t size)`**
- **Purpose**: Resize previously allocated memory
- **Parameters**: `ptr` - Original pointer, `size` - New size
## Data Structures (Current Implementation)

### heap_block_t
```c
typedef struct heap_block {
    uint32_t magic;          // 0xDEADBEEF (alloc) / 0xFEEDFACE (free)
    size_t size;             // Block size excluding header
    int is_free;             // 1 = free, 0 = allocated
    struct heap_block *next; // Next block in linked list
    struct heap_block *prev; // Previous block in linked list
    const char *file;        // Source file (debug builds) - currently unused
    uint32_t line;           // Source line (debug builds) - currently unused
} heap_block_t;
```

### memory_stats_t (Current)
```c
typedef struct {
    uint32_t heap_size;         // Current heap size (64KB)
    uint32_t heap_used;         // Allocated heap memory
    uint32_t heap_free;         // Free heap memory  
    uint32_t allocation_count;  // Total allocations made
    uint32_t free_count;        // Total frees made
    // Note: Physical/virtual stats will be added in Phase 2
} memory_stats_t;
```

## Constants and Macros (Current Implementation)

### Memory Layout (Current)
```c
#define PAGE_SIZE           4096
#define HEAP_START          0x500000    // 5MB physical address
#define INITIAL_HEAP_SIZE   (64 * 1024) // 64KB fixed size
#define HEAP_ALIGN          8           // 8-byte alignment
```

### Magic Numbers (Current)
```c
#define HEAP_MAGIC_ALLOC 0xDEADBEEF  // Allocated block signature
#define HEAP_MAGIC_FREE  0xFEEDFACE  // Free block signature
```

### Alignment Macros (Current)
```c
#define ALIGN_8(size) (((size) + 7) & ~7)  // 8-byte alignment
```

## Planned Constants (Phase 2)

### Virtual Memory Layout (When Enabled)
```c
#define KERNEL_VIRTUAL_BASE 0xC0000000  // 3GB
#define KERNEL_PHYSICAL_BASE 0x100000   // 1MB  
#define HEAP_VIRTUAL_START  0xD0000000  // Heap start
#define HEAP_VIRTUAL_END    0xDFFFFFFF  // Heap end (256MB max)
```

### Page Flags (When Paging Enabled)
```c
#define PAGE_PRESENT    0x001  // Page is present in memory
#define PAGE_WRITABLE   0x002  // Page is writable
#define PAGE_USER       0x004  // Page accessible from user mode
#define PAGE_ACCESSED   0x020  // Page has been accessed
#define PAGE_DIRTY      0x040  // Page has been written to
```

## Assembly Interface (Ready for Phase 2)

### Available Assembly Functions
The following assembly functions are implemented and ready for Phase 2:

```nasm
; Enable paging with given page directory
global enable_paging
enable_paging:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]  ; page_directory parameter
    mov cr3, eax        ; Load page directory
    mov eax, cr0
    or eax, 0x80000000  ; Set PG bit
    mov cr0, eax        ; Enable paging
    pop ebp
    ret

; Flush single page from TLB
global flush_tlb_single  
flush_tlb_single:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]  ; virtual_address parameter
    invlpg [eax]        ; Invalidate page
    pop ebp
    ret
```

## Common Usage Patterns (Current Implementation)

### Safe Allocation Pattern
```c
void *safe_alloc_example(size_t size) {
    void *ptr = kmalloc(size);
    if (!ptr) {
        terminal_writestring("Allocation failed\n");
        return NULL;
    }
    
    // Initialize allocated memory
    memset(ptr, 0, size);
    return ptr;
}

// Usage
void example_function(void) {
    void *buffer = safe_alloc_example(1024);
    if (buffer) {
        // Use buffer...
        kfree(buffer);
    }
}
```

### Memory Statistics Monitoring
```c
void print_memory_status(void) {
    memory_stats_t stats;
    get_memory_stats(&stats);
    
    terminal_writestring("Heap Status:\n");
    terminal_writestring("Size: ");
    terminal_write_dec(stats.heap_size);
    terminal_writestring(" bytes\n");
    
    terminal_writestring("Used: ");
    terminal_write_dec(stats.heap_used);
    terminal_writestring(" bytes\n");
    
    terminal_writestring("Free: ");
    terminal_write_dec(stats.heap_free);
    terminal_writestring(" bytes\n");
    
    terminal_writestring("Allocations: ");
    terminal_write_dec(stats.allocation_count);
    terminal_writestring("\n");
}
```

### Heap Debugging Pattern
```c
void debug_heap_state(void) {
    terminal_writestring("=== Heap Debug Info ===\n");
    heap_dump();  // Display all heap blocks
    print_memory_status();  // Show statistics
}
```

## Best Practices (Current Implementation)

### Allocation Guidelines
1. **Always check for NULL**: `kmalloc()` can fail when heap is full
2. **Initialize allocated memory**: Use `memset()` to clear new allocations
3. **Free all allocations**: Prevent memory leaks by calling `kfree()`
4. **Avoid double-free**: Don't call `kfree()` twice on same pointer

### Performance Tips
1. **Allocate appropriate sizes**: Avoid many tiny allocations (overhead)
2. **Free in reverse order**: Helps reduce fragmentation
3. **Monitor heap usage**: Use statistics to track memory consumption
4. **Batch operations**: Group related allocations together

### Debugging Recommendations
1. **Use heap_dump()**: Visualize heap state during development
2. **Monitor statistics**: Track allocation patterns
3. **Test with limited memory**: Verify behavior when heap is nearly full
4. **Validate magic numbers**: Corruption detection will catch many bugs

This API reference reflects the current stable implementation (Phase 1) while documenting the planned advanced features that will be enabled in future updates.
