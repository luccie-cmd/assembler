global main
type main, function
arguments main, 0
section .text
main:
    mov rax, 3
    mov rbx, 5
    add rax, rbx
    mov rcx, rax
    mov rax, 3
    mov rbx, 5
    add rax, rbx
    mov rdx, rax
    ret

; Expected:
; global main
; type main, function
; section .text
; main:
;     mov rax, 3
;     mov rbx, 5
;     add rax, rbx
;     mov rcx, rax
;     mov rdx, rax
;     ret