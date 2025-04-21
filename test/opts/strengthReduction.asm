global main
type main, function
arguments main, 0
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