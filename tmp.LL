target triple = "armv7-unknown-linux-gnueabihf"
@A = global [100 x i32] zeroinitializer, align 4
@B = global [100 x i32] zeroinitializer, align 4
@C = global [100 x i32] zeroinitializer, align 4
@D = global [100 x i32] zeroinitializer, align 4
@p = constant i32 234145, align 4
define void @f(i32* %t171, i32* %t173, i32* %t175, i32* %t177, i32 %t179) {
B169:
  %t178 = alloca i32*, align 4
  %t176 = alloca i32*, align 4
  %t174 = alloca i32*, align 4
  %t172 = alloca i32*, align 4
  store i32* %t171, i32** %t172, align 4
  store i32* %t173, i32** %t174, align 4
  store i32* %t175, i32** %t176, align 4
  store i32* %t177, i32** %t178, align 4
  %t184 = icmp eq i32 %t179, 0
  %t185 = icmp ne i1 %t184, 0
  br i1 %t185, label %B181, label %B182
B181:                               	; preds = %B169
  br label %B170
B182:                               	; preds = %B169
  br label %B189
B170:                               	; preds = %B181, %B188
  ret void
B189:                               	; preds = %B182, %B187
  %t452 = phi i32 [ 0, %B182 ], [ %t65, %B187 ]
  %t191 = icmp slt i32 %t452, 100
  br i1 %t191, label %B187, label %B188
B187:                               	; preds = %B189
  %t193 = load i32*, i32** %t172, align 4
  %t197 = load i32*, i32** %t176, align 4
  %t201 = load i32*, i32** %t174, align 4
  %t205 = load i32*, i32** %t178, align 4
  %t209 = load i32*, i32** %t174, align 4
  %t213 = load i32*, i32** %t172, align 4
  %t217 = load i32*, i32** %t176, align 4
  %t221 = load i32*, i32** %t178, align 4
  %t225 = load i32*, i32** %t176, align 4
  %t229 = load i32*, i32** %t174, align 4
  %t233 = load i32*, i32** %t172, align 4
  %t237 = load i32*, i32** %t178, align 4
  %t241 = load i32*, i32** %t172, align 4
  %t245 = load i32*, i32** %t172, align 4
  %t250 = load i32*, i32** %t174, align 4
  %t254 = load i32*, i32** %t174, align 4
  %t259 = load i32*, i32** %t176, align 4
  %t263 = load i32*, i32** %t176, align 4
  %t65 = add i32 %t452, 1
  br label %B189
B188:                               	; preds = %B189
  %t269 = load i32*, i32** %t174, align 4
  %t270 = load i32*, i32** %t176, align 4
  %t271 = load i32*, i32** %t172, align 4
  %t272 = load i32*, i32** %t178, align 4
  %t71 = sub i32 %t179, 1
  call void @f(i32* %t269, i32* %t270, i32* %t271, i32* %t272, i32 %t71)
  br label %B170
}
define i32 @sum(i32* %t280) {
B275:
  %t281 = alloca i32*, align 4
  store i32* %t280, i32** %t281, align 4
  br label %B286
B286:                               	; preds = %B275, %B284
  %t456 = phi i32 [ 0, %B275 ], [ %t85, %B284 ]
  %t455 = phi i32 [ 0, %B275 ], [ %t82, %B284 ]
  %t288 = icmp slt i32 %t456, 100
  br i1 %t288, label %B284, label %B285
B284:                               	; preds = %B286
  %t292 = load i32*, i32** %t281, align 4
  %t293 = getelementptr inbounds i32,i32* %t292, i32 %t456
  %t294 = load i32, i32* %t293, align 4
  %t82 = add i32 %t455, %t294
  %t85 = add i32 %t456, 1
  br label %B286
B285:                               	; preds = %B286
  ret i32 %t455
}
define i32 @main() {
B298:
  br label %B306
B306:                               	; preds = %B298, %B304
  %t457 = phi i32 [ 0, %B298 ], [ %t111, %B304 ]
  %t308 = icmp slt i32 %t457, 100
  br i1 %t308, label %B304, label %B305
B304:                               	; preds = %B306
  %t312 = getelementptr inbounds [100 x i32],[100 x i32]* @A, i32 0, i32 %t457
  %t313 = load i32, i32* %t312, align 4
  %t93 = sdiv i32 %t457, 4
  %t94 = add i32 1, %t93
  store i32 %t94, i32* %t312, align 4
  %t318 = getelementptr inbounds [100 x i32],[100 x i32]* @B, i32 0, i32 %t457
  %t319 = load i32, i32* %t318, align 4
  %t98 = sdiv i32 %t457, 3
  %t99 = add i32 2, %t98
  store i32 %t99, i32* %t318, align 4
  %t324 = getelementptr inbounds [100 x i32],[100 x i32]* @C, i32 0, i32 %t457
  %t325 = load i32, i32* %t324, align 4
  %t103 = sdiv i32 %t457, 2
  %t104 = add i32 3, %t103
  store i32 %t104, i32* %t324, align 4
  %t330 = getelementptr inbounds [100 x i32],[100 x i32]* @D, i32 0, i32 %t457
  %t331 = load i32, i32* %t330, align 4
  %t108 = add i32 4, %t457
  store i32 %t108, i32* %t330, align 4
  %t111 = add i32 %t457, 1
  br label %B306
B305:                               	; preds = %B306
  %t340 = getelementptr inbounds [100 x i32],[100 x i32]* @D, i32 0, i32 0
  %t341 = load i32, i32* %t340, align 4
  %t342 = call i32 @sum(i32* %t340)
  %t117 = add i32 0, %t342
  %t345 = getelementptr inbounds [100 x i32],[100 x i32]* @A, i32 0, i32 0
  %t346 = load i32, i32* %t345, align 4
  %t349 = getelementptr inbounds [100 x i32],[100 x i32]* @B, i32 0, i32 0
  %t350 = load i32, i32* %t349, align 4
  %t353 = getelementptr inbounds [100 x i32],[100 x i32]* @C, i32 0, i32 0
  %t354 = load i32, i32* %t353, align 4
  %t357 = getelementptr inbounds [100 x i32],[100 x i32]* @D, i32 0, i32 0
  %t358 = load i32, i32* %t357, align 4
  call void @f(i32* %t345, i32* %t349, i32* %t353, i32* %t357, i32 10)
  %t364 = getelementptr inbounds [100 x i32],[100 x i32]* @D, i32 0, i32 0
  %t365 = load i32, i32* %t364, align 4
  %t366 = call i32 @sum(i32* %t364)
  %t127 = add i32 %t117, %t366
  %t369 = getelementptr inbounds [100 x i32],[100 x i32]* @A, i32 0, i32 0
  %t370 = load i32, i32* %t369, align 4
  %t373 = getelementptr inbounds [100 x i32],[100 x i32]* @A, i32 0, i32 0
  %t374 = load i32, i32* %t373, align 4
  %t377 = getelementptr inbounds [100 x i32],[100 x i32]* @A, i32 0, i32 0
  %t378 = load i32, i32* %t377, align 4
  %t381 = getelementptr inbounds [100 x i32],[100 x i32]* @D, i32 0, i32 0
  %t382 = load i32, i32* %t381, align 4
  call void @f(i32* %t369, i32* %t373, i32* %t377, i32* %t381, i32 10)
  %t388 = getelementptr inbounds [100 x i32],[100 x i32]* @D, i32 0, i32 0
  %t389 = load i32, i32* %t388, align 4
  %t390 = call i32 @sum(i32* %t388)
  %t137 = add i32 %t127, %t390
  %t395 = getelementptr inbounds [100 x i32],[100 x i32]* @A, i32 0, i32 0
  %t396 = load i32, i32* %t395, align 4
  %t397 = call i32 @sum(i32* %t395)
  %t142 = add i32 %t137, %t397
  %t400 = getelementptr inbounds [100 x i32],[100 x i32]* @D, i32 0, i32 0
  %t401 = load i32, i32* %t400, align 4
  %t404 = getelementptr inbounds [100 x i32],[100 x i32]* @D, i32 0, i32 0
  %t405 = load i32, i32* %t404, align 4
  %t408 = getelementptr inbounds [100 x i32],[100 x i32]* @D, i32 0, i32 0
  %t409 = load i32, i32* %t408, align 4
  %t412 = getelementptr inbounds [100 x i32],[100 x i32]* @A, i32 0, i32 0
  %t413 = load i32, i32* %t412, align 4
  call void @f(i32* %t400, i32* %t404, i32* %t408, i32* %t412, i32 10)
  %t419 = getelementptr inbounds [100 x i32],[100 x i32]* @A, i32 0, i32 0
  %t420 = load i32, i32* %t419, align 4
  %t421 = call i32 @sum(i32* %t419)
  %t152 = add i32 %t142, %t421
  %t426 = getelementptr inbounds [100 x i32],[100 x i32]* @D, i32 0, i32 0
  %t427 = load i32, i32* %t426, align 4
  %t428 = call i32 @sum(i32* %t426)
  %t157 = add i32 %t152, %t428
  call void @putint(i32 %t157)
  call void @putch(i32 10)
  %t434 = getelementptr inbounds [100 x i32],[100 x i32]* @A, i32 0, i32 0
  %t435 = load i32, i32* %t434, align 4
  call void @putarray(i32 100, i32* %t434)
  %t439 = getelementptr inbounds [100 x i32],[100 x i32]* @B, i32 0, i32 0
  %t440 = load i32, i32* %t439, align 4
  call void @putarray(i32 100, i32* %t439)
  %t444 = getelementptr inbounds [100 x i32],[100 x i32]* @C, i32 0, i32 0
  %t445 = load i32, i32* %t444, align 4
  call void @putarray(i32 100, i32* %t444)
  %t449 = getelementptr inbounds [100 x i32],[100 x i32]* @D, i32 0, i32 0
  %t450 = load i32, i32* %t449, align 4
  call void @putarray(i32 100, i32* %t449)
  ret i32 0
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
