#!/bin/bash

echo "生成自己的中间代码"
make runi

echo "生成自己的中间代码完成，开始生成clang的中间代码"
clang -target arm-linux-gnueabihf -mcpu=cortex-a8 -mfloat-abi=hard -x c -S -emit-llvm example.sy -o exampleOK.ll



echo "生成自己的汇编代码"
make run

echo "生成自己的汇编代码完成，开始生成clang的汇编代码"

# 使用 clang 生成 ARM 汇编代码
clang -target arm-linux-gnueabihf -mcpu=cortex-a8 -mfloat-abi=hard -x c -S example.sy -o exampleOK.s #-fno-pic
echo "开始运行生成的汇编代码"



# 使用 run.sh 运行自己的汇编代码
echo "运行自己的汇编代码:"
./run.sh example.s

# 使用 run.sh 运行 clang 的汇编代码
echo "运行clang生成的汇编代码:"
./run.sh exampleOK.s 


