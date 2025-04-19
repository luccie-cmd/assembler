global main
type main, function
section .text
main:
    cmp rax, rax
    je .label1
    jmp .label2
.label1:
    mov rbx, 1
    jmp .end
.label2:
    mov rbx, 2
.end:
    ret

; Expected:
; global main
; type main, function
; section .text
; main:
;     cmp rax, rax
;     mov rbx, 1
;     jmp end
;     jmp .label2
; .label2:
;     mov rbx, 2
; .end:
;     ret