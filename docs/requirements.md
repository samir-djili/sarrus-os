# Sarrus OS Requirements

## System Requirements

### Minimum Hardware
- **CPU**: x86 32-bit processor (i386 or later)
- **RAM**: 512 MB (for QEMU emulation)
- **Storage**: 100 MB for development environment
- **OS**: Windows 10/11, Linux (Ubuntu 20.04+), or macOS

### Development Environment
- **Host OS**: Windows, Linux, or macOS
- **Terminal**: PowerShell (Windows), Bash (Linux/macOS)
- **Internet**: Required for downloading tools and dependencies

## Required Software Tools

### Core Build Tools (Essential)

| Tool | Purpose | Windows Installation | Linux Installation |
|------|---------|---------------------|-------------------|
| **i686-elf-gcc** | Cross-compiler for x86 | [Download pre-built](https://github.com/lordmilko/i686-elf-tools/releases) | Build from source or package manager |
| **NASM** | Assembler | [Download](https://www.nasm.us/pub/nasm/releasebuilds/) | `sudo apt install nasm` |
| **QEMU** | Emulator | [Download](https://www.qemu.org/download/) | `sudo apt install qemu-system-x86` |

### Bootloader Tools (Important)

| Tool | Purpose | Windows Installation | Linux Installation |
|------|---------|---------------------|-------------------|
| **grub-mkrescue** | ISO creation | WSL2 or MSYS2 | `sudo apt install grub-pc-bin` |
| **xorriso** | ISO filesystem | WSL2 or MSYS2 | `sudo apt install xorriso` |

### Development Tools (Recommended)

| Tool | Purpose | Installation |
|------|---------|-------------|
| **Make** | Build automation | WSL2, MSYS2, or use .bat scripts |
| **GDB** | Debugging | Part of cross-compiler toolchain |
| **Git** | Version control | Already installed |
| **VS Code** | Code editor | [Download](https://code.visualstudio.com/) |

## Installation Methods

### Method 1: WSL2 (Recommended for Windows)

**Advantages:**
- Complete Linux environment on Windows
- Easy package management
- All tools available through apt
- Best compatibility with build scripts

**Installation:**
```powershell
# Install WSL2
wsl --install

# In WSL Ubuntu terminal
sudo apt update
sudo apt install build-essential nasm qemu-system-x86 grub-pc-bin grub-common xorriso
```

### Method 2: Native Windows

**Advantages:**
- No virtualization overhead
- Direct Windows integration
- Faster file system access

**Requirements:**
- Manual tool installation
- PATH configuration
- Limited GRUB support

### Method 3: Docker Container

**Advantages:**
- Isolated environment
- Consistent across platforms
- Easy to share and reproduce

**Setup:**
```dockerfile
FROM ubuntu:22.04
RUN apt update && apt install -y build-essential nasm qemu-system-x86 grub-pc-bin xorriso
# Add cross-compiler build steps
```

## Version Requirements

### Minimum Versions

| Tool | Minimum Version | Recommended Version |
|------|----------------|-------------------|
| **GCC** | 9.0 | Latest stable |
| **NASM** | 2.14 | Latest stable |
| **QEMU** | 5.0 | Latest stable |
| **Make** | 4.0 | Latest stable |
| **GRUB** | 2.04 | Latest stable |

### Compatibility Notes

- **GCC**: Must be cross-compiled for i686-elf target
- **NASM**: Intel syntax support required
- **QEMU**: i386 system emulation required
- **GRUB**: Multiboot specification support needed

## Pre-built Tool Downloads

### Windows Pre-built Packages

1. **Cross-Compiler Toolchain:**
   - [i686-elf-tools by lordmilko](https://github.com/lordmilko/i686-elf-tools/releases)
   - [i686-elf-gcc builds](https://github.com/nativeos/i686-elf-tools/releases)

2. **NASM:**
   - [Official NASM releases](https://www.nasm.us/pub/nasm/releasebuilds/)

3. **QEMU:**
   - [Official QEMU downloads](https://www.qemu.org/download/)

### Package Managers

**MSYS2 (Windows):**
```bash
pacman -S mingw-w64-i686-gcc nasm qemu grub xorriso
```

**Homebrew (macOS):**
```bash
brew install i686-elf-gcc nasm qemu grub xorriso
```

**APT (Ubuntu/Debian):**
```bash
sudo apt install build-essential nasm qemu-system-x86 grub-pc-bin xorriso
```

## Environment Setup

### PATH Configuration

**Windows:**
Add these directories to your PATH environment variable:
- `C:\path\to\i686-elf-gcc\bin`
- `C:\path\to\nasm`
- `C:\path\to\qemu`

**Linux/macOS:**
Add to your shell profile (`.bashrc`, `.zshrc`):
```bash
export PATH="/usr/local/cross/bin:$PATH"
```

### Environment Variables

```bash
# Cross-compiler prefix
export CROSS_COMPILE=i686-elf-

# Compiler flags
export CFLAGS="-std=gnu99 -ffreestanding -O2 -Wall -Wextra"

# Target architecture
export TARGET=i686-elf
```

## Verification Commands

### Quick Check
```bash
# Check all required tools
i686-elf-gcc --version
nasm --version
qemu-system-i386 --version
grub-mkrescue --version    # May not work on Windows
make --version
```

### Build Test
```bash
# Try building the OS
make                       # Linux/WSL
scripts\build.bat         # Windows

# Try running in QEMU
make run                   # Linux/WSL
scripts\run.bat           # Windows
```

## Alternative Solutions

### If Cross-Compiler Build Fails
1. Use pre-built binaries
2. Try different GCC versions
3. Use Docker with pre-built environment
4. Use cloud development environment

### If GRUB Tools Unavailable
1. Use simple custom bootloader
2. Use WSL2 for GRUB tools
3. Use Docker container
4. Boot directly with QEMU (no ISO)

### If QEMU Issues
1. Try different QEMU versions
2. Use VirtualBox with manual setup
3. Use hardware testing (advanced)
4. Use cloud emulation services

## Troubleshooting Matrix

| Problem | Symptom | Solution |
|---------|---------|----------|
| Missing cross-compiler | `i686-elf-gcc: command not found` | Install cross-compiler toolchain |
| Wrong GCC | `crt0.o not found` | Use i686-elf-gcc, not system gcc |
| Missing NASM | `nasm: command not found` | Install NASM assembler |
| QEMU not found | `qemu-system-i386: command not found` | Install QEMU emulator |
| GRUB missing | `grub-mkrescue: command not found` | Use WSL2 or alternative bootloader |
| Build errors | Various compilation errors | Check tool versions and flags |
| Boot fails | Black screen or crash | Check bootloader and kernel code |

This requirements document provides a comprehensive guide for setting up the Sarrus OS development environment.
