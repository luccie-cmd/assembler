global main
type main, function
arguments main, 0
section .text
main:
    mov [rbx], 5
    mov [rbx], 10
    ret

; Expected:
; global main
; type main, function
; section .text
; main:
;     mov [rbx], 10
;     ret