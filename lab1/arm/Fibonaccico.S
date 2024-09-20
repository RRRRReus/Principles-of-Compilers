LC0:
	.ascii	"%d\000"
	.align	2
LC1:
	.ascii	"%d\012\000"
	@.text
	@.align	1
	.global	main   @ 使 main 函数全局可见
	.syntax unified  @ 使用统一的语法
	.thumb
	.thumb_func   @ 指定 Thumb 架构 和上面那句不能同时删？
	@.type	main, %function @ 声明main为一个函数而不是数据
main:
	push	{r7, lr} 		@ 保存使用的寄存器以及链接寄存器LR
	sub	sp, sp, #24		@ 将栈指针 sp 减少 24 字节，为 n 和 t 预留栈空间
	add	r7, sp, #0		@ r7 = sp
	ldr	r2, L6		@从标签 L6 中加载一个值到寄存器 r2 ???????????
LPIC4:
	add	r2, pc
	ldr	r3, L6+4
	ldr	r3, [r2, r3]
	ldr	r3, [r3]
	str	r3, [r7, #20]
	mov	r3, #0
	movs	r3, #0
	str	r3, [r7, #4]
	movs	r3, #1
	str	r3, [r7, #8]
	movs	r3, #1
	str	r3, [r7, #12]
	mov	r3, r7
	mov	r1, r3
	ldr	r3, L6+8
LPIC0:
	add	r3, pc
	mov	r0, r3
	bl	__isoc99_scanf(PLT)
	ldr	r1, [r7, #4]
	ldr	r3, L6+12
LPIC1:
	add	r3, pc
	mov	r0, r3
	bl	printf(PLT)
	ldr	r1, [r7, #8]
	ldr	r3, L6+16
LPIC2:
	add	r3, pc
	mov	r0, r3
	bl	printf(PLT)
	b	L2
L3:
	ldr	r3, [r7, #8]
	str	r3, [r7, #16]
	ldr	r2, [r7, #8]
	ldr	r3, [r7, #4]
	add	r3, r3, r2
	str	r3, [r7, #8]
	ldr	r1, [r7, #8]
	ldr	r3, L6+20
LPIC3:
	add	r3, pc
	mov	r0, r3
	bl	printf(PLT)
	ldr	r3, [r7, #16]
	str	r3, [r7, #4]
	ldr	r3, [r7, #12]
	adds	r3, r3, #1
	str	r3, [r7, #12]
L2:
	ldr	r3, [r7]
	ldr	r2, [r7, #12]
	cmp	r2, r3
	blt	L3
	movs	r3, #0
	ldr	r1, L6+24
LPIC5:
	add	r1, pc
	ldr	r2, L6+4
	ldr	r2, [r1, r2]
	ldr	r1, [r2]
	ldr	r2, [r7, #20]
	eors	r1, r2, r1
	mov	r2, #0
	beq	L5
	bl	__stack_chk_fail(PLT)
L5:
	mov	r0, r3
	adds	r7, r7, #24
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
L7:
	.align	2
L6:
	.word	_GLOBAL_OFFSET_TABLE_-(LPIC4+4)
	.word	__stack_chk_guard(GOT)
	.word	LC0-(LPIC0+4)
	.word	LC1-(LPIC1+4)
	.word	LC1-(LPIC2+4)
	.word	LC1-(LPIC3+4)
	.word	_GLOBAL_OFFSET_TABLE_-(LPIC5+4)	
