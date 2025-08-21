# Getting Started with Sarrus OS Development

## Prerequisites

Before you can build and run Sarrus OS, you need to install the following tools:

## 🛠️ Required Tools for Sarrus OS Development

### **Essential Build Tools**

#### 1. **Cross-Compiler Toolchain**
- **i686-elf-gcc** (GCC cross-compiler for 32-bit x86)
- **i686-elf-ld** (Linker)
- **i686-elf-objcopy** (Object file utilities)

**Installation Options:**
- **Pre-built binaries**: Download from [lordmilko's releases](https://github.com/lordmilko/i686-elf-tools/releases)
- **Build from source**: Follow [OSDev.org GCC Cross-Compiler guide](https://wiki.osdev.org/GCC_Cross-Compiler)
- **Package managers**: 
  - MSYS2: `pacman -S mingw-w64-i686-gcc`
  - WSL Ubuntu: Build from source or use existing packages

#### 2. **Assembler**
- **NASM** (Netwide Assembler)
- Download from: [NASM official site](https://www.nasm.us/pub/nasm/releasebuilds/)
- Add to Windows PATH environment variable

#### 3. **Emulator**
- **QEMU** (Quick Emulator)
- Download from: [QEMU official site](https://www.qemu.org/download/)
- Specifically need: `qemu-system-i386` for 32-bit x86 emulation

#### 4. **Bootloader Tools**
- **GRUB** (GRand Unified Bootloader)
- **grub-mkrescue** utility
- **xorriso** (ISO creation utility)

**Installation Options:**
- **WSL/Linux**: `sudo apt install grub-pc-bin grub-common xorriso`
- **MSYS2**: `pacman -S grub xorriso`
- **Docker**: Use a Linux container with GRUB tools

### **Development Tools**

#### 5. **Build System**
- **Make** (GNU Make)
- **Windows alternatives**: 
  - Use the provided `.bat` scripts
  - Install Make via MSYS2 or WSL

#### 6. **Debugger**
- **GDB** (GNU Debugger)
- Specifically: `i686-elf-gdb` for cross-debugging
- Used with QEMU's GDB server (`-s -S` flags)

### **Optional but Recommended**

#### 7. **Code Editor/IDE**
- **VS Code** with C/C++ extension
- **CLion** or other C IDEs
- **Vim/Emacs** with appropriate plugins

#### 8. **Version Control**
- **Git** (already have this)

#### 9. **Documentation Tools**
- **Markdown viewer/editor**
- **Doxygen** (for API documentation generation)

### Windows Setup

#### **Option 1: WSL2 (Recommended)**
1. **Install WSL2** with Ubuntu:
   ```powershell
   wsl --install
   ```

2. **Install all tools in WSL**:
   ```bash
   # Update package list
   sudo apt update
   
   # Install build tools
   sudo apt install build-essential nasm
   
   # Install QEMU
   sudo apt install qemu-system-x86
   
   # Install GRUB tools
   sudo apt install grub-pc-bin grub-common xorriso
   
   # Install cross-compiler dependencies
   sudo apt install gcc-multilib
   ```

3. **Build cross-compiler** (if needed):
   ```bash
   # Follow OSDev.org guide for building i686-elf-gcc
   # Or use pre-built packages if available
   ```

#### **Option 2: Native Windows**
1. **Install Cross-Compiler**:
   - Download i686-elf-gcc from [lordmilko's releases](https://github.com/lordmilko/i686-elf-tools/releases)
   - Extract and add to PATH

2. **Install NASM**:
   - Download from [NASM official site](https://www.nasm.us/pub/nasm/releasebuilds/)
   - Install and add to PATH

3. **Install QEMU**:
   - Download from [QEMU official site](https://www.qemu.org/download/)
   - Install and add to PATH

4. **Install GRUB tools**:
   - Use MSYS2: `pacman -S grub xorriso`
   - Or use Docker with Linux container

### Linux Setup (Ubuntu/Debian)

```bash
# Install build tools
sudo apt update
sudo apt install build-essential nasm

# Install QEMU
sudo apt install qemu-system-x86

# Install GRUB tools
sudo apt install grub-pc-bin grub-common xorriso

# Install cross-compiler (or build from source)
sudo apt install gcc-multilib

# Optional: Install GDB for debugging
sudo apt install gdb
```

## 🚀 Quick Setup Commands

### **Windows (PowerShell)**
```powershell
# Check if tools are installed
where i686-elf-gcc
where nasm
where qemu-system-i386

# Run setup script to check environment
scripts\setup.bat

# Build the OS
scripts\build.bat

# Run in QEMU
scripts\run.bat

# Debug with GDB
scripts\debug.bat
```

### **WSL/Linux**
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt update
sudo apt install build-essential nasm qemu-system-x86 grub-pc-bin grub-common xorriso

# Build and run
make
make run

# Debug with GDB
make debug
```

## 🔧 Tool Installation Priority

### **High Priority (Essential)**
1. **i686-elf-gcc** - Cross-compiler for building kernel
2. **NASM** - Assembler for bootloader and low-level code
3. **QEMU** - Emulator for testing the OS

### **Medium Priority (Important)**
4. **GRUB tools** - For creating bootable ISOs
5. **Make** - Build automation
6. **GDB** - Debugging support

### **Low Priority (Nice to have)**
7. **Advanced IDEs** - Better development experience
8. **Documentation tools** - For generating docs

## 🚨 Common Issues & Solutions

### **"i686-elf-gcc not found"**
**Problem**: Cross-compiler not installed or not in PATH
**Solutions**:
- Download pre-built toolchain from [lordmilko's releases](https://github.com/lordmilko/i686-elf-tools/releases)
- Build from source following [OSDev.org guide](https://wiki.osdev.org/GCC_Cross-Compiler)
- Add installation directory to PATH environment variable
- Verify with: `i686-elf-gcc --version`

### **"NASM not found"**
**Problem**: NASM assembler not installed
**Solutions**:
- Download from [NASM website](https://www.nasm.us/pub/nasm/releasebuilds/)
- Install and add to PATH
- Verify with: `nasm --version`

### **"QEMU not starting"**
**Problem**: QEMU emulator not installed or not in PATH
**Solutions**:
- Install QEMU for Windows from [official site](https://www.qemu.org/download/)
- Add QEMU installation directory to PATH
- Verify with: `qemu-system-i386 --version`

### **"grub-mkrescue not found"**
**Problem**: GRUB tools not available on Windows
**Solutions**:
- Use WSL2 with Linux GRUB tools (recommended)
- Install GRUB via MSYS2: `pacman -S grub xorriso`
- Use Docker container with GRUB tools
- Alternative: Implement simple custom bootloader

### **Build errors with missing headers**
**Problem**: Standard library headers not found
**Solutions**:
- Ensure cross-compiler is properly installed
- Check that you're using `i686-elf-gcc` not regular `gcc`
- Verify compiler flags in Makefile

### **Permission denied errors**
**Problem**: Script execution permissions
**Solutions**:
- On Windows: Run PowerShell as Administrator
- On Linux: Make scripts executable with `chmod +x scripts/*`

## 💡 Recommended Setup Approaches

### **For Beginners (Easiest)**
1. **Install WSL2** with Ubuntu
2. **Install all tools in WSL**:
   ```bash
   sudo apt install build-essential nasm qemu-system-x86 grub-pc-bin grub-common xorriso
   ```
3. **Use VS Code** with WSL extension
4. **Build and test** entirely in WSL environment

### **For Advanced Users**
1. **Native Windows** with all tools installed
2. **Docker containers** for isolated build environments
3. **Virtual machines** for testing on different hardware

### **Tool Verification Script**
Run this to check your setup:

**Windows (PowerShell)**:
```powershell
# Create verification script
echo "Checking Sarrus OS development environment..."
echo ""

# Check cross-compiler
if (Get-Command i686-elf-gcc -ErrorAction SilentlyContinue) {
    echo "✓ i686-elf-gcc found: $((i686-elf-gcc --version) -split '\n' | Select-Object -First 1)"
} else {
    echo "✗ i686-elf-gcc not found"
}

# Check NASM
if (Get-Command nasm -ErrorAction SilentlyContinue) {
    echo "✓ NASM found: $((nasm --version) -split '\n' | Select-Object -First 1)"
} else {
    echo "✗ NASM not found"
}

# Check QEMU
if (Get-Command qemu-system-i386 -ErrorAction SilentlyContinue) {
    echo "✓ QEMU found: $((qemu-system-i386 --version) -split '\n' | Select-Object -First 1)"
} else {
    echo "✗ QEMU not found"
}
```

**Linux/WSL**:
```bash
#!/bin/bash
echo "Checking Sarrus OS development environment..."
echo ""

# Check cross-compiler
if command -v i686-elf-gcc &> /dev/null; then
    echo "✓ i686-elf-gcc found: $(i686-elf-gcc --version | head -n1)"
else
    echo "✗ i686-elf-gcc not found"
fi

# Check NASM
if command -v nasm &> /dev/null; then
    echo "✓ NASM found: $(nasm --version)"
else
    echo "✗ NASM not found"
fi

# Check QEMU
if command -v qemu-system-i386 &> /dev/null; then
    echo "✓ QEMU found: $(qemu-system-i386 --version | head -n1)"
else
    echo "✗ QEMU not found"
fi

# Check GRUB
if command -v grub-mkrescue &> /dev/null; then
    echo "✓ GRUB found: $(grub-mkrescue --version | head -n1)"
else
    echo "✗ GRUB not found"
fi
```

## Building the OS

### Using Make (Linux/WSL)

```bash
# Build the complete OS
make

# Clean build files
make clean

# Run in QEMU
make run

# Debug with GDB
make debug
```

### Using Windows Batch Scripts

```cmd
REM Build the OS
scripts\build.bat

REM Clean build files
scripts\clean.bat

REM Run in QEMU
scripts\run.bat

REM Debug with GDB
scripts\debug.bat
```

## Project Structure Explained

```
sarrus-os/
├── src/
│   ├── boot/           # Bootloader code
│   │   └── boot.asm    # Multiboot-compliant bootloader
│   ├── kernel/         # Kernel implementation
│   │   └── main.c      # Kernel entry point and basic functions
│   ├── drivers/        # Device drivers (to be implemented)
│   ├── libc/          # Standard library implementation
│   ├── fs/            # File system implementation
│   └── mm/            # Memory management
├── include/           # Header files
├── scripts/           # Build and utility scripts
├── build/             # Build output (generated)
├── docs/              # Documentation
├── tests/             # Test files
├── tools/             # Development tools
├── Makefile           # Build configuration
├── linker.ld          # Linker script
└── grub.cfg          # GRUB configuration
```

## First Steps

1. **Clone and build:**
   ```bash
   git clone <repository-url>
   cd sarrus-os
   make  # or scripts\build.bat on Windows
   ```

2. **Test in QEMU:**
   ```bash
   make run  # or scripts\run.bat on Windows
   ```

3. **Start developing:**
   - Check `TODO.md` for development roadmap
   - Read `docs/architecture.md` for system design
   - Follow `docs/development.md` for coding guidelines

## Troubleshooting

### Common Issues

1. **Cross-compiler not found:**
   - Ensure i686-elf-gcc is in your PATH
   - Verify installation with `i686-elf-gcc --version`

2. **QEMU not starting:**
   - Check if QEMU is installed and in PATH
   - Try running `qemu-system-i386 --version`

3. **GRUB errors:**
   - Ensure grub-mkrescue is available
   - Check if xorriso is installed (Linux)

4. **Build errors:**
   - Check if all source files exist
   - Verify file paths in Makefile
   - Ensure proper file permissions

### Getting Help

- Read the documentation in `docs/`
- Check the [OSDev.org wiki](https://wiki.osdev.org/)
- Review the project's issue tracker
- Join OS development communities

## Next Steps

Once you have successfully built and run the basic kernel:

1. Follow the TODO.md roadmap
2. Implement basic kernel features
3. Add device drivers
4. Develop user space programs
5. Build a simple shell

Happy OS development!
