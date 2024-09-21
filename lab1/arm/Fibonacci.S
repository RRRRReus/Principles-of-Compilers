.arch armv7-a
.text

Fib:
    push {lr}          @ 保存返回地址
    cmp r0, #0
    beq ifzero         @ 如果 r0 == 0，返回 0
    cmp r0, #1				
    beq ifone          @ 如果 r0 == 1，返回 1

    push {r0}          @ 保存当前 r0 的值
    sub r0, r0, #1
    bl Fib             @ 第一次递归调用，计算 Fib(n-1)
    mov r1, r0         @ 将 Fib(n-1) 的结果存入 r1
    pop {r0}           @ 恢复原始的 r0

    push {r1}          @ 保存 Fib(n-1) 的结果
    sub r0, r0, #2
    bl Fib             @ 第二次递归调用，计算 Fib(n-2)
    pop {r1}           @ 恢复 Fib(n-1) 的结果
    add r0, r0, r1     @ Fib(n) = Fib(n-1) + Fib(n-2)

    pop {lr}           @ 恢复返回地址
    bx lr              @ 返回

ifzero:
    mov r0, #0         @ 返回 0
    pop {lr}
    bx lr

ifone:
    mov r0, #1         @ 返回 1
    pop {lr}
    bx lr


.globl main
main:
    push {fp, lr}
    add fp, sp, #4          @ 设置新的帧指针
					@ add fp, sp, #4
    sub sp, sp, #8

    ldr r0, =prompt
    bl printf

				@ ldr r0, =debug
				@ bl printf

    ldr r0, =format			
    add r1, sp, #4			@ r1 存储栈顶向下偏移4字节的位置
    bl scanf    			@ 读取用户输入的整数值，并将其存储在寄存器 r1 指向的地址中

    ldr r0, [sp, #4]        @ 将用户输入的 n 加载到 r0
    cmp r0, #0      		@ if r0 <= 0
    bge valid_input			@ 无符号大于或等于

    ldr r0, =error_msg		
    bl printf
    add sp, sp, #8
    pop {fp, lr}
    bx lr


valid_input:
    bl Fib
    mov r2, r0
    ldr r0, =result
    ldr r1, [sp, #4]     @按照寄存器先后顺序输出,第一个格式化字符串是r1,第二个是r2
    bl printf


end:
    add sp, sp, #8
    pop {fp, lr}
	bx lr

.data
prompt:    
	.asciz "需要计算斐波那契数列的第几位:"
format:    
	.asciz "%d"
error_msg: 
	.asciz "输入错误"
result:    
	.asciz "第%d项为:%d\n"
debug:
	.asciz "debug" 