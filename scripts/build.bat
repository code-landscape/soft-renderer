@echo off
setlocal enabledelayedexpansion

set "BUILD_TYPE=Release"


set "SCRIPT_DIR=%~dp0"
for /f "delims=" %%i in ("%SCRIPT_DIR%..") do set "PROJECT_DIR=%%~fi"
for /f "delims=" %%i in ("%SCRIPT_DIR%..\build") do set "BUILD_DIR=%%~fi"
for /f "delims=" %%i in ("%SCRIPT_DIR%..\tools\ninja") do set "NINJA_DIR=%%~fi"

rem === Pre-flight checks ===
where clang++ >nul 2>nul
if errorlevel 1 (
    echo Error: clang++ not found on PATH. Install LLVM/Clang and add bin\ to your PATH.
    exit /b 1
)
where cmake >nul 2>nul
if errorlevel 1 (
    echo Error: cmake not found on PATH.
    exit /b 1
)

rem Detect vcpkg from PATH
where vcpkg >nul 2>nul
if errorlevel 1 (
    echo Error: vcpkg not found on PATH. Add vcpkg to your PATH or run bootstrap-vcpkg.bat first.
    exit /b 1
)
for /f "delims=" %%i in ('where vcpkg') do set "VCPKG_DIR=%%~dpi"
if not exist "%VCPKG_DIR%\scripts\buildsystems\vcpkg.cmake" (
    echo Error: vcpkg toolchain not found at "%VCPKG_DIR%\scripts\buildsystems\vcpkg.cmake"
    exit /b 1
)


rem === Ninja setup: PATH -> local cache -> download ===
where ninja >nul 2>nul
if errorlevel 1 (
    if exist "%NINJA_DIR%\ninja.exe" (
        set "PATH=%NINJA_DIR%;%PATH%"
    ) else (
        if not exist "%NINJA_DIR%" mkdir "%NINJA_DIR%"
        set "NINJA_URL=https://github.com/ninja-build/ninja/releases/download/v1.12.1/ninja-win.zip"
        set "NINJA_ZIP=%NINJA_DIR%\ninja.zip"
        echo Downloading Ninja...
        powershell -NoProfile -Command "$ProgressPreference='SilentlyContinue'; Invoke-WebRequest -Uri '%NINJA_URL%' -OutFile '%NINJA_ZIP%' -UseBasicParsing -ErrorAction Stop"
        if errorlevel 1 (
            echo Error: Failed to download Ninja.
            exit /b 1
        )
        powershell -NoProfile -Command "Expand-Archive -Path '%NINJA_ZIP%' -DestinationPath '%NINJA_DIR%' -Force"
        if errorlevel 1 (
            echo Error: Failed to extract Ninja archive.
            del "%NINJA_ZIP%" 2>nul
            exit /b 1
        )
        del "%NINJA_ZIP%"
        if not exist "%NINJA_DIR%\ninja.exe" (
            echo Error: ninja.exe not found after extraction.
            exit /b 1
        )
        set "PATH=%NINJA_DIR%;%PATH%"
    )
)

rem === CMake configure ===
cmake -S "%PROJECT_DIR%" -B "%BUILD_DIR%" -G Ninja ^
    -DCMAKE_CXX_COMPILER=clang++ ^
    -DCMAKE_C_COMPILER=clang ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
    -DCMAKE_TOOLCHAIN_FILE="%VCPKG_DIR%\scripts\buildsystems\vcpkg.cmake"
if errorlevel 1 (
    echo CMake configuration failed.
    exit /b 1
)

rem === Build ===
cmake --build "%BUILD_DIR%" --config %BUILD_TYPE%
if errorlevel 1 (
    echo Build failed.
    exit /b 1
)

echo. && echo Build successful.
