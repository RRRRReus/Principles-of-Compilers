target triple = "armv7-unknown-linux-gnueabihf"
@ggg = global i32 9, align 4
@kk = global i32 6, align 4
define i32 @f() {
B24:
  %t30 = alloca i32, align 4
  %t29 = alloca i32, align 4
  %t27 = alloca i32, align 4
  store i32 0, i32* %t27, align 4
  %t31 = load i32, i32* @kk, align 4
  store i32 5, i32* @kk, align 4
  %t32 = load i32, i32* %t30, align 4
  %t33 = load i32, i32* @kk, align 4
  store i32 %t33, i32* %t30, align 4
  %t34 = load i32, i32* %t30, align 4
  store i32 %t34, i32* %t27, align 4
  br label %B25
B25:                               	; preds = %B24
  %t28 = load i32, i32* %t27, align 4
  ret i32 %t28
}
define i32 @main() {
B35:
  %t43 = alloca i32, align 4
  %t42 = alloca i32, align 4
  %t41 = alloca i32, align 4
  %t40 = alloca i32, align 4
  %t38 = alloca i32, align 4
  store i32 0, i32* %t38, align 4
  %t44 = load i32, i32* %t40, align 4
  store i32 10, i32* %t40, align 4
  %t45 = load i32, i32* %t41, align 4
  store i32 4, i32* %t41, align 4
  %t46 = load i32, i32* %t42, align 4
  store i32 2, i32* %t42, align 4
  %t50 = load i32, i32* %t42, align 4
  %t51 = icmp sgt i32 %t50, 3
  %t54 = icmp ne i1 %t51, 0
  br i1 %t54, label %B47, label %B48
B47:                               	; preds = %B35
  %t52 = load i32, i32* %t42, align 4
  store i32 6, i32* %t42, align 4
  br label %B49
B48:                               	; preds = %B35
  %t53 = load i32, i32* %t42, align 4
  store i32 7, i32* %t42, align 4
  br label %B49
B49:                               	; preds = %B47, %B48
  %t55 = load i32, i32* %t43, align 4
  %t56 = load i32, i32* %t41, align 4
  %t57 = load i32, i32* %t42, align 4
  %t22 = add i32 %t56, %t57
  store i32 %t22, i32* %t43, align 4
  %t58 = call i32 @f()
  store i32 %t58, i32* %t38, align 4
  br label %B36
B36:                               	; preds = %B49
  %t39 = load i32, i32* %t38, align 4
  ret i32 %t39
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
