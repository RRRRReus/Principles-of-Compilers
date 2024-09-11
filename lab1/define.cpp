#define GREETING "Hello, World!"
#define SQUARE(x) ((x) * (x))

int main() {
    int num1 = 5;
    std::cout << GREETING << std::endl;
    std::cout << "The square of " << num1 << " is: " << SQUARE(num1) << std::endl;
    
    return 0;
}

