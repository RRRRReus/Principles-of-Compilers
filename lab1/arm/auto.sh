#!/bin/bash

# Step 1: Compile the ARM assembly file
echo "Compiling Fibonacci.S..."
arm-linux-gnueabihf-gcc Fibonacci.S -o Fibonacci.out  #告诉编译器不要使用标准的启动文件和库 -nostartfiles 和 -nostdlib

# Check if compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

echo "Compilation successful."

# Step 2: Run the compiled program using QEMU
echo "Running Fibonacci.out using QEMU..."
qemu-arm -L /usr/arm-linux-gnueabihf ./Fibonacci.out

# Check if QEMU ran the program successfully
if [ $? -ne 0 ]; then
     
    exit 1
fi

echo "Program executed successfully."