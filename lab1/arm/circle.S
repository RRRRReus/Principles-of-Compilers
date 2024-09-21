 .arch armv7-a


  .data
pi: .float 3.14
d: .float 0.0
length_format: .asciz "周长为：%.2f"
input_format: .asciz "%f"


    .text
.global main

main:
    push {fp, lr}   

    @输入D的值
    ldr r0, =input_format  
    ldr r1, =d              
    bl scanf               @ 读取用户输入

    @加载pi和d的值
    ldr r0, =pi
    ldr r1, =d
    vldr.32 s0, [r0]           @ 将pi加载到s0
    vldr.32 s1, [r1]            @ 将D加载到s1

    @计算周长
    vmul.f32 s2, s0, s1            @存储结果到s2

    @输出
    ldr r0, =length_format       
    vcvt.f64.f32 d7, s2            @转为双精度浮点数,与printf兼容
    vmov r2, r3, d7                @将双精度浮点数存储到r2和r3两个寄存器
    bl printf

    mov r0, #0
    pop {fp, pc}
