@echo off
REM Sarrus OS Build Script for Windows

echo Building Sarrus OS...

REM Check if build directory exists
if not exist "build" mkdir build
if not exist "build\kernel" mkdir "build\kernel"
if not exist "build\boot" mkdir "build\boot"
if not exist "build\iso" mkdir "build\iso"
if not exist "build\iso\boot" mkdir "build\iso\boot"
if not exist "build\iso\boot\grub" mkdir "build\iso\boot\grub"

REM Build bootloader
echo Assembling bootloader...
nasm -f elf32 src\boot\boot.asm -o build\boot.o
if errorlevel 1 (
    echo Error assembling bootloader
    exit /b 1
)

REM Build kernel
echo Compiling kernel...
i686-elf-gcc -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Iinclude -c src\kernel\main.c -o build\kernel\main.o
if errorlevel 1 (
    echo Error compiling kernel
    exit /b 1
)

REM Link kernel
echo Linking kernel...
i686-elf-ld -T linker.ld -nostdlib -o build\kernel.elf build\boot.o build\kernel\main.o -lgcc
if errorlevel 1 (
    echo Error linking kernel
    exit /b 1
)

REM Create ISO
echo Creating ISO image...
copy build\kernel.elf build\iso\boot\
copy grub.cfg build\iso\boot\grub\
grub-mkrescue -o build\sarrus-os.iso build\iso
if errorlevel 1 (
    echo Error creating ISO
    exit /b 1
)

echo Build completed successfully!
echo ISO image: build\sarrus-os.iso
