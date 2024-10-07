#!/bin/bash

# Step 1: Compile the ARM assembly file
echo "Compiling Fibonacci.S..."
arm-linux-gnueabihf-gcc Fibonacci.S -o Fibonacci.out  

# Check if compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

echo "Compilation successful."

# Step 2: Run the compiled program using QEMU
echo "Running Fibonacci.out using QEMU..."
qemu-arm -L /usr/arm-linux-gnueabihf ./Fibonacci.out


echo "Program executed successfully."