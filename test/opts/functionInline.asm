global main
type main, function
type add1, function
section .text
add1:
    inc rax
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