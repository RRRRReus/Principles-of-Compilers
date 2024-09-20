.arch armv7-a
.global main
.text

    .data
num_a:    
    .asciz "a="             @ 提示用户输入 a
num_b:    
    .asciz "b="             @ 提示用户输入 b
result_msg: 
    .asciz "平方和为: %lf" @ 输出平方和结果
format_double:
    .asciz "%lf"            @ 双精度浮点数格式化符

    .bss                    @ 定义全局变量 a 和 b，使用 8 字节空间
    .align 3
a:  .space 8                @ double a (64-bit)
b:  .space 8                @ double b (64-bit)

    .text
main:
    push {fp, lr}           @ 保存帧指针和链接寄存器
    add fp, sp, #4          @ 设置新的帧指针

    sub sp, sp, #16         @ 为局部变量分配栈空间

    @ 输入 a
    ldr r0, =num_a       @ 加载提示字符串地址
    bl printf               @ 调用 printf
    ldr r0, =format_double  @ 加载格式化字符串地址
    ldr r1, =a              @ 将全局变量 a 的地址加载到 r1
    bl scanf                @ 读取 a 的值

    @ 输入 b
    ldr r0, =num_b       @ 加载提示字符串地址
    bl printf               @ 调用 printf
    ldr r0, =format_double  @ 加载格式化字符串地址
    ldr r1, =b              @ 将全局变量 b 的地址加载到 r1
    bl scanf                @ 读取 b 的值

    @ 计算平方和: c = a*a + b*b
    vldr.f64 d0, a          @ 加载 a 到浮点寄存器 d0
    vldr.f64 d1, b          @ 加载 b 到浮点寄存器 d1
    vmul.f64 d2, d0, d0     @ 计算 a * a 并存储在 d2
    vmul.f64 d3, d1, d1     @ 计算 b * b 并存储在 d3
    vadd.f64 d4, d2, d3     @ 计算 a*a + b*b 并存储在 d4

    @ 输出结果
    ldr r0, =result_msg     @ 加载结果提示字符串地址
    vmov r1, d4[0]          @ 将 c 的低32位移动到 r1
    vmov r2, d4[1]          @ 将 c 的高32位移动到 r2
    bl printf               @ 输出结果

    @ 函数返回
    add sp, sp, #16         @ 恢复栈指针
    pop {fp, lr}            @ 恢复帧指针和返回地址
    bx lr                   @ 返回

