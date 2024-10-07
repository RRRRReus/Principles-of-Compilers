.global main

.section .data
msg:    .asciz "Hello, World!"

.section .text
main:
    // write syscall
    mov r7, #4          // syscall number for write
    mov r0, #1          // file descriptor 1 (stdout)
    ldr r1, =msg        // pointer to message
    mov r2, #14         // message length
    svc #0              // make syscall

    // exit syscall
    mov r7, #1          // syscall number for exit
    mov r0, #0          // exit code 0
    svc #0              // make syscall