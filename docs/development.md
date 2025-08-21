# Development Guide

## Coding Standards

### C Code Style

- Use C99 standard
- 4-space indentation (no tabs)
- Opening braces on same line for functions, separate line for control structures
- Maximum line length: 100 characters
- Use descriptive variable names

```c
// Good
int calculate_memory_size(uint32_t pages) {
    if (pages == 0) {
        return -1;
    }
    return pages * PAGE_SIZE;
}

// Bad
int calc(uint32_t p){return p*4096;}
```

### Assembly Code Style

- Use Intel syntax (NASM)
- Clear comments for each section
- Consistent indentation
- Label naming: lowercase with underscores

```asm
; Good
section .text
global start
start:
    mov esp, stack_top    ; Set up stack pointer
    call kernel_main      ; Jump to C code
    cli                   ; Disable interrupts
    hlt                   ; Halt processor
```

### File Organization

```
src/
├── boot/               # Bootloader code
├── kernel/             # Core kernel
│   ├── main.c         # Kernel entry point
│   ├── memory.c       # Memory management
│   ├── process.c      # Process management
│   └── syscall.c      # System call handling
├── drivers/            # Device drivers
│   ├── keyboard.c
│   ├── vga.c
│   └── timer.c
├── fs/                 # File system
├── mm/                 # Memory management
└── libc/              # Standard library
```

## Header Files

### Include Guards

Always use include guards in header files:

```c
#ifndef SARRUS_MEMORY_H
#define SARRUS_MEMORY_H

// Header content here

#endif /* SARRUS_MEMORY_H */
```

### Standard Headers

Create consistent header structure:

```c
#ifndef SARRUS_EXAMPLE_H
#define SARRUS_EXAMPLE_H

#include <stdint.h>
#include <stddef.h>

// Constants
#define EXAMPLE_CONSTANT 42

// Type definitions
typedef struct {
    uint32_t field1;
    uint32_t field2;
} example_t;

// Function declarations
int example_function(example_t *data);
void example_init(void);

#endif /* SARRUS_EXAMPLE_H */
```

## Error Handling

### Return Codes

Use consistent error codes:

```c
#define SUCCESS         0
#define ERROR_INVALID  -1
#define ERROR_NOMEM    -2
#define ERROR_IO       -3
#define ERROR_PERM     -4
```

### Error Handling Pattern

```c
int function_that_can_fail(void) {
    if (some_condition) {
        return ERROR_INVALID;
    }
    
    int result = other_function();
    if (result != SUCCESS) {
        return result;  // Propagate error
    }
    
    return SUCCESS;
}
```

## Memory Management

### Kernel Memory Allocation

```c
// Use kernel allocator
void *ptr = kmalloc(size);
if (!ptr) {
    return ERROR_NOMEM;
}

// Always free allocated memory
kfree(ptr);
```

### Stack vs Heap

- Use stack for small, temporary data
- Use heap for large or persistent data
- Be mindful of stack size limits

## Debugging Techniques

### Debug Output

```c
#ifdef DEBUG
#define debug_print(fmt, ...) printf("[DEBUG] " fmt, ##__VA_ARGS__)
#else
#define debug_print(fmt, ...)
#endif
```

### Assertions

```c
#define ASSERT(condition) \
    do { \
        if (!(condition)) { \
            panic("Assertion failed: %s at %s:%d", \
                  #condition, __FILE__, __LINE__); \
        } \
    } while(0)
```

### Serial Debugging

Use serial port for debug output when VGA is not available:

```c
void serial_write_string(const char *str) {
    while (*str) {
        outb(0x3F8, *str++);  // Write to COM1
    }
}
```

## Testing Strategy

### Unit Testing

Create simple test framework:

```c
typedef struct {
    const char *name;
    int (*test_func)(void);
} test_case_t;

int run_tests(test_case_t *tests, int count) {
    int passed = 0;
    for (int i = 0; i < count; i++) {
        printf("Running %s... ", tests[i].name);
        if (tests[i].test_func() == SUCCESS) {
            printf("PASS\n");
            passed++;
        } else {
            printf("FAIL\n");
        }
    }
    return passed;
}
```

### Integration Testing

Test complete subsystems:

```c
int test_memory_subsystem(void) {
    // Initialize memory manager
    memory_init();
    
    // Test allocation
    void *ptr = kmalloc(1024);
    if (!ptr) return ERROR_NOMEM;
    
    // Test deallocation
    kfree(ptr);
    
    return SUCCESS;
}
```

## Performance Guidelines

### Critical Sections

Keep critical sections short:

```c
// Good
disable_interrupts();
update_critical_data();
enable_interrupts();

// Bad
disable_interrupts();
do_lots_of_work();  // Long operation with interrupts disabled
enable_interrupts();
```

### Memory Access Patterns

- Prefer sequential memory access
- Minimize pointer dereferencing
- Use cache-friendly data structures

### Function Calls

- Inline small, frequently called functions
- Avoid deep call stacks in interrupt handlers
- Use function pointers for flexibility, direct calls for performance

## Documentation

### Function Documentation

```c
/**
 * Allocates memory from the kernel heap
 * 
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory, or NULL on failure
 * 
 * The allocated memory is not initialized and must be freed
 * with kfree() when no longer needed.
 */
void *kmalloc(size_t size);
```

### Code Comments

```c
// Initialize the page directory
page_directory_t *pd = create_page_directory();

/* 
 * Map the first 4MB of physical memory to virtual address 0xC0000000
 * This is where the kernel will be loaded in virtual memory
 */
for (int i = 0; i < 1024; i++) {
    map_page(pd, 0xC0000000 + i * 0x1000, i * 0x1000, PAGE_PRESENT | PAGE_RW);
}
```

## Build System

### Adding New Files

1. Add source files to appropriate directory
2. Update Makefile if needed
3. Add header files to include/
4. Update documentation

### Compilation Flags

Use appropriate compiler flags:

```makefile
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Werror
CFLAGS += -fno-stack-protector -fno-builtin
CFLAGS += -nostdlib -nostartfiles -nodefaultlibs
```

## Git Workflow

### Commit Messages

Use clear, descriptive commit messages:

```
type(scope): description

Examples:
feat(mm): add page frame allocator
fix(boot): correct stack alignment issue
docs(readme): update build instructions
refactor(kernel): simplify interrupt handling
```

### Branch Strategy

- `main`: Stable, working code
- `develop`: Integration branch
- `feature/name`: Feature development
- `bugfix/name`: Bug fixes

## Common Pitfalls

### Memory Issues

- Always check allocation success
- Initialize allocated memory
- Avoid buffer overflows
- Don't access freed memory

### Interrupt Handling

- Keep interrupt handlers short
- Don't call blocking functions in interrupt context
- Save and restore registers properly

### Stack Overflow

- Monitor stack usage
- Avoid deep recursion
- Use heap for large data structures

## Tools and Utilities

### Recommended Tools

- **Editor**: VS Code with C/C++ extension
- **Debugger**: GDB with QEMU
- **Version Control**: Git
- **Documentation**: Markdown

### QEMU Debugging

```bash
# Start QEMU with GDB server
qemu-system-i386 -s -S -cdrom sarrus-os.iso

# In another terminal
gdb build/kernel.elf
(gdb) target remote localhost:1234
(gdb) break kernel_main
(gdb) continue
```

## Resources

- [OSDev.org Wiki](https://wiki.osdev.org/)
- [Intel x86 Documentation](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
- [QEMU Documentation](https://qemu.readthedocs.io/)
- [GDB Manual](https://sourceware.org/gdb/documentation/)
- [NASM Documentation](https://nasm.us/docs.php)
