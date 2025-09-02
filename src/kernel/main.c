#include <stddef.h>
#include <stdint.h>
#include "kernel.h"
#include "memory.h"

uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT)
            terminal_row = 0;
        return;
    }
    
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT)
            terminal_row = 0;
    }
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
    terminal_write(data, strlen(data));
}

void kernel_main(void) {
    /* Initialize terminal interface */
    terminal_initialize();

    /* Print welcome message */
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal_writestring("Welcome to Sarrus OS!\n");
    terminal_writestring("====================\n\n");
    
    terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    terminal_writestring("Version: 0.1.0 (Development)\n");
    terminal_writestring("Architecture: x86 (32-bit)\n");
    terminal_writestring("Build: DEBUG\n\n");
    
    /* Initialize memory management system */
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    terminal_writestring("Initializing Memory Management...\n");
    memory_init();
    
    /* Test the memory system */
    terminal_setcolor(vga_entry_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK));
    memory_test();
    
    /* Print memory statistics */
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK));
    memory_print_stats();
    
    /* System ready message */
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal_writestring("\nSystem initialized successfully!\n");
    terminal_writestring("Sarrus OS is running with basic memory management.\n");
    
    terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    terminal_writestring("\nCurrent features:\n");
    terminal_writestring("- VGA text mode display\n");
    terminal_writestring("- Basic heap allocation (kmalloc/kfree)\n");
    terminal_writestring("- Memory corruption detection\n");
    terminal_writestring("- Memory usage statistics\n\n");
    
    terminal_writestring("Next steps:\n");
    terminal_writestring("- Enable paging and virtual memory\n");
    terminal_writestring("- Interrupt handling (IDT)\n");
    terminal_writestring("- Process management\n");
    terminal_writestring("- File system\n");
    terminal_writestring("- DOOM compatibility layer\n\n");
    
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    terminal_writestring("System running. Memory management operational.\n");
    
    /* Kernel main loop - for now, just halt */
    while (1) {
        asm volatile ("hlt");
    }
}
