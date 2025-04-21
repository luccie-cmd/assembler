global main
type main, function
arguments main, 0
section .text
main:
    mov rbx, 3
    mov rax, 10
    ret

; Expected:
; global main
; type main, function
; section .text
; main:
;     mov rax, 10
;     ret