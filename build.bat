@echo off
REM Windows Build Script for Limit Order Manager
REM Supports different build configurations

setlocal enabledelayedexpansion

REM Configuration
set CXX=g++
set CXXFLAGS=-std=c++17 -Wall -Wextra -pedantic
set INCLUDES=-Iinclude
set SOURCES=src\main.cpp src\order_manager.cpp
set TARGET=limit_order_manager.exe

REM Check if g++ is available
where g++ >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: g++ compiler not found in PATH
    echo.
    echo Please install one of the following:
    echo 1. MinGW-w64: https://www.mingw-w64.org/
    echo 2. MSYS2: https://www.msys2.org/
    echo 3. Visual Studio Build Tools with C++ support
    echo.
    echo After installation, make sure the compiler is in your PATH
    pause
    exit /b 1
)

REM Parse command line arguments
set BUILD_TYPE=debug
if "%1"=="release" set BUILD_TYPE=release
if "%1"=="profile" set BUILD_TYPE=profile

echo Building Limit Order Manager (%BUILD_TYPE% configuration)...

REM Set build-specific flags
if "%BUILD_TYPE%"=="debug" (
    set CXXFLAGS=%CXXFLAGS% -g -O0
    echo Debug build with debug info
) else if "%BUILD_TYPE%"=="release" (
    set CXXFLAGS=%CXXFLAGS% -O3 -DNDEBUG
    echo Release build with optimizations
) else if "%BUILD_TYPE%"=="profile" (
    set CXXFLAGS=%CXXFLAGS% -O2 -g -pg
    echo Profile build with profiling info
)

REM Create object files
echo Compiling source files...
for %%f in (%SOURCES%) do (
    set OBJ_FILE=%%~nf.o
    echo   Compiling %%f...
    %CXX% %CXXFLAGS% %INCLUDES% -c %%f -o !OBJ_FILE!
    if !errorlevel! neq 0 (
        echo Error: Compilation failed for %%f
        pause
        exit /b 1
    )
)

REM Link executable
echo Linking executable...
%CXX% %CXXFLAGS% *.o -o %TARGET%
if %errorlevel% neq 0 (
    echo Error: Linking failed
    pause
    exit /b 1
)

REM Clean up object files
del *.o

echo.
echo Build completed successfully!
echo Executable: %TARGET%
echo Build type: %BUILD_TYPE%
echo.

REM Run basic test if requested
if "%2"=="test" (
    echo Running basic test...
    %TARGET% load data\ticks.txt
    echo.
    echo Test completed!
)

pause 