global main
type main, function
section .text
main:
    mov rax, 5*10+2
    mov rax, [rax+2*10]
    mov rax, qword [rax+rcx*2+2*10]
    ret

; Expected:
; global main
; type main, function
; section .text
; main:
;     mov rax, 52
;     ret