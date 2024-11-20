echo "生成自己的中间代码"

make run

echo "生成自己的中间代码结束，开始生成clang的中间代码"

clang -target arm-linux-gnueabihf -mcpu=cortex-a8 -mfloat-abi=hard -x c -S -emit-llvm example.sy -o exampleOK.ll

echo "自己的运行结果"

./run.sh example.ll

echo "clang的运行结果"

./run.sh exampleOK.ll