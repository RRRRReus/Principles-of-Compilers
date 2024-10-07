.arch armv7-a
@commsection save global variable without initialization
.comm a,4 @global variables
.comm b,4
.text
.align 2
@rodata section save constant
.section .rodata
.align 2
_str0:
    .ascii "%d%d\0" @\000 is also one representation for `null character`
    .align 2
_str1:
    .ascii "max is: %d\n"
    @text section code
    .text
    .align 2

    .global max
max: @function int max(int a, int b)
    str fp, [sp,#-4]! @ pre-index mode, sp = sp - 4, push fp
    mov fp, sp
    sub sp, sp, #12 @ allocate space for local variable
    str r0, [fp,#-8] @ r0 = [fp,#-8] = a
    str r1, [fp,#-12] @ r1 = [fp,#-12] = b
    cmp r0, r1
    blt .L2
    ldr r0, [fp,#-8]
    b .L3
.L2:
    ldr r0, [fp,#-12]
.L3:
    add sp, fp, #0
    ldr fp, [sp], #4 @ post-index mode, pop fp, sp = sp + 4
    bx lr @ recover sp fp pc
    @ do you know the difference between `bx` and `bl`?
    @ and if max function is non-leaf, what should we do with the `lr` register?
    
.global main
main:
    push {fp, lr}
    add fp, sp, #4
    ldr r2, _bridge @ r2 = &b
    ldr r1, _bridge + 4 @ r1 = &a
    ldr r0, _bridge + 8 @ *r0 = "%d%d\000"
    bl __isoc99_scanf @ scanf("%d%d", &a, &b)
    ldr r3, _bridge + 4 @ r3 = &a
    ldr r0, [r3] @ r0 = a
    ldr r3, _bridge @ r3 = &b
    ldr r1, [r3] @ r1 = b
    bl max
    mov r1, r0 @ r1 = r0
    ldr r0, _bridge + 12 @ *r0 = "max is: %d\0"
    bl printf @ printf("max is: %d", max(a, b));
    mov r0, #0
    pop {fp, pc} @ return 0

_bridge:
    .word b
    .word a
    .word _str0
    .word _str1
 