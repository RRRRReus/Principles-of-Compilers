target triple = "armv7-unknown-linux-gnueabihf"
@s = global i32 0, align 4
define i32 @get_ans_se(i32 %t184, i32 %t186, i32 %t188) {
B179:
  %t195 = icmp eq i32 %t186, %t188
  %t197 = icmp ne i1 %t195, 0
  br i1 %t197, label %B191, label %B192
B191:                               	; preds = %B179
  br label %B192
B192:                               	; preds = %B179, %B191
  %t362 = phi i32 [ 0, %B179 ] ,[ 1, %B191 ] 
  %t11 = mul i32 %t184, 2
  %t15 = add i32 %t11, %t362
  %t204 = load i32, i32* @s, align 4
  %t19 = add i32 %t204, %t15
  store i32 %t19, i32* @s, align 4
  ret i32 %t15
}
define i32 @get_ans(i32 %t212, i32 %t214, i32 %t216) {
B207:
  %t223 = icmp eq i32 %t214, %t216
  %t225 = icmp ne i1 %t223, 0
  br i1 %t225, label %B219, label %B220
B219:                               	; preds = %B207
  br label %B220
B220:                               	; preds = %B207, %B219
  %t365 = phi i32 [ 0, %B207 ] ,[ 1, %B219 ] 
  %t31 = mul i32 %t212, 2
  %t35 = add i32 %t31, %t365
  ret i32 %t35
}
define i32 @main() {
B232:
  %t39 = add i32 -2147483648, 1
  %t42 = sub i32 2147483647, 1
  %t249 = call i32 @get_ans(i32 0, i32 -2147483648, i32 -2147483648)
  %t55 = add i32 -2147483648, 1
  %t254 = call i32 @get_ans(i32 %t249, i32 %t55, i32 %t39)
  %t259 = sub i32 0, 2147483647
  %t62 = sub i32 %t259, 1
  %t260 = call i32 @get_ans(i32 %t254, i32 -2147483648, i32 %t62)
  %t68 = add i32 %t42, 1
  %t265 = call i32 @get_ans(i32 %t260, i32 -2147483648, i32 %t68)
  %t73 = sdiv i32 -2147483648, 2
  %t75 = sdiv i32 %t39, 2
  %t270 = call i32 @get_ans(i32 %t265, i32 %t73, i32 %t75)
  %t275 = sub i32 0, 2147483647
  %t81 = sub i32 %t275, 1
  %t276 = call i32 @get_ans(i32 %t270, i32 -2147483648, i32 %t81)
  %t87 = add i32 %t42, 1
  %t281 = call i32 @get_ans(i32 %t276, i32 -2147483648, i32 %t87)
  %t285 = call i32 @get_ans(i32 0, i32 %t39, i32 2147483647)
  %t290 = call i32 @get_ans(i32 %t285, i32 %t39, i32 %t42)
  %t295 = call i32 @get_ans(i32 %t290, i32 2147483647, i32 %t42)
  %t106 = sdiv i32 -2147483648, 2
  %t108 = sdiv i32 -2147483648, 2
  %t300 = call i32 @get_ans(i32 %t295, i32 %t106, i32 %t108)
  %t304 = call i32 @get_ans_se(i32 0, i32 -2147483648, i32 -2147483648)
  %t117 = add i32 -2147483648, 1
  %t309 = call i32 @get_ans_se(i32 %t304, i32 %t117, i32 %t39)
  %t314 = sub i32 0, 2147483647
  %t124 = sub i32 %t314, 1
  %t315 = call i32 @get_ans_se(i32 %t309, i32 -2147483648, i32 %t124)
  %t130 = add i32 %t42, 1
  %t320 = call i32 @get_ans_se(i32 %t315, i32 -2147483648, i32 %t130)
  %t135 = sdiv i32 -2147483648, 2
  %t137 = sdiv i32 %t39, 2
  %t325 = call i32 @get_ans_se(i32 %t320, i32 %t135, i32 %t137)
  %t330 = sub i32 0, 2147483647
  %t143 = sub i32 %t330, 1
  %t331 = call i32 @get_ans_se(i32 %t325, i32 -2147483648, i32 %t143)
  %t149 = add i32 %t42, 1
  %t336 = call i32 @get_ans_se(i32 %t331, i32 -2147483648, i32 %t149)
  %t340 = call i32 @get_ans_se(i32 0, i32 %t39, i32 2147483647)
  %t345 = call i32 @get_ans_se(i32 %t340, i32 %t39, i32 %t42)
  %t350 = call i32 @get_ans_se(i32 %t345, i32 2147483647, i32 %t42)
  %t168 = sdiv i32 -2147483648, 2
  %t170 = sdiv i32 -2147483648, 2
  %t355 = call i32 @get_ans_se(i32 %t350, i32 %t168, i32 %t170)
  %t174 = add i32 %t281, %t300
  %t176 = add i32 %t174, %t336
  %t178 = add i32 %t176, %t355
  ret i32 %t178
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
