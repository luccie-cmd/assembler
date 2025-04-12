global main
type main, function
section .text
main:
    mov rax, 3
    add rax, 5
    ret

; Expected:
; global main
; type main, function
; section .text
; main:
;     mov rax, 8
;     ret