@echo off
setlocal

REM ============================================================================
REM Path Configuration
REM ============================================================================

REM SCRIPT_DIR: Directory where this batch file resides (scripts/)
set "SCRIPT_DIR=%~dp0"

REM PROJECT_DIR: Project root (parent of scripts/)
set "PROJECT_DIR=%SCRIPT_DIR%.."

REM BUILD_DIR: Output directory for CMake build artifacts
set "BUILD_DIR=%SCRIPT_DIR%..\build"

REM LLVM_DIR: Installation path of LLVM/Clang toolchain
set "LLVM_DIR=C:\Program Files\LLVM"

REM VCPKG_DIR: Root of vcpkg package manager (provides TBB, SDL3, etc.)
set "VCPKG_DIR=C:\Users\ipxie\vcpkg"

REM VCPKG_TRIPLET: Target architecture triplet for vcpkg
set "VCPKG_TRIPLET=x64-windows"

REM ============================================================================
REM Compiler Detection
REM ============================================================================

REM Use LLVM Clang as the C++ compiler
set "CLANGXX=%LLVM_DIR%\bin\clang++.exe"

REM Use LLVM Clang as the C compiler
set "CLANG=%LLVM_DIR%\bin\clang.exe"

REM Bail early if clang++ is not installed
if not exist "%CLANGXX%" (
    echo Error: clang++ not found at %CLANGXX%
    exit /b 1
)

REM ============================================================================
REM Ninja Build System Setup
REM ============================================================================

REM Directory to cache a local Ninja download (fallback if not on PATH)
set "NINJA_TOOL_DIR=%SCRIPT_DIR%..\tools\ninja"
set "NINJA_EXE=%NINJA_TOOL_DIR%\ninja.exe"

REM First, try to find Ninja on the system PATH (e.g., installed via WinGet)
where ninja >nul 2>nul
if %errorlevel% equ 0 (
    set "NINJA_EXE=ninja"

REM Fallback: use a previously cached local copy
) else if exist "%NINJA_EXE%" (
    set "PATH=%NINJA_TOOL_DIR%;%PATH%"

REM Last resort: download Ninja from GitHub and cache it locally
) else (
    echo Downloading Ninja...
    if not exist "%NINJA_TOOL_DIR%" mkdir "%NINJA_TOOL_DIR%"

    REM Fetch the Ninja Windows binary zip from the official GitHub release
    powershell -Command "Invoke-WebRequest -Uri 'https://github.com/ninja-build/ninja/releases/download/v1.12.1/ninja-win.zip' -OutFile '%NINJA_TOOL_DIR%\ninja.zip'"
    if %errorlevel% neq 0 (
        echo Failed to download Ninja.
        pause
        exit /b 1
    )

    REM Extract the zip and clean up
    powershell -Command "Expand-Archive -Path '%NINJA_TOOL_DIR%\ninja.zip' -DestinationPath '%NINJA_TOOL_DIR%' -Force"
    del "%NINJA_TOOL_DIR%\ninja.zip"

    REM Verify extraction succeeded
    if not exist "%NINJA_EXE%" (
        echo Failed to extract Ninja.
        pause
        exit /b 1
    )

    REM Add the cached Ninja to PATH
    set "PATH=%NINJA_TOOL_DIR%;%PATH%"
)

REM ============================================================================
REM CMake Configuration
REM ============================================================================

echo Configuring CMake with Ninja + LLVM clang + vcpkg...

REM Key flags:
REM   -G Ninja        : Use the Ninja build system (fast, parallel builds)
REM   CMAKE_CXX_COMPILER : Force Clang as the C++ compiler
REM   CMAKE_C_COMPILER   : Force Clang as the C compiler
REM   CMAKE_BUILD_TYPE   : Release mode (optimized, no debug symbols)
REM   CMAKE_TOOLCHAIN_FILE : vcpkg toolchain for automatic dependency resolution
REM   VCPKG_TARGET_TRIPLET : x64-windows (64-bit Windows libraries)
cmake -S "%PROJECT_DIR%" -B "%BUILD_DIR%" -G Ninja -DCMAKE_CXX_COMPILER="%CLANGXX%" -DCMAKE_C_COMPILER="%CLANG%" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="%VCPKG_DIR%\scripts\buildsystems\vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=%VCPKG_TRIPLET%

REM Abort on configuration failure
if errorlevel 1 (
    echo CMake configuration failed.
    pause
    exit /b 1
)

REM ============================================================================
REM Build
REM ============================================================================

echo Building project...

REM Invoke Ninja via CMake to compile all targets
cmake --build "%BUILD_DIR%" --config Release

REM Abort on build failure
if errorlevel 1 (
    echo Build failed.
    pause
    exit /b 1
)

REM ============================================================================
REM Success
REM ============================================================================

echo.
echo Build successful.

REM Keep the terminal open so the user can inspect the output
pause
