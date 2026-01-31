#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Remove previous compiled objects and binaries
echo "Cleaning up previous build files..."
rm -f root.o main.o os.o stdio.o string.o calculadora.elf calculadora.bin

echo "Assembling root.s..."
arm-none-eabi-as -o root.o drivers/root.s

echo "Compiling os.c (OS Level)..."
arm-none-eabi-gcc -c -o os.o drivers/os.c -I drivers

echo "Compiling stdio.c (Language Library Level)..."
arm-none-eabi-gcc -c -o stdio.o lib/stdio.c -I lib -I drivers

echo "Compiling string.c (Language Library Level)..."
arm-none-eabi-gcc -c -o string.o lib/string.c -I lib

echo "Compiling main.c (User Level)..."
arm-none-eabi-gcc -c -o main.o app/main.c -I lib -I drivers

echo "Linking object files..."
arm-none-eabi-ld -T linker.ld -o calculadora.elf root.o os.o stdio.o string.o main.o

echo "Converting ELF to binary..."
arm-none-eabi-objcopy -O binary calculadora.elf calculadora.bin

echo "Running QEMU..."
qemu-system-arm -M versatilepb -nographic -kernel calculadora.elf
