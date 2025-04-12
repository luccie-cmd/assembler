global main
type main, function
section .text
main:
    jmp .used
.unused:
    mov rax, 42
    ret
.used:
    mov rax, 1
    ret

; Expected:
; global main
; type main, function
; section .text
; main:
;     jmp .used
; .used:
;     mov rax, 1
;     ret