# Sarrus OS Makefile

# Compiler and tools
CC = i686-elf-gcc
AS = nasm
LD = i686-elf-ld
OBJCOPY = i686-elf-objcopy

# Directories
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include
BOOT_DIR = $(SRC_DIR)/boot
KERNEL_DIR = $(SRC_DIR)/kernel

# Flags
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I$(INCLUDE_DIR)
ASFLAGS = -f elf32
LDFLAGS = -T linker.ld -nostdlib

# Source files
BOOT_ASM = $(BOOT_DIR)/boot.asm
KERNEL_C = $(wildcard $(KERNEL_DIR)/*.c)
KERNEL_ASM = $(wildcard $(KERNEL_DIR)/*.asm)

# Object files
BOOT_OBJ = $(BUILD_DIR)/boot.o
KERNEL_C_OBJ = $(KERNEL_C:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
KERNEL_ASM_OBJ = $(KERNEL_ASM:$(SRC_DIR)/%.asm=$(BUILD_DIR)/%.o)
KERNEL_OBJ = $(KERNEL_C_OBJ) $(KERNEL_ASM_OBJ)

# Output
KERNEL_ELF = $(BUILD_DIR)/kernel.elf
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
ISO_DIR = $(BUILD_DIR)/iso
ISO_FILE = $(BUILD_DIR)/sarrus-os.iso

# Default target
all: $(ISO_FILE)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/kernel
	mkdir -p $(BUILD_DIR)/boot
	mkdir -p $(ISO_DIR)
	mkdir -p $(ISO_DIR)/boot
	mkdir -p $(ISO_DIR)/boot/grub

# Boot object
$(BOOT_OBJ): $(BOOT_ASM) | $(BUILD_DIR)
	$(AS) $(ASFLAGS) $< -o $@

# Kernel C objects
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Kernel ASM objects
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm | $(BUILD_DIR)
	$(AS) $(ASFLAGS) $< -o $@

# Kernel ELF
$(KERNEL_ELF): $(BOOT_OBJ) $(KERNEL_OBJ) | $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $^ `$(CC) -print-libgcc-file-name`

# Kernel binary
$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

# ISO image
$(ISO_FILE): $(KERNEL_ELF) | $(BUILD_DIR)
	cp $(KERNEL_ELF) $(ISO_DIR)/boot/
	cp grub.cfg $(ISO_DIR)/boot/grub/
	grub-mkrescue -o $@ $(ISO_DIR)

# Run in QEMU
run: $(ISO_FILE)
	qemu-system-i386 -cdrom $(ISO_FILE) -m 512M

# Run with debugging
debug: $(ISO_FILE)
	qemu-system-i386 -cdrom $(ISO_FILE) -m 512M -s -S

# Run with monitor
monitor: $(ISO_FILE)
	qemu-system-i386 -cdrom $(ISO_FILE) -m 512M -monitor stdio

# Run with VNC
vnc: $(ISO_FILE)
	qemu-system-i386 -cdrom $(ISO_FILE) -m 512M -vnc :1

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

# Clean and rebuild
rebuild: clean all

# Create initial source files
init:
	@echo "Creating initial source files..."
	@if [ ! -f $(BOOT_DIR)/boot.asm ]; then \
		mkdir -p $(BOOT_DIR); \
		echo "Creating boot.asm..."; \
		touch $(BOOT_DIR)/boot.asm; \
	fi
	@if [ ! -f $(KERNEL_DIR)/main.c ]; then \
		mkdir -p $(KERNEL_DIR); \
		echo "Creating main.c..."; \
		touch $(KERNEL_DIR)/main.c; \
	fi
	@if [ ! -f linker.ld ]; then \
		echo "Creating linker.ld..."; \
		touch linker.ld; \
	fi
	@if [ ! -f grub.cfg ]; then \
		echo "Creating grub.cfg..."; \
		touch grub.cfg; \
	fi

# Install dependencies (Ubuntu/Debian)
install-deps:
	@echo "Installing dependencies..."
	sudo apt update
	sudo apt install -y build-essential nasm qemu-system-x86 grub-pc-bin grub-common xorriso

# Help
help:
	@echo "Sarrus OS Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Build the complete OS (default)"
	@echo "  run          - Build and run in QEMU"
	@echo "  debug        - Build and run in QEMU with GDB server"
	@echo "  monitor      - Build and run in QEMU with monitor"
	@echo "  vnc          - Build and run in QEMU with VNC"
	@echo "  clean        - Remove all build files"
	@echo "  rebuild      - Clean and rebuild"
	@echo "  init         - Create initial source files"
	@echo "  install-deps - Install build dependencies"
	@echo "  help         - Show this help message"

.PHONY: all run debug monitor vnc clean rebuild init install-deps help
