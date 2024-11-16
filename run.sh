#!/bin/bash

# 确保传入了参数
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <file.ll>"
    exit 1
fi

# 定义变量
INPUT_FILE="$1"
OUTPUT_FILE="output"
EXECUTABLE="hello244"

# 清理上次生成的文件
rm -f "$OUTPUT_FILE" "$EXECUTABLE"

# 生成目标文件
clang --target=arm-linux-gnueabihf -mcpu=cortex-a8 -mfloat-abi=hard -o "$EXECUTABLE" "$INPUT_FILE" -lm

# 检查是否成功生成了可执行文件
if [ ! -f "$EXECUTABLE" ]; then
    echo "Error: Failed to generate executable."
    exit 1
fi

# 运行可执行文件
export LD_LIBRARY_PATH=/usr/arm-linux-gnueabihf/lib

# 运行 QEMU
qemu-arm -L /usr/arm-linux-gnueabihf/ ./"$EXECUTABLE"


# 获取返回值
retval=$?

# 输出返回值
echo "Program exited with return value: $retval"
 
# 清理生成的文件
rm -f "$EXECUTABLE"
