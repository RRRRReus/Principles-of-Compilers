#!/bin/bash

# 编译汇编文件，使用 vfpv3 作为 FPU 选项，采用硬件浮点支持
arm-linux-gnueabihf-gcc -mfpu=vfpv3 -o circle.out circle.S

# 检查编译是否成功
if [ $? -eq 0 ]; then
    echo "编译成功！生成的可执行文件为 'circle.out'"
else
    echo "编译失败！请检查代码和配置。"
    exit 1
fi

# 使用 QEMU 执行生成的可执行文件
echo "正在运行 'circle.out'..."
qemu-arm -L /usr/arm-linux-gnueabihf ./circle.out

