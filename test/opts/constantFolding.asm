global main
type main, function
arguments main, 0
section .text
main:
    mov rcx, 5*10+2
    mov rax, [rcx+2*10]
    mov rax, qword [rax+rcx*2+2*10]
    ret

; Expected:
; global main
; type main, function
; section .text
; main:
;     mov rcx, 52
;     mov rax, [rcx+20]
;     mov rax, qword [rax+rcx*2+20]
;     ret