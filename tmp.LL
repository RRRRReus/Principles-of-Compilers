define i32 @main() {
B21431:
  %t21436 = alloca i32, align 4
  store i32 1, i32* %t21436, align 4
  br label %B21439
B21439:                               	; preds = %B21431, %B21437
  %t21440 = load i32, i32* %t21436, align 4
  %t21441 = icmp sle i32 %t21440, 20
  br i1 %t21441, label %B21437, label %B21438
B21437:                               	; preds = %B21439
  call void @putch(i32 102)
  call void @putch(i32 105)
  call void @putch(i32 98)
  call void @putch(i32 40)
  %t21446 = load i32, i32* %t21436, align 4
  call void @putint(i32 %t21446)
  call void @putch(i32 41)
  call void @putch(i32 32)
  call void @putch(i32 61)
  call void @putch(i32 32)
  %t21452 = load i32, i32* %t21436, align 4
  %t21453 = call i32 @fib(i32 %t21452)
  call void @putint(i32 %t21453)
  call void @putch(i32 10)
  %t21456 = load i32, i32* %t21436, align 4
  %t21457 = load i32, i32* %t21436, align 4
  %t8000 = add i32 %t21457, 1
  store i32 %t8000, i32* %t21436, align 4
  br label %B21439
B21438:                               	; preds = %B21439
  ret i32 0
}