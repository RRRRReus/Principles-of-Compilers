    .section .data
fmt_input:  .asciz "%d"            @ 输入格式字符串
fmt_output: .asciz "%d"          @ 输出格式字符串

    .section .text
    .global main                   @ 使 main 函数全局可见
    .syntax unified                @ 使用统一的语法
    .thumb                         @ 使用 Thumb 指令集
    .thumb_func                    @ 指定 Thumb 架构
    .type main, %function          @ 声明 main 为函数类型

main:
    @ 保存使用的寄存器以及链接寄存器 LR
    push {r4, r5, r6, r7, lr}

    @ 初始化栈帧
    sub sp, sp, #12                @ 为 n, t 预留栈空间
    add r7, sp, #0                 @ r7 作为帧指针

    movs r4, #0                    @ a = 0
    movs r5, #1                    @ b = 1
    movs r6, #1                    @ i = 1

    @ 调用 scanf 获取用户输入 n
    ldr r0, =fmt_input             @ 加载输入格式字符串
    mov r1, r7                     @ 将 r7 作为 n 的地址
    bl scanf                       @ 调用 scanf

    @ 输出 a
    mov r0, r4
    ldr r1, =fmt_output            @ 加载输出格式字符串
    bl printf                      @ 调用 printf 输出 a

    @ 输出 b
    mov r0, r5
    bl printf                      @ 调用 printf 输出 b

loop:
    ldr r0, [r7]                   @ 加载 n
    cmp r6, r0                     @ 比较 i 和 n
    bge end                        @ 如果 i >= n 跳转到 end

    mov r0, r5                     @ 保存 b 到 r0
    str r0, [r7, #4]               @ 保存 b 到 t
    add r5, r4, r5                 @ b = a + b
    mov r0, r5
    bl printf                      @ 调用 printf 输出 b

    ldr r4, [r7, #4]               @ a = t
    add r6, r6, #1                 @ i = i + 1
    b loop                         @ 跳转到 loop

end:
    @ 恢复栈空间
    add sp, sp, #12
    @ 恢复寄存器并返回
    pop {r4, r5, r6, r7, lr}
    bx lr                          @ 返回
