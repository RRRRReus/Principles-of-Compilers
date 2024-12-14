target triple = "armv7-unknown-linux-gnueabihf"
define i32 @main() {
B52:
  br label %B61
B61:                               	; preds = %B52, %B66
  %t170 = phi i32 [ 0, %B52 ], [ %t169, %B66 ]
  %t162 = phi i32 [ 0, %B52 ], [ %t50, %B66 ]
  %t157 = phi i32 [ 0, %B52 ], [ %t158, %B66 ]
  %t155 = phi i32 [ 0, %B52 ], [ %t152, %B66 ]
  %t150 = phi i32 [ 0, %B52 ], [ %t149, %B66 ]
  %t141 = phi i32 [ 0, %B52 ], [ %t140, %B66 ]
  %t63 = icmp slt i32 %t162, 20
  br i1 %t63, label %B59, label %B60
B59:                               	; preds = %B61
  br label %B67
B60:                               	; preds = %B61
  ret i32 %t170
B67:                               	; preds = %B59, %B72
  %t169 = phi i32 [ %t170, %B59 ], [ %t168, %B72 ]
  %t161 = phi i32 [ %t162, %B59 ], [ %t163, %B72 ]
  %t158 = phi i32 [ 0, %B59 ], [ %t44, %B72 ]
  %t152 = phi i32 [ %t155, %B59 ], [ %t153, %B72 ]
  %t149 = phi i32 [ %t150, %B59 ], [ %t143, %B72 ]
  %t140 = phi i32 [ %t141, %B59 ], [ %t139, %B72 ]
  %t69 = icmp slt i32 %t158, 10
  br i1 %t69, label %B65, label %B66
B65:                               	; preds = %B67
  br label %B73
B66:                               	; preds = %B67
  %t50 = add i32 %t161, 1
  br label %B61
B73:                               	; preds = %B65, %B121
  %t168 = phi i32 [ %t169, %B65 ], [ %t167, %B121 ]
  %t163 = phi i32 [ %t161, %B65 ], [ %t164, %B121 ]
  %t156 = phi i32 [ %t158, %B65 ], [ %t159, %B121 ]
  %t153 = phi i32 [ 0, %B65 ], [ %t41, %B121 ]
  %t143 = phi i32 [ %t149, %B65 ], [ %t144, %B121 ]
  %t139 = phi i32 [ %t140, %B65 ], [ %t138, %B121 ]
  %t75 = icmp slt i32 %t153, 5
  br i1 %t75, label %B71, label %B72
B71:                               	; preds = %B73
  br label %B79
B72:                               	; preds = %B73
  %t44 = add i32 %t156, 1
  br label %B67
B79:                               	; preds = %B71, %B108
  %t167 = phi i32 [ %t168, %B71 ], [ %t38, %B108 ]
  %t164 = phi i32 [ %t163, %B71 ], [ %t165, %B108 ]
  %t159 = phi i32 [ %t156, %B71 ], [ %t160, %B108 ]
  %t151 = phi i32 [ %t153, %B71 ], [ %t154, %B108 ]
  %t148 = phi i32 [ 0, %B71 ], [ %t35, %B108 ]
  %t138 = phi i32 [ %t139, %B71 ], [ %t137, %B108 ]
  %t81 = icmp slt i32 %t148, 3
  br i1 %t81, label %B77, label %B78
B77:                               	; preds = %B79
  %t14 = add i32 %t148, 1
  %t85 = icmp sge i32 %t14, 3
  %t105 = icmp ne i1 %t85, 0
  br i1 %t105, label %B82, label %B83
B78:                               	; preds = %B79, %B97
  %t144 = phi i32 [ %t148, %B79 ], [ %t148, %B97 ]
  br label %B122
B82:                               	; preds = %B77
  %t104 = icmp ne i32 %t148, 0
  br i1 %t104, label %B86, label %B87
B83:                               	; preds = %B77, %B87
  %t145 = phi i32 [ %t148, %B77 ], [ %t146, %B87 ]
  br label %B109
B122:                               	; preds = %B78
  %t123 = icmp ne i32 1, 0
  br i1 %t123, label %B120, label %B121
B86:                               	; preds = %B82
  %t94 = icmp ne i32 %t148, 0
  %t93 = xor i1 %t94, 1
  %t95 = icmp ne i32 %t148, 0
  %t96 = or i1 %t95, %t93
  %t103 = icmp ne i1 %t96, 0
  br i1 %t103, label %B89, label %B90
B87:                               	; preds = %B82, %B90
  %t146 = phi i32 [ %t148, %B82 ], [ %t147, %B90 ]
  br label %B83
B109:                               	; preds = %B83, %B107
  %t166 = phi i32 [ %t167, %B83 ], [ %t166, %B107 ]
  %t165 = phi i32 [ %t164, %B83 ], [ %t165, %B107 ]
  %t160 = phi i32 [ %t159, %B83 ], [ %t160, %B107 ]
  %t154 = phi i32 [ %t151, %B83 ], [ %t154, %B107 ]
  %t142 = phi i32 [ %t145, %B83 ], [ %t142, %B107 ]
  %t137 = phi i32 [ 0, %B83 ], [ %t29, %B107 ]
  %t111 = icmp slt i32 %t137, 2
  br i1 %t111, label %B107, label %B108
B120:                               	; preds = %B122
  br label %B126
B121:                               	; preds = %B122, %B125
  %t41 = add i32 %t151, 1
  br label %B73
B89:                               	; preds = %B86
  %t100 = sub i32 0, 1
  %t22 = sub i32 %t148, %t100
  %t101 = icmp sge i32 %t22, 3
  %t102 = icmp ne i1 %t101, 0
  br i1 %t102, label %B97, label %B98
B90:                               	; preds = %B86, %B98
  %t147 = phi i32 [ %t148, %B86 ], [ %t148, %B98 ]
  br label %B87
B107:                               	; preds = %B109
  %t29 = add i32 %t137, 1
  br label %B109
B108:                               	; preds = %B109
  %t35 = add i32 %t142, 1
  %t38 = add i32 %t166, 1
  br label %B79
B126:                               	; preds = %B120
  %t127 = icmp ne i32 1, 0
  br i1 %t127, label %B124, label %B125
B97:                               	; preds = %B89
  br label %B78
B98:                               	; preds = %B89
  br label %B90
B124:                               	; preds = %B126
  br label %B125
B125:                               	; preds = %B124, %B126
  br label %B121
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
