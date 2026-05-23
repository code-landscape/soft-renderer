@echo off
setlocal

set "PROJECT_DIR=%~dp0.."
mkdir build 2>nul

rem checkout tools
where clang++ >nul 2>nul
if errorlevel 1 (
    echo Error: clang++ not found
    exit /b 1
)

where vcpkg >nul 2>nul
if errorlevel 1 (
    echo Error: vcpkg not found
    exit /b 1
)
if "%VCPKG_DIR%"=="" for /f "delims=" %%i in ('where vcpkg') do set "VCPKG_DIR=%%~dpi"

where cmake >nul 2>nul
if errorlevel 1 (
    echo Error: cmake not found
    exit /b 1
)

where ninja >nul 2>nul
if errorlevel 1 (
    echo Error: ninja not found
    exit /b 1
)

rem vcpkg configuration
vcpkg install --triplet x64-windows

rem CMake configure
set "CMAKE_TOOLCHAIN_FILE=%VCPKG_DIR%\scripts\buildsystems\vcpkg.cmake"
cmake -S "%PROJECT_DIR%" -B "build" -G Ninja ^
    -DCMAKE_CXX_COMPILER=clang++ ^
    -DCMAKE_C_COMPILER=clang ^
    -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
    echo CMake configuration failed.
    exit /b 1
)

rem build
cmake --build "build" --config Release
if errorlevel 1 (
    echo Build failed.
    exit /b 1
)

echo Build successful.
