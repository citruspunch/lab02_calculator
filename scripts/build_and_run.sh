#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Build output directory
BUILD_DIR="build"

# Create build directory if it doesn't exist
mkdir -p $BUILD_DIR

# Remove previous compiled objects and binaries
echo "Cleaning up previous build files..."
rm -f $BUILD_DIR/*.o $BUILD_DIR/*.elf $BUILD_DIR/*.bin

echo "Assembling root.s..."
arm-none-eabi-as -o $BUILD_DIR/root.o drivers/root.s

echo "Compiling os.c (OS Level)..."
arm-none-eabi-gcc -c -msoft-float -o $BUILD_DIR/os.o drivers/os.c -I drivers

echo "Compiling stdio.c (Language Library Level)..."
arm-none-eabi-gcc -c -msoft-float -o $BUILD_DIR/stdio.o lib/stdio.c -I lib -I drivers

echo "Compiling string.c (Language Library Level)..."
arm-none-eabi-gcc -c -o $BUILD_DIR/string.o lib/string.c -I lib

echo "Compiling main.c (User Level)..."
arm-none-eabi-gcc -c -msoft-float -o $BUILD_DIR/main.o app/main.c -I lib -I drivers

echo "Linking object files..."
arm-none-eabi-gcc -T scripts/linker.ld -nostdlib -msoft-float -o $BUILD_DIR/calculadora.elf \
    $BUILD_DIR/root.o $BUILD_DIR/os.o $BUILD_DIR/stdio.o $BUILD_DIR/string.o $BUILD_DIR/main.o -lgcc

echo "Converting ELF to binary..."
arm-none-eabi-objcopy -O binary $BUILD_DIR/calculadora.elf $BUILD_DIR/calculadora.bin

echo "Running QEMU..."
qemu-system-arm -M versatilepb -nographic -kernel $BUILD_DIR/calculadora.elf
