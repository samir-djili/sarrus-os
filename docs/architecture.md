# Sarrus OS Architecture

## Overview

Sarrus OS is designed as a monolithic kernel with modular components. The architecture follows traditional OS design patterns while being simple enough for educational purposes.

## System Architecture

```
┌─────────────────────────────────────────┐
│              User Space                 │
├─────────────────────────────────────────┤
│              System Calls               │
├─────────────────────────────────────────┤
│                                         │
│              Kernel Space               │
│                                         │
│  ┌─────────┐ ┌─────────┐ ┌─────────┐   │
│  │ Process │ │ Memory  │ │  File   │   │
│  │ Manager │ │ Manager │ │ System  │   │
│  └─────────┘ └─────────┘ └─────────┘   │
│                                         │
│  ┌─────────┐ ┌─────────┐ ┌─────────┐   │
│  │ Device  │ │ Network │ │ Timer   │   │
│  │ Drivers │ │ Stack   │ │ Manager │   │
│  └─────────┘ └─────────┘ └─────────┘   │
│                                         │
├─────────────────────────────────────────┤
│            Hardware Layer               │
└─────────────────────────────────────────┘
```

## Boot Process

1. **BIOS/UEFI**: Computer starts and loads bootloader
2. **Bootloader (GRUB)**: Loads kernel into memory
3. **Kernel Entry**: Assembly code sets up environment
4. **Kernel Main**: C code initializes subsystems
5. **User Space**: Launch init process and shell

### Detailed Boot Sequence

```
Power On → BIOS → MBR → GRUB → Kernel
                               ↓
                        boot.asm (Assembly)
                               ↓
                        kernel_main() (C)
                               ↓
                        Initialize subsystems
                               ↓
                        Enter main loop
```

## Memory Layout

### Physical Memory Map

```
0x00000000 - 0x000FFFFF: Real Mode Memory (1MB)
0x00100000 - 0x00FFFFFF: Kernel Space (15MB)
0x01000000 - 0xFFFFFFFF: User Space + Free Memory
```

### Virtual Memory Layout

```
0x00000000 - 0x3FFFFFFF: User Space (1GB)
0x40000000 - 0x7FFFFFFF: Shared Libraries (1GB)
0x80000000 - 0xBFFFFFFF: Kernel Heap (1GB)
0xC0000000 - 0xFFFFFFFF: Kernel Space (1GB)
```

## Kernel Components

### Core Subsystems

1. **Memory Management**
   - Physical memory allocator
   - Virtual memory manager (paging)
   - Heap allocator (kmalloc/kfree)
   - Memory protection

2. **Process Management**
   - Process control blocks (PCB)
   - Scheduler (round-robin, priority)
   - Context switching
   - Inter-process communication

3. **File System**
   - Virtual File System (VFS) layer
   - Disk drivers
   - File operations
   - Directory management

4. **Device Drivers**
   - Character devices (keyboard, serial)
   - Block devices (disk, RAM disk)
   - Network devices
   - Graphics devices

### Interrupt Handling

```
Hardware Interrupt → IDT → Interrupt Handler → Kernel Service → Return
```

## System Call Interface

### System Call Mechanism

1. User program calls library function
2. Library sets up registers and triggers interrupt
3. Kernel handles system call interrupt
4. Kernel executes requested service
5. Return to user space with result

### System Call Categories

- **Process Control**: fork, exec, exit, wait
- **File Operations**: open, read, write, close
- **Memory Management**: mmap, munmap, brk
- **Communication**: pipe, socket, signal
- **Device I/O**: ioctl, select, poll

## Threading Model

### Kernel Threads
- Cooperative multitasking initially
- Preemptive multitasking (future)
- Kernel-level threads only

### User Threads
- User-level thread library (future)
- 1:1 threading model (one user thread per kernel thread)

## Security Model

### Protection Mechanisms
- Memory protection via paging
- User/kernel mode separation
- System call validation
- Resource limits

### Future Security Features
- Access control lists (ACLs)
- Capabilities system
- Secure boot
- Address space layout randomization (ASLR)

## File System Architecture

### VFS Layer
```
Application
     ↓
System Calls
     ↓
VFS Interface
     ↓
File System Drivers (ext2, fat32, etc.)
     ↓
Block Device Interface
     ↓
Device Drivers
     ↓
Hardware
```

## Network Stack (Future)

### OSI Model Implementation
```
Application Layer    → Socket API
Transport Layer      → TCP/UDP
Network Layer        → IP
Data Link Layer      → Ethernet
Physical Layer       → Network Drivers
```

## Performance Considerations

### Optimization Strategies
- Efficient memory allocation
- Smart scheduling algorithms
- I/O request batching
- Caching mechanisms
- Lazy loading where possible

### Bottlenecks to Address
- Context switching overhead
- System call latency
- Memory allocation fragmentation
- I/O wait times

## Design Principles

1. **Simplicity**: Keep code readable and maintainable
2. **Modularity**: Design components that can be easily replaced
3. **Performance**: Optimize critical paths
4. **Reliability**: Handle errors gracefully
5. **Security**: Implement defense in depth

## Development Phases

### Phase 1: Foundation
- Basic boot process
- Memory management
- Simple VGA output

### Phase 2: Core Services
- Process management
- System calls
- Basic drivers

### Phase 3: User Space
- Shell implementation
- File system
- User programs

### Phase 4: Advanced Features
- Networking
- Graphics
- Advanced security

## References

- [OSDev.org](https://wiki.osdev.org/) - OS Development Wiki
- [Intel x86 Manuals](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
- [AMD64 Architecture](https://www.amd.com/system/files/TechDocs/24593.pdf)
- [Linux Kernel Documentation](https://www.kernel.org/doc/html/latest/)
- [MINIX Book](https://www.cs.vu.nl/~ast/books/mos2/) - Operating Systems Design and Implementation
