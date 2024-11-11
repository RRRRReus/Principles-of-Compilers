#include <iostream> // 包含输入输出流库
#include <string.h> // 包含字符串处理库
#include <unistd.h> // 包含POSIX操作系统API
#include "common.h" // 包含自定义的common.h头文件
#include "Ast.h" // 包含自定义的Ast.h头文件

extern FILE *yyin; // 声明外部文件指针yyin
extern FILE *yyout; // 声明外部文件指针yyout

int yyparse(); // 声明外部解析函数yyparse

Ast ast; // 创建Ast对象
char outfile[256] = "a.out"; // 定义输出文件名并初始化为"a.out"
dump_type_t dump_type = ASM; // 定义dump类型并初始化为ASM

int main(int argc, char *argv[]) // 主函数，接收命令行参数
{
    int opt; // 定义变量用于存储选项
    while ((opt = getopt(argc, argv, "ato:")) != -1) // 解析命令行选项
    {
        switch (opt) // 根据选项执行相应操作
        {
        case 'o': // 如果选项是'o'
            strcpy(outfile, optarg); // 将输出文件名设置为optarg
            break;
        case 'a': // 如果选项是'a'
            dump_type = AST; // 将dump类型设置为AST
            break;
        case 't': // 如果选项是't'
            dump_type = TOKENS; // 将dump类型设置为TOKENS
            break;
        default: // 如果选项无效
            fprintf(stderr, "Usage: %s [-o outfile] infile\n", argv[0]); // 打印使用说明
            exit(EXIT_FAILURE); // 退出程序
            break;
        }
    }
    if (optind >= argc) // 如果没有输入文件
    {
        fprintf(stderr, "no input file\n"); // 打印错误信息
        exit(EXIT_FAILURE); // 退出程序
    }
    if (!(yyin = fopen(argv[optind], "r"))) // 打开输入文件
    {
        fprintf(stderr, "%s: No such file or directory\nno input file\n", argv[optind]); // 打印错误信息
        exit(EXIT_FAILURE); // 退出程序
    }
    if (!(yyout = fopen(outfile, "w"))) // 打开输出文件
    {
        fprintf(stderr, "%s: fail to open output file\n", outfile); // 打印错误信息
        exit(EXIT_FAILURE); // 退出程序
    }
    yyparse(); // 调用解析函数
    if(dump_type == AST) // 如果dump类型是AST
        ast.output(); // 输出AST
    return 0; // 返回0，表示程序成功结束
}
