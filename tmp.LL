target triple = "armv7-unknown-linux-gnueabihf"
define i32 @main() {
B56:
  %t66 = alloca i32, align 4
  %t65 = alloca i32, align 4
  %t64 = alloca i32, align 4
  %t63 = alloca i32, align 4
  %t62 = alloca i32, align 4
  %t61 = alloca i32, align 4
  %t59 = alloca i32, align 4
  store i32 0, i32* %t59, align 4
  store i32 1, i32* %t61, align 4
  store i32 2, i32* %t62, align 4
  store i32 3, i32* %t63, align 4
  store i32 4, i32* %t64, align 4
  store i32 5, i32* %t65, align 4
  store i32 6, i32* %t66, align 4
  %t70 = load i32, i32* %t61, align 4
  %t71 = load i32, i32* %t62, align 4
  %t8 = mul i32 %t70, %t71
  %t72 = load i32, i32* %t63, align 4
  %t10 = add i32 %t8, %t72
  %t73 = icmp slt i32 %t10, 6
  %t74 = load i32, i32* %t64, align 4
  %t75 = icmp ne i32 %t74, 0
  %t76 = and i1 %t73, %t75
  %t149 = icmp ne i1 %t76, 0
  br i1 %t149, label %B67, label %B68
B67:                               	; preds = %B56
  %t80 = load i32, i32* %t65, align 4
  %t81 = load i32, i32* %t61, align 4
  %t83 = icmp ne i32 %t81, 0
  %t82 = xor i1 %t83, 1
  %t84 = zext i1 %t82 to i32
  %t18 = add i32 %t84, 0
  %t85 = icmp ne i32 %t80, 0
  %t86 = icmp ne i32 %t18, 0
  %t87 = or i1 %t85, %t86
  %t148 = icmp ne i1 %t87, 0
  br i1 %t148, label %B77, label %B78
B68:                               	; preds = %B56
  store i32 1, i32* %t59, align 4
  br label %B57
  br label %B69
B77:                               	; preds = %B67
  %t91 = load i32, i32* %t63, align 4
  %t92 = icmp eq i32 %t91, 2
  %t93 = load i32, i32* %t64, align 4
  %t94 = load i32, i32* %t65, align 4
  %t24 = add i32 %t93, %t94
  %t95 = icmp sgt i32 %t24, 2
  %t96 = and i1 %t92, %t95
  %t147 = icmp ne i1 %t96, 0
  br i1 %t147, label %B88, label %B89
B78:                               	; preds = %B67
  store i32 2, i32* %t59, align 4
  br label %B57
  br label %B79
B69:                               	; preds = %B67, %B68
B57:                               	; preds = %B88, %B97, %B113, %B135, %B136, %B129, %B123, %B107, %B78, %B68
  %t60 = load i32, i32* %t59, align 4
  ret i32 %t60
B88:                               	; preds = %B77
  store i32 3, i32* %t59, align 4
  br label %B57
  br label %B90
B89:                               	; preds = %B77
  %t100 = load i32, i32* %t66, align 4
  %t101 = load i32, i32* %t63, align 4
  %t29 = srem i32 %t100, %t101
  %t102 = load i32, i32* %t65, align 4
  %t103 = icmp ne i32 %t29, 0
  %t104 = icmp ne i32 %t102, 0
  %t105 = and i1 %t103, %t104
  %t146 = icmp ne i1 %t105, 0
  br i1 %t146, label %B97, label %B98
B79:                               	; preds = %B77, %B78
  br label %B69
B90:                               	; preds = %B88, %B89
  br label %B79
B97:                               	; preds = %B89
  store i32 4, i32* %t59, align 4
  br label %B57
  br label %B99
B98:                               	; preds = %B89
  %t109 = load i32, i32* %t64, align 4
  %t110 = load i32, i32* %t62, align 4
  %t34 = sdiv i32 %t109, %t110
  %t111 = load i32, i32* %t61, align 4
  %t36 = add i32 %t34, %t111
  %t112 = icmp sge i32 %t36, 2
  %t145 = icmp ne i1 %t112, 0
  br i1 %t145, label %B106, label %B107
B99:                               	; preds = %B97, %B98
  br label %B90
B106:                               	; preds = %B98
  %t116 = load i32, i32* %t65, align 4
  %t117 = load i32, i32* %t66, align 4
  %t40 = sub i32 %t116, %t117
  %t118 = icmp sge i32 %t40, 0
  %t119 = load i32, i32* %t64, align 4
  %t120 = icmp sgt i32 %t119, 4
  %t121 = or i1 %t118, %t120
  %t144 = icmp ne i1 %t121, 0
  br i1 %t144, label %B113, label %B114
B107:                               	; preds = %B98
  store i32 5, i32* %t59, align 4
  br label %B57
  br label %B108
B113:                               	; preds = %B106
  store i32 6, i32* %t59, align 4
  br label %B57
  br label %B115
B114:                               	; preds = %B106
  %t125 = load i32, i32* %t63, align 4
  %t126 = load i32, i32* %t66, align 4
  %t127 = icmp ne i32 %t125, %t126
  %t143 = icmp ne i1 %t127, 0
  br i1 %t143, label %B122, label %B123
B108:                               	; preds = %B106, %B107
  br label %B99
B115:                               	; preds = %B113, %B114
  br label %B108
B122:                               	; preds = %B114
  %t131 = load i32, i32* %t62, align 4
  %t132 = load i32, i32* %t65, align 4
  %t133 = load i32, i32* %t64, align 4
  %t51 = mul i32 %t132, %t133
  %t52 = add i32 %t131, %t51
  %t134 = icmp sgt i32 %t52, 10
  %t142 = icmp ne i1 %t134, 0
  br i1 %t142, label %B128, label %B129
B123:                               	; preds = %B114
  store i32 7, i32* %t59, align 4
  br label %B57
  br label %B124
B128:                               	; preds = %B122
  %t138 = load i32, i32* %t66, align 4
  %t140 = icmp ne i32 %t138, 0
  %t139 = xor i1 %t140, 1
  %t141 = icmp ne i1 %t139, 0
  br i1 %t141, label %B135, label %B136
B129:                               	; preds = %B122
  store i32 8, i32* %t59, align 4
  br label %B57
  br label %B130
B124:                               	; preds = %B122, %B123
  br label %B115
B135:                               	; preds = %B128
  store i32 9, i32* %t59, align 4
  br label %B57
  br label %B137
B136:                               	; preds = %B128
  store i32 10, i32* %t59, align 4
  br label %B57
  br label %B137
B130:                               	; preds = %B128, %B129
  br label %B124
B137:                               	; preds = %B135, %B136
  br label %B130
}
declare i32 @getint()
declare void @putint(i32)
declare void @putch(i32)
declare void @putarray(i32, i32*)
declare i32 @getfarray(float*)
declare void @putfarray(i32, float*)
declare i32 @getch()
declare void @putfloat(float)
declare float @getfloat()
