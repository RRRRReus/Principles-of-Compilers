make run

clang -target arm-linux-gnueabihf -mcpu=cortex-a8 -mfloat-abi=hard -x c++ -S -emit-llvm example.sy -o exampleOK.ll

echo "自己的"

./run.sh example.ll

echo "正确的"

./run.sh exampleOK.ll