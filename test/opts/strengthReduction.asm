global main
type main, function
section .text
main:
    mov rax, 3
    imul rax, 4
    ret

; Expected:
; global main
; type main, function
; section .text
; main:
;     mov rax, 3
;     shl rax, 2
;     ret