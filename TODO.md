# Sarrus OS Development TODO

## Phase 1: Basic Boot and Setup

### Bootloader
- [ ] Create basic bootloader in Assembly (boot.asm)
- [ ] Set up 16-bit to 32-bit mode transition
- [ ] Load kernel from disk
- [ ] Set up initial GDT (Global Descriptor Table)
- [ ] Enable A20 line
- [ ] Set up basic memory map

### Build System
- [ ] Create Makefile for automated building
- [ ] Set up cross-compiler toolchain
- [ ] Configure linker script
- [ ] Set up GRUB configuration
- [ ] Create ISO image generation

### Development Environment
- [ ] Set up QEMU testing scripts
- [ ] Configure debugging with GDB
- [ ] Create development documentation

## Phase 2: Kernel Foundation

### Basic Kernel
- [ ] Kernel entry point in C
- [ ] Basic VGA text mode output
- [ ] Kernel printf implementation
- [ ] Basic error handling and panic function
- [ ] Kernel logging system

### Memory Management
- [ ] Physical memory manager
- [ ] Virtual memory manager (paging)
- [ ] Heap allocator (malloc/free)
- [ ] Memory protection and isolation
- [ ] Stack overflow protection

### Interrupts and Exceptions
- [ ] IDT (Interrupt Descriptor Table) setup
- [ ] Basic exception handlers
- [ ] Timer interrupt (PIT)
- [ ] Keyboard interrupt
- [ ] System call interface

## Phase 3: Core System Components

### Process Management
- [ ] Process control blocks (PCB)
- [ ] Basic scheduler (round-robin)
- [ ] Context switching
- [ ] Process creation/termination
- [ ] Inter-process communication (IPC)

### File System
- [ ] Virtual File System (VFS) layer
- [ ] Simple file system implementation
- [ ] File operations (open, read, write, close)
- [ ] Directory operations
- [ ] File permissions

### Device Drivers
- [ ] Driver framework/interface
- [ ] Keyboard driver
- [ ] VGA/graphics driver
- [ ] Serial port driver
- [ ] Timer driver
- [ ] Disk/storage driver

## Phase 4: User Space and Shell

### System Calls
- [ ] System call interface
- [ ] Process management syscalls
- [ ] File system syscalls
- [ ] Memory management syscalls
- [ ] I/O syscalls

### User Space
- [ ] User mode execution
- [ ] ELF file loading
- [ ] Basic C library for user programs
- [ ] Standard I/O implementation

### Shell
- [ ] Basic command interpreter
- [ ] Built-in commands (ls, cat, echo, etc.)
- [ ] Program execution
- [ ] Command history
- [ ] Environment variables

## Phase 5: Advanced Features

### Networking
- [ ] Network stack foundation
- [ ] Ethernet driver
- [ ] Basic TCP/IP implementation
- [ ] Socket interface

### Graphics
- [ ] Basic graphics mode
- [ ] Window system foundation
- [ ] Font rendering
- [ ] Mouse support

### Security
- [ ] User permissions system
- [ ] Memory protection
- [ ] Secure system calls
- [ ] Basic encryption support

## Phase 6: Optimization and Polish

### Performance
- [ ] Kernel optimization
- [ ] Memory usage optimization
- [ ] I/O performance improvements
- [ ] Scheduler improvements

### Debugging and Testing
- [ ] Comprehensive test suite
- [ ] Kernel debugging tools
- [ ] Memory leak detection
- [ ] Performance profiling

### Documentation
- [ ] Complete API documentation
- [ ] User manual
- [ ] Developer guide
- [ ] Architecture documentation

## Ongoing Tasks

### Code Quality
- [ ] Code review and refactoring
- [ ] Consistent coding style
- [ ] Error handling improvements
- [ ] Memory safety checks

### Testing
- [ ] Unit tests for kernel functions
- [ ] Integration tests
- [ ] Stress testing
- [ ] Hardware compatibility testing

### Documentation
- [ ] Keep documentation up to date
- [ ] Code comments and explanations
- [ ] Build and setup instructions
- [ ] Troubleshooting guide

## Future Ideas

- [ ] SMP (Symmetric Multiprocessing) support
- [ ] 64-bit architecture support
- [ ] Modern file systems (ext2/3/4)
- [ ] USB support
- [ ] Audio support
- [ ] Package manager
- [ ] GUI framework

## Notes

- Each major feature should be implemented incrementally
- Test thoroughly at each phase before moving to the next
- Maintain backwards compatibility where possible
- Document all design decisions and trade-offs
- Regular code reviews and refactoring sessions

## Priority Levels

🔴 **High Priority**: Essential for basic OS functionality
🟡 **Medium Priority**: Important for usability
🟢 **Low Priority**: Nice-to-have features
🔵 **Future**: Long-term goals

Last updated: August 21, 2025
