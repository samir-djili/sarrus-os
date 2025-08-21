# Sarrus OS API Reference

## Kernel API

### Terminal Functions

#### `void terminal_initialize(void)`
Initializes the VGA text mode terminal.
- Clears the screen
- Sets default colors
- Resets cursor position

#### `void terminal_setcolor(uint8_t color)`
Sets the terminal foreground and background color.
- **Parameters:**
  - `color`: VGA color byte (foreground | background << 4)

#### `void terminal_putchar(char c)`
Outputs a single character to the terminal.
- **Parameters:**
  - `c`: Character to output
- **Special characters:**
  - `\n`: Newline (moves to next line)

#### `void terminal_writestring(const char* data)`
Outputs a null-terminated string to the terminal.
- **Parameters:**
  - `data`: Null-terminated string to output

### VGA Colors

```c
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};
```

### Helper Functions

#### `static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)`
Creates a VGA color byte from foreground and background colors.
- **Parameters:**
  - `fg`: Foreground color
  - `bg`: Background color
- **Returns:** VGA color byte

#### `size_t strlen(const char* str)`
Calculates the length of a null-terminated string.
- **Parameters:**
  - `str`: Null-terminated string
- **Returns:** Length of string (excluding null terminator)

## Error Codes

```c
#define SUCCESS         0    // Operation completed successfully
#define ERROR_INVALID  -1    // Invalid parameter or state
#define ERROR_NOMEM    -2    // Out of memory
#define ERROR_IO       -3    // I/O error
#define ERROR_PERM     -4    // Permission denied
```

## Memory Layout

### Virtual Memory Map

```
0x00000000 - 0x3FFFFFFF: User Space (1GB)
0x40000000 - 0x7FFFFFFF: Shared Libraries (1GB)
0x80000000 - 0xBFFFFFFF: Kernel Heap (1GB)
0xC0000000 - 0xFFFFFFFF: Kernel Space (1GB)
```

### Physical Memory Map

```
0x00000000 - 0x000FFFFF: Real Mode Memory (1MB)
0x00100000 - 0x00FFFFFF: Kernel Space (15MB)
0x01000000 - 0xFFFFFFFF: User Space + Free Memory
```

## Boot Protocol

### Multiboot Header

The kernel uses the Multiboot specification for booting:

```c
#define MULTIBOOT_MAGIC         0x1BADB002
#define MULTIBOOT_FLAG_ALIGN    0x00000001
#define MULTIBOOT_FLAG_MEMINFO  0x00000002
```

### Boot Information

Information passed by the bootloader:

```c
typedef struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    // ... additional fields
} multiboot_info_t;
```

## Development Guidelines

### Function Naming

- Use lowercase with underscores: `function_name()`
- Prefix with subsystem: `memory_allocate()`, `process_create()`
- Use descriptive names: `calculate_page_count()` not `calc_pages()`

### Return Values

- Return 0 (SUCCESS) on success
- Return negative error codes on failure
- Use consistent error codes across subsystems

### Memory Management

```c
// Kernel memory allocation (future)
void *kmalloc(size_t size);
void kfree(void *ptr);
void *krealloc(void *ptr, size_t size);

// Page management (future)
void *alloc_page(void);
void free_page(void *page);
```

### Interrupt Handling

```c
// Interrupt service routine signature (future)
typedef void (*isr_t)(struct registers *regs);

// Register interrupt handler (future)
void register_interrupt_handler(uint8_t n, isr_t handler);
```

## System Calls (Future)

### Process Management

```c
int fork(void);                    // Create new process
int exec(const char *filename);    // Execute program
void exit(int status);             // Exit process
int wait(int *status);             // Wait for child process
```

### File Operations

```c
int open(const char *filename, int flags);
int close(int fd);
int read(int fd, void *buffer, size_t count);
int write(int fd, const void *buffer, size_t count);
```

### Memory Management

```c
void *malloc(size_t size);
void free(void *ptr);
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int munmap(void *addr, size_t length);
```

## Hardware Interfaces

### Port I/O

```c
// Read/write to I/O ports (future)
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t value);
uint16_t inw(uint16_t port);
void outw(uint16_t port, uint16_t value);
```

### Interrupt Control

```c
// Interrupt control (future)
void enable_interrupts(void);
void disable_interrupts(void);
int save_and_disable_interrupts(void);
void restore_interrupts(int state);
```

## Debugging

### Debug Output

```c
#ifdef DEBUG
#define debug_printf(fmt, ...) printf("[DEBUG] " fmt, ##__VA_ARGS__)
#else
#define debug_printf(fmt, ...)
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

## Build System

### Compilation

```bash
# Compile C source
i686-elf-gcc -std=gnu99 -ffreestanding -O2 -Wall -Wextra -c source.c -o object.o

# Assemble ASM source
nasm -f elf32 source.asm -o object.o

# Link kernel
i686-elf-ld -T linker.ld -nostdlib -o kernel.elf objects...
```

### ISO Creation

```bash
# Create bootable ISO
grub-mkrescue -o sarrus-os.iso iso_directory/
```

This API reference will be updated as new features are implemented.
