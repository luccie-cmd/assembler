global main
type main, function
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