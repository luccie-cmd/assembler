global main
type main, function
arguments main, 1
type add1, function
arguments add1, 1
section .text
add1:
    inc rdi
    mov rax, rdi
    ret
main:
    call add1
    ret

; Expected:
; global main
; type main, function
; section .text
; main:
;     inc rax
;     ret