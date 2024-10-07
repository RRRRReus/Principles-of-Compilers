 .arch armv7-a

  @数据段
  .data
.align 1
pi: .float 3.14
D: .float 0.0
four: .float 4.0
length_format: .asciz "周长为：%.2f"
input_format: .asciz "%f"

@代码段
    .text
.align 1
.global main

main:
    @保存返回地址
    push {fp, lr}   

    @输入D的值
    ldr r0, =input_format  
    ldr r1, =D              
    bl scanf               @ 读取用户输入

    @加载pi和D的值
    ldr r0, =pi
    ldr r1, =D
    vldr.32 s0, [r0]           @将pi加载到s0
    vldr.32 s1, [r1]            @将D加载到s1

    @计算周长
    vmul.f32 s2, s0, s1            @存储结果到s2

    @输出周长
    ldr r0, =length_format       
    vcvt.f64.f32 d7, s2         
    vmov r2, r3, d7
    bl printf

    mov r0, #0
    pop {fp, pc}
