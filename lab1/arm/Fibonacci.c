#include<stdio.h>

int Fib(int n)
{
    if(n == 0)
    {
        return 0;
    }
    else if(n == 1)
    {
        return 1;
    }
    else
    {
        return Fib(n - 1) + Fib(n - 2);
    }

}

int main()
{
    int n;
    printf("需要计算斐波那契数列的第几位:");
    scanf("%d", &n);
    if(n < 0)
    {
        printf("输入错误\n");
        return 0;
    }
    printf("第%d项为:%d\n", n,Fib(n));
    return 0;
}