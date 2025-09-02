# Sarrus OS

A custom operating system built from scratch in C and Assembly with the ultimate goal of running DOOM (1993).

## 🎯 Main Goal

**Run the classic DOOM game natively on a custom-built operating system.**

This project serves as both an educational journey into operating system development and a practical demonstration of building a complete OS capable of running real applications.

## Overview

Sarrus OS is a hobby operating system project designed to learn and understand low-level system programming, kernel development, and computer architecture fundamentals. The development follows a structured approach, building from basic kernel functionality up to a complete OS capable of running DOOM.

## 🎮 Why DOOM?

DOOM is the perfect target application because it requires:
- Graphics system (VGA Mode 13h - 320x200, 256 colors)
- Memory management (dynamic allocation, virtual memory)
- File system (loading WAD files and game assets)
- Input handling (keyboard for controls)
- Sound system (PC Speaker/Sound Blaster)
- Real-time performance (60fps gameplay)

Running DOOM proves the OS has all essential subsystems working correctly.

## Features (Planned)

### Phase 1: Core Kernel ✅

**🔧 Build System & Infrastructure**
- ✅ Cross-platform Makefile with comprehensive targets
- ✅ Cross-compiler toolchain (i686-elf-gcc) integration  
- ✅ GRUB multiboot bootloader configuration
- ✅ QEMU testing environment with debugging support
- ✅ Automated ISO generation and testing

**🖥️ Core Kernel**
- ✅ Multiboot-compliant bootloader (GRUB compatible)
- ✅ VGA text mode terminal with color support
- ✅ Basic kernel initialization and system setup
- ✅ Proper kernel entry point and stack management

**🧠 Advanced Memory Management System**
- ✅ **Physical Memory Manager (PMM)**: Stack-based page frame allocator
- ✅ **Virtual Memory Manager (VMM)**: x86 paging with TLB management  
- ✅ **Kernel Heap**: Dynamic allocation with corruption detection
- ✅ **Memory Protection**: Page-level access control and validation
- ✅ **Statistics & Debugging**: Comprehensive memory usage tracking
- ✅ **Error Handling**: Robust failure detection and recovery

**📚 Complete Documentation**
- ✅ Memory system technical reference ([docs/memory-api.md](docs/memory-api.md))
- ✅ Implementation guide ([docs/memory-implementation.md](docs/memory-implementation.md))
- ✅ System overview ([docs/memory-system.md](docs/memory-system.md))
- ✅ Build and development instructions

### Phase 2: System Infrastructure 🚧
- Virtual memory management (paging, heap allocation)
- Process scheduling and multitasking
- ELF executable loading
- Interrupt and exception handling

### Phase 3: I/O and Graphics 📋
- VGA graphics mode (320x200, 256 colors)
- Keyboard and mouse input drivers
- Sound system (PC Speaker/Sound Blaster)
- File system implementation

### Phase 4: DOOM Integration 🎯
- WAD file format support
- Graphics rendering pipeline
- Input handling for gameplay
- Sound effects and music playback
- **DOOM fully playable on Sarrus OS**

### Additional Features
- Basic shell and command interface
- System calls and C library
- Device drivers for essential hardware
- Debugging and development tools

## Project Structure

```
sarrus-os/
├── src/              # Source code
│   ├── boot/         # Bootloader code
│   ├── kernel/       # Kernel implementation
│   ├── drivers/      # Device drivers
│   ├── libc/         # Standard library implementation
│   ├── fs/           # File system implementation
│   └── mm/           # Memory management
├── include/          # Header files
├── scripts/          # Build and utility scripts
├── build/            # Build output
├── docs/             # Documentation
├── tests/            # Test files
└── tools/            # Development tools
```

## Prerequisites

- GCC cross-compiler (i686-elf-gcc or x86_64-elf-gcc)
- NASM (Netwide Assembler)
- QEMU (for testing and emulation)
- Make
- GRUB (for bootloader)

**For detailed installation instructions, see [Requirements](docs/requirements.md)**

## Building

```bash
# Build the OS
make

# Clean build files
make clean

# Run in QEMU
make run

# Debug with QEMU
make debug
```

## Quick Start

1. Install prerequisites (see [Requirements](docs/requirements.md))
2. Clone the repository
3. Build: `make` (Linux/WSL) or `scripts\build.bat` (Windows)
4. Test: `make run` (Linux/WSL) or `scripts\run.bat` (Windows)

## Development Roadmap

See [plan.md](plan.md) for the complete DOOM development roadmap including:
- Detailed technical specifications
- Phase-by-phase implementation plan
- Estimated timelines and milestones
- Technical challenges and solutions

**Current Status**: Phase 1 Complete - Advanced memory management system operational ✅

## Documentation

See the `docs/` directory for detailed documentation:

### 📋 General Documentation
- [Requirements](docs/requirements.md) - Complete tool installation guide
- [Getting Started](docs/getting-started.md) - Setup and first steps  
- [Architecture](docs/architecture.md) - System design overview

### 🧠 Memory Management Documentation
- [Memory System Overview](docs/memory-system.md) - Comprehensive system architecture
- [Memory API Reference](docs/memory-api.md) - Complete function documentation
- [Implementation Guide](docs/memory-implementation.md) - Design decisions and best practices
- [Development Guide](docs/development.md) - Coding guidelines
- [API Reference](docs/api.md) - Function documentation
- [DOOM Development Plan](plan.md) - Complete roadmap to running DOOM

## Project Timeline

**🎯 Ultimate Goal**: DOOM running natively on Sarrus OS
- **Estimated Timeline**: 6-12 months of development
- **Current Progress**: Phase 1 (Foundation) - ✅ Complete
- **Next Milestone**: Virtual memory management and graphics system

## Technical Specifications

### Minimum Requirements for DOOM
- **CPU**: 486 DX2-66 or compatible (32-bit x86)
- **RAM**: 8 MB (4 MB OS + 4 MB DOOM)
- **Graphics**: VGA Mode 13h (320x200, 256 colors)
- **Storage**: ~50 MB (OS + DOOM WAD files)
- **Sound**: PC Speaker or Sound Blaster compatible

## Contributing

This is primarily a personal learning project with the goal of running DOOM, but contributions and suggestions are welcome! 

Areas where help would be appreciated:
- Graphics system optimization
- Sound driver development  
- DOOM engine porting assistance
- Testing and debugging
- Documentation improvements

## License

See LICENSE file for details.

---

**"The best way to understand an operating system is to build one... and the best way to test it is to run DOOM on it!"** 🎮

*Follow the development journey from basic kernel to DOOM-capable OS!*