	.arch armv8-a
	.arch_extension crc
	.arm
	.global main
	.type main , %function
main:
.L5:
	push {r4, r5, fp, lr}
	mov fp, sp
	sub sp, sp, #40
	mov r4, #-40
	mov r4, #0
	mov r5, #4
	mul r5, r4, r5
	add r4, r4, r5
	add r5, fp, r4
	ldr r4, [r5]
	mov r4, #34
	str r4, [r5]
	mov r4, #-40
	mov r4, #0
	mov r5, #4
	mul r5, r4, r5
	add r4, r4, r5
	add r5, fp, r4
	ldr r4, [r5]
	mov r0, r4
	bl putint
	mov r0, #10
	bl putch
	mov r0, #0
	mov sp, fp
	pop {r4, r5, fp, lr}
	bx lr
