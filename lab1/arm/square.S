.arch armv7-a
.global main
.text

    .data
num_a:    
    .asciz "a="             @ 提示用户输入 a
num_b:    
    .asciz "b="             @ 提示用户输入 b
result_msg: 
    .asciz "平方和为: %lf"   @ 输出平方和结果
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
    ldr r0, =num_a          @ 加载提示字符串地址
    bl printf               @ 调用 printf
    ldr r0, =format_double  @ 加载格式化字符串地址
    ldr r1, =a              @ 将全局变量 a 的地址加载到 r1
    bl scanf                @ 读取 a 的值

    @ 输入 b
    ldr r0, =num_b          @ 加载提示字符串地址
    bl printf               @ 调用 printf
    ldr r0, =format_double  @ 加载格式化字符串地址
    ldr r1, =b              @ 将全局变量 b 的地址加载到 r1
    bl scanf                @ 读取 b 的值

    @ 输出 a 和 b 的值进行验证
    ldr r0, =format_double
    ldr r1, =a
    vldr.f64 d0, [r1]       @ 从内存中加载 a 的值到浮点寄存器 d0
    sub sp, sp, #8
    vstr.f64 d0, [sp]
    mov r1, sp
    bl printf
    add sp, sp, #8

    ldr r0, =format_double
    ldr r1, =b
    vldr.f64 d1, [r1]       @ 从内存中加载 b 的值到浮点寄存器 d1
    sub sp, sp, #8
    vstr.f64 d1, [sp]
    mov r1, sp
    bl printf
    add sp, sp, #8

    @ 计算平方和: c = a*a + b*b
    vmul.f64 d2, d0, d0     @ 计算 a * a 并存储在 d2
    vmul.f64 d3, d1, d1     @ 计算 b * b 并存储在 d3
    vadd.f64 d4, d2, d3     @ 计算 a*a + b*b 并存储在 d4

    @ 输出 d2 和 d3 的值进行验证
    ldr r0, =format_double
    sub sp, sp, #8
    vstr.f64 d2, [sp]
    mov r1, sp
    bl printf
    add sp, sp, #8

    ldr r0, =format_double
    sub sp, sp, #8
    vstr.f64 d3, [sp]
    mov r1, sp
    bl printf
    add sp, sp, #8

    @ 输出结果
    ldr r0, =result_msg     @ 加载结果提示字符串地址
    bl printf               @ 输出提示
    ldr r0, =format_double  @ 加载浮点数格式化字符串
    sub sp, sp, #8          @ 为浮点数参数分配栈空间
    vstr.f64 d4, [sp]       @ 将 d4 中的值存储到栈中
    mov r1, sp              @ 将栈指针传递给 r1 作为参数
    bl printf               @ 调用 printf 输出 d4 的值
    add sp, sp, #8          @ 恢复栈指针

    @ 函数返回
    add sp, sp, #16         @ 恢复栈指针
    pop {fp, lr}            @ 恢复帧指针和返回地址
    bx lr                   @ 返回
