# Sarrus OS Memory Management System

## Overview

The Sarrus OS memory management system is designed as a layered architecture that provides stable, reliable memory allocation for the operating system. The current implementation focuses on **robust basic functionality** with a clear path for future enhancements.

## Current Implementation Status

### ✅ **Phase 1: Basic Memory Management (CURRENT)**
- **Physical heap allocation**: Stable kmalloc/kfree implementation
- **Memory safety**: Corruption detection and validation
- **Memory utilities**: Optimized memset, memcpy, and related functions
- **Statistics tracking**: Comprehensive memory usage monitoring
- **Error handling**: Graceful failure detection and recovery

### 🚧 **Phase 2: Advanced Features (PLANNED)**
- **Virtual Memory Manager**: x86 paging with TLB management
- **Physical Memory Manager**: Page frame allocation system
- **Dynamic heap expansion**: Virtual memory-based growth
- **Memory protection**: Page-level access control

## Architecture Overview (Current)

```
┌─────────────────────────────────────────────────────────┐
│                  Application Layer                      │
├─────────────────────────────────────────────────────────┤
│                Basic Kernel Heap                       │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐      │
│  │  kmalloc()  │ │   kfree()   │ │ krealloc()  │      │
│  └─────────────┘ └─────────────┘ └─────────────┘      │
├─────────────────────────────────────────────────────────┤
│                Memory Utilities                        │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐      │
│  │   memset    │ │   memcpy    │ │  Statistics │      │
│  └─────────────┘ └─────────────┘ └─────────────┘      │
├─────────────────────────────────────────────────────────┤
│              Physical Memory Layout                    │
│                 (Direct Access)                        │
└─────────────────────────────────────────────────────────┘
```

## Current Memory Layout

### Physical Memory Layout (Current Implementation)

```
Physical Memory:
0x02000000 ┌─────────────────────────────────────┐
           │       Available RAM                 │
           │     (Future expansion)              │
0x00510000 ├─────────────────────────────────────┤ ← Heap End
           │       Kernel Heap                   │
           │        (64KB)                       │
0x00500000 ├─────────────────────────────────────┤ ← HEAP_START (5MB)
           │    Free Space / Future Use          │
0x00400000 ├─────────────────────────────────────┤
           │       Kernel Image                  │
0x00100000 ├─────────────────────────────────────┤ ← KERNEL_PHYSICAL_BASE
           │        BIOS/Boot                    │
0x00000000 └─────────────────────────────────────┘
```

## Core Components (Current Implementation)

### 1. Basic Heap Manager

The current implementation provides a robust heap allocation system using direct physical memory access.

#### Key Constants
```c
#define PAGE_SIZE 4096
#define HEAP_START 0x500000           /* 5MB mark - safe after kernel */
#define INITIAL_HEAP_SIZE (64 * 1024) /* 64KB initial heap */
```

#### Core Functions

**Initialization:**
```c
void memory_init(void) {
    /* Use physical addresses initially (before paging) */
    heap_start = 0x500000; /* 5MB mark - safe area after kernel */
    heap_end = heap_start + (64 * 1024); /* 64KB initial heap */
    
    /* Initialize first heap block */
    heap_first = (heap_block_t *)heap_start;
    heap_first->magic = HEAP_MAGIC_FREE;
    heap_first->size = (64 * 1024) - sizeof(heap_block_t);
    heap_first->is_free = 1;
    // ... rest of initialization
}
```

**Memory Allocation:**
```c
void *kmalloc(size_t size) {
    if (size == 0) return NULL;
    
    // Align size to 8-byte boundary
    size = (size + 7) & ~7;
    
    heap_block_t *block = find_free_block(size);
    if (!block) {
        // In current implementation, heap has fixed size
        terminal_writestring("Heap exhausted - no expansion in basic mode\n");
        return NULL;
    }
    
    block->magic = HEAP_MAGIC_ALLOC;
    block->is_free = 0;
    
    split_block(block, size);
    return (uint8_t *)block + sizeof(heap_block_t);
}
```

### 2. Memory Safety Features

#### Heap Block Structure
```c
typedef struct heap_block {
    uint32_t magic;          // 0xDEADBEEF (alloc) / 0xFEEDFACE (free)
    size_t size;             // Block size excluding header
    int is_free;             // 1 = free, 0 = allocated
    struct heap_block *next; // Next block in linked list
    struct heap_block *prev; // Previous block in linked list
    const char *file;        // Source file (debug builds)
    uint32_t line;           // Source line (debug builds)
} heap_block_t;
```

#### Corruption Detection
- **Magic numbers**: Each allocated block has `0xDEADBEEF`, free blocks have `0xFEEDFACE`
- **Boundary checking**: Validates block integrity during allocation/deallocation
- **Double-free detection**: Prevents freeing already freed blocks

### 3. Memory Utilities

**Optimized Memory Operations:**
```c
void *memset(void *ptr, int value, size_t size) {
    uint8_t *bytes = (uint8_t *)ptr;
    uint8_t val = (uint8_t)value;
    
    // Optimize for aligned access
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
```

## Current System Statistics and Debugging

### Memory Statistics (Current Implementation)
```c
typedef struct {
    uint32_t total_physical;    // Total physical memory
    uint32_t used_physical;     // Used physical memory  
    uint32_t free_physical;     // Free physical memory
    uint32_t heap_size;         // Current heap size (64KB)
    uint32_t heap_used;         // Used heap memory
    uint32_t heap_free;         // Free heap memory
    uint32_t allocation_count;  // Number of allocations
    uint32_t free_count;        // Number of frees
} memory_stats_t;
```

### Debug Functions (Current)
```c
void heap_dump(void) {
    heap_block_t *current = heap_first;
    uint32_t block_num = 0;
    
    terminal_writestring("=== HEAP DUMP ===\n");
    
    while (current) {
        terminal_writestring("Block ");
        terminal_write_dec(block_num++);
        terminal_writestring(": ");
        
        if (current->is_free) {
            terminal_writestring("FREE ");
        } else {
            terminal_writestring("ALLOC ");
        }
        
        terminal_write_hex(current->size);
        terminal_writestring(" bytes at ");
        terminal_write_hex((uint32_t)current);
        terminal_writestring("\n");
        
        current = current->next;
    }
}
```

### Current Memory Layout
```
Physical Memory Layout:
0x000000 - 0x0FFFFF: Reserved (BIOS, IVT, etc.)
0x100000 - 0x4FFFFF: Kernel code and data
0x500000 - 0x50FFFF: Kernel heap (64KB)
0x510000+          : Available for future expansion
```

## Planned Advanced Features (Phase 2)

The following features are implemented in the codebase but currently disabled for stability:

### 1. Physical Memory Manager (PMM)

Will manage physical page frames using a stack-based allocation strategy.

#### Implementation Ready
```c
void pmm_init(uint32_t mem_size) {
    total_memory = mem_size;
    max_free_pages = (mem_size - 0x400000) / PAGE_SIZE;
    free_page_stack = (uint32_t *)(KERNEL_VIRTUAL_BASE + 0x300000);
    
    // Populate free page stack
    for (uint32_t addr = 0x400000; addr < mem_size; addr += PAGE_SIZE) {
        if (free_page_count < max_free_pages) {
            free_page_stack[free_page_count++] = addr;
        }
    }
}
```

### 2. Virtual Memory Manager (VMM)

Will provide virtual-to-physical address translation using x86 paging.

#### Paging Infrastructure Ready
```c
void vmm_init(void) {
    // Allocate page directory
    uint32_t page_dir_phys = pmm_alloc_page();
    page_directory = (uint32_t *)(page_dir_phys + KERNEL_VIRTUAL_BASE);
    
    // Identity map first 4MB for kernel
    for (uint32_t addr = 0; addr < 0x400000; addr += PAGE_SIZE) {
        vmm_map_page(addr + KERNEL_VIRTUAL_BASE, addr, 
                     PAGE_PRESENT | PAGE_WRITABLE);
    }
    
    enable_paging(page_dir_phys);
}
```

### 3. Advanced Heap Features (Planned)

When VMM is enabled, the heap will support:
- **Dynamic expansion**: Automatic page allocation for heap growth
- **Virtual memory mapping**: Heap operates in kernel virtual space  
- **Better fragmentation handling**: Larger address space for allocation strategies

## Current Initialization Sequence

The memory system currently initializes with a simple, stable approach:

```c
void memory_init(void) {
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
    
    /* Initialize statistics */
    mem_stats.heap_size = 64 * 1024;
    mem_stats.heap_free = heap_first->size;
    mem_stats.heap_used = 0;
    mem_stats.allocation_count = 0;
    mem_stats.free_count = 0;
    
    terminal_writestring("Memory system initialized\n");
    terminal_writestring("Heap: 64KB at 5MB physical address\n");
}
```

## Planned Advanced Initialization (Phase 2)

When advanced features are enabled, initialization will follow this sequence:

```c
void memory_init_advanced(void) {
    // Phase 1: Detect available memory
    uint32_t memory_size = detect_memory(); // From multiboot info
    
    // Phase 2: Initialize Physical Memory Manager
    pmm_init(memory_size);
    
    // Phase 3: Set up Virtual Memory Manager and enable paging
    vmm_init(); // This enables paging
    
    // Phase 4: Initialize kernel heap in virtual memory
    heap_init_virtual();
    
    // Phase 5: Update statistics
    update_memory_stats();
}
```

## Performance Characteristics

### Time Complexity
- **Page allocation**: O(1) - stack-based allocation
- **Page deallocation**: O(1) - stack push operation
- **Heap allocation**: O(n) where n is number of heap blocks
- **Heap deallocation**: O(1) with coalescing

### Space Complexity
- **Page frame tracking**: O(n) where n is number of pages
- **Page tables**: O(n) where n is mapped virtual pages
- **Heap metadata**: ~32 bytes per allocation

## Error Handling

The memory system provides comprehensive error handling:

```c
// Return codes
#define MEM_SUCCESS     0
#define MEM_ERROR      -1
#define MEM_OUT_OF_MEM -2
#define MEM_INVALID    -3
#define MEM_CORRUPT    -4
```

Common error scenarios:
- **Out of physical memory**: PMM allocation fails
- **Out of virtual space**: Heap expansion beyond limits
- **Corruption detected**: Magic number validation fails
- **Invalid parameters**: NULL pointers, zero sizes

## Usage Examples

### Basic Allocation
```c
// Allocate 1KB buffer
void *buffer = kmalloc(1024);
if (buffer) {
    memset(buffer, 0, 1024);
    // Use buffer...
    kfree(buffer);
}
```

### Large Allocation
```c
// Allocate 64KB (multiple pages)
void *large_buffer = kmalloc(65536);
if (large_buffer) {
    // Automatically maps new pages as needed
    // Use buffer...
    kfree(large_buffer);
}
```

### Memory Statistics
```c
memory_stats_t stats;
memory_get_stats(&stats);
terminal_writestring("Heap usage: ");
terminal_write_dec(mem_stats.heap_used);
terminal_writestring("/");
terminal_write_dec(mem_stats.heap_size);
terminal_writestring(" bytes\n");
```

## Future Development Plan

### Phase 2: Advanced Memory Management (Next Update)
1. **PMM Integration**: Enable physical memory manager with page frame tracking
2. **VMM Activation**: Implement x86 paging with virtual memory support  
3. **Dynamic Heap**: Allow heap expansion using virtual memory mapping
4. **Advanced Allocation**: Implement best-fit and size-class allocators

### Phase 3: Extended Features (Long-term)
1. **NUMA awareness**: Multi-socket memory topology support
2. **Memory compression**: Compressed swap and memory pages
3. **Guard pages**: Page-level overflow protection  
4. **Memory pools**: Specialized allocators for common sizes
5. **Copy-on-write**: Efficient memory sharing for processes

## Current Implementation Status

### ✅ Completed (Phase 1)
- Basic heap allocation system with 64KB at 5MB physical address
- Memory safety with magic numbers and corruption detection
- Statistics tracking and debugging capabilities
- Optimized memory utilities (memset, memcpy, etc.)
- Stable boot process with reliable memory initialization

### 🔧 Ready but Disabled (Phase 2)
- Physical Memory Manager (PMM) implementation in memory.c
- Virtual Memory Manager (VMM) implementation in memory.c  
- x86 paging assembly functions in paging.asm
- Advanced heap expansion capabilities

### 📋 Planned (Phase 3+)
- Advanced allocation strategies
- Memory pools and specialized allocators
- NUMA support and memory compression
- Guard pages and enhanced security features

## Implementation Notes (Current System)

- Uses direct physical memory addressing (no virtual memory yet)
- Heap located at safe 5MB physical address after kernel
- Fixed 64KB heap size provides stable foundation
- All allocations are 8-byte aligned for optimal performance
- Magic numbers provide immediate corruption detection
- Simple but robust error handling with clear user feedback

This memory management system provides a solid, stable foundation for Sarrus OS. The current implementation prioritizes reliability and correctness, with advanced features ready to be enabled in future updates as the system matures.
