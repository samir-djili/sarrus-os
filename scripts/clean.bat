@echo off
REM Clean build files

echo Cleaning build files...

if exist "build" (
    rmdir /s /q build
    echo Build directory removed
) else (
    echo Build directory not found
)

echo Clean completed!
