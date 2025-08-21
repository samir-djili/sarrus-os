@echo off
REM Debug Sarrus OS in QEMU with GDB

echo Starting Sarrus OS in QEMU with GDB server...

REM Check if ISO exists
if not exist "build\sarrus-os.iso" (
    echo ISO file not found. Building first...
    call scripts\build.bat
    if errorlevel 1 (
        echo Build failed
        exit /b 1
    )
)

echo Starting QEMU with GDB server on port 1234...
echo In another terminal, run: i686-elf-gdb build\kernel.elf
echo Then in GDB: target remote localhost:1234

REM Run QEMU with GDB server
qemu-system-i386 -cdrom build\sarrus-os.iso -m 512M -s -S

pause
