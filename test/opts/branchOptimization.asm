global main
type main, function
arguments main, 0
section .text
main:
    jmp .label1
.label1:
    jmp .label2
.label2:
    mov rax, 1
    ret

; Expected:
; global main
; type main, function
; section .text
; main:
;     mov rax, 1
;     ret