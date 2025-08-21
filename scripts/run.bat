@echo off
REM Run Sarrus OS in QEMU

echo Starting Sarrus OS in QEMU...

REM Check if ISO exists
if not exist "build\sarrus-os.iso" (
    echo ISO file not found. Building first...
    call scripts\build.bat
    if errorlevel 1 (
        echo Build failed
        exit /b 1
    )
)

REM Run QEMU
qemu-system-i386 -cdrom build\sarrus-os.iso -m 512M

if errorlevel 1 (
    echo Error running QEMU. Make sure QEMU is installed and in PATH.
    echo Download QEMU from: https://www.qemu.org/download/
)

pause
