@echo off
REM Sarrus OS Setup Script
REM This script sets up the initial development environment

echo Setting up Sarrus OS development environment...

REM Create additional directories that might be needed
if not exist "src\drivers" mkdir "src\drivers"
if not exist "src\libc" mkdir "src\libc"
if not exist "src\fs" mkdir "src\fs"
if not exist "src\mm" mkdir "src\mm"
if not exist "tests" mkdir "tests"
if not exist "tools" mkdir "tools"

REM Create placeholder files to maintain directory structure
echo. > src\drivers\.placeholder
echo. > src\libc\.placeholder
echo. > src\fs\.placeholder
echo. > src\mm\.placeholder
echo. > tests\.placeholder
echo. > tools\.placeholder

REM Check for required tools
echo Checking for required tools...

where i686-elf-gcc >nul 2>&1
if errorlevel 1 (
    echo [WARNING] i686-elf-gcc not found in PATH
    echo Please install a cross-compiler toolchain
) else (
    echo [OK] Cross-compiler found
)

where nasm >nul 2>&1
if errorlevel 1 (
    echo [WARNING] NASM not found in PATH
    echo Please install NASM assembler
) else (
    echo [OK] NASM found
)

where qemu-system-i386 >nul 2>&1
if errorlevel 1 (
    echo [WARNING] QEMU not found in PATH
    echo Please install QEMU emulator
) else (
    echo [OK] QEMU found
)

echo.
echo Setup complete!
echo.
echo Next steps:
echo 1. Install missing tools (see docs/getting-started.md)
echo 2. Run 'scripts\build.bat' to build the OS
echo 3. Run 'scripts\run.bat' to test in QEMU
echo 4. Check TODO.md for development roadmap
echo.

pause
