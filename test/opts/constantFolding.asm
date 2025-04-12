global main
type main, function
section .text
main:
    mov rax, 5*10+2
    ret

; Expected:
; global main
; type main, function
; section .text
; main:
;     mov rax, 52
;     ret