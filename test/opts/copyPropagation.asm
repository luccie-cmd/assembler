global main
type main, function
arguments main, 0
section .text
main:
    mov rbx, 5
    mov rcx, rbx
    mov rax, rcx
    ret

; Expected:
; global main
; type main, function
; section .text
; main:
;     mov rbx, 5
;     mov rax, rbx
;     ret