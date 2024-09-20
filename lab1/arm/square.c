#include<stdio.h>

double a=0;
double b=0;

int main()
{
    printf("a=");
    scanf("%lf", &a);
    printf("b=");
    scanf("%lf", &b);

    double c = a * a + b * b;
    printf("平方和为: %lf\n", c);

}