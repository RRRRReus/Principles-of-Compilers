.global main

.section .data
a:      .word 0
b:      .word 1
i:      .word 1
n:      .word 0
t:      .word 0
input_buffer: .space 4

.section .bss

.section .text
main:
    // 读取输入的n
    ldr r7, =3          // syscall number for read
    mov r0, #0          // file descriptor 0 (stdin)
    ldr r1, =input_buffer // buffer to store input
    mov r2, #4          // number of bytes to read
    svc #0              // make syscall

    // 将输入转换为整数
    ldr r0, =input_buffer
    bl atoi
    ldr r1, =n
    str r0, [r1]

    // 打印a
    ldr r0, =a
    ldr r1, [r0]
    bl print_num

    // 打印b
    ldr r0, =b
    ldr r1, [r0]
    bl print_num

    // while (i < n)
loop:
    ldr r0, =i
    ldr r1, [r0]
    ldr r2, =n
    ldr r3, [r2]
    cmp r1, r3
    bge end_loop

    // t = b
    ldr r0, =b
    ldr r1, [r0]
    ldr r2, =t
    str r1, [r2]

    // b = a + b
    ldr r0, =a
    ldr r1, [r0]
    ldr r2, =b
    ldr r3, [r2]
    add r1, r1, r3
    str r1, [r2]

    // 打印b
    ldr r0, =b
    ldr r1, [r0]
    bl print_num

    // a = t
    ldr r0, =t
    ldr r1, [r0]
    ldr r2, =a
    str r1, [r2]

    // i = i + 1
    ldr r0, =i
    ldr r1, [r0]
    add r1, r1, #1
    str r1, [r0]

    b loop

end_loop:
    // 退出程序
    mov r7, #1          // syscall number for exit
    mov r0, #0          // exit code 0
    svc #0              // make syscall

print_num:
    // 将数字转换为字符串并打印
    push {r4, r5, r6, lr}
    mov r4, r1          // 保存数字
    ldr r7, =4          // syscall number for write
    mov r0, #1          // file descriptor 1 (stdout)
    ldr r1, =buffer     // buffer to store string
    mov r2, #12         // buffer size
    bl itoa             // 将数字转换为字符串
    svc #0              // make syscall
    pop {r4, r5, r6, lr}
    bx lr

itoa:
    // 将整数转换为字符串
    push {r4, r5, r6, lr}
    mov r5, r0          // 保存原始数字
    mov r6, r1          // 保存字符串缓冲区指针
    add r1, r1, #11     // 移动到缓冲区末尾
    mov r2, #0          // 初始化计数器
itoa_loop:
    mov r0, r5
    bl mod10            // 计算数字的最后一位
    add r0, r0, #48     // 转换为ASCII字符
    strb r0, [r1], #-1  // 存储字符并移动指针
    mov r0, r5
    bl div10            // 计算剩余的数字
    add r2, r2, #1      // 增加计数器
    cmp r0, #0
    bne itoa_loop
    mov r0, r6
    add r1, r1, #1
    sub r2, r2, #1
    add r1, r1, r2
    pop {r4, r5, r6, lr}
    bx lr

mod10:
    // 计算r0 % 10
    mov r1, #10
    bl __aeabi_uidivmod
    mov r0, r1
    bx lr

div10:
    // 计算r0 / 10
    mov r1, #10
    bl __aeabi_uidiv
    bx lr

atoi:
    // 将字符串转换为整数
    push {r4, r5, lr}
    mov r4, r0          // 保存字符串指针
    mov r5, #0          // 初始化结果
    mov r6, #10         // 初始化乘数
atoi_loop:
    ldrb r0, [r4], #1   // 读取下一个字符
    subs r0, r0, #48    // 转换为数字
    blt atoi_done       // 如果字符不是数字，跳转到结束
    mul r5, r5, r6      // 结果乘以10
    add r5, r5, r0      // 加上当前数字
    b atoi_loop
atoi_done:
    mov r0, r5          // 返回结果
    pop {r4, r5, r6, lr}
    bx lr

.section .data
buffer: .space 12