#include <stdio.h>

float pi = 3.14;
float D = 0.0; // 输入的直径

int main() {

    scanf("%f", &D);// 输入D

    float length = pi * D;
    printf("周长为：%.2f\n", length);

    return 0;
}
