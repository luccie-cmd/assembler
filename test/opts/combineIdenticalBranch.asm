global main
type main, function
section .text
main:
    cmp rax, 5
    je label1
    mov rbx, 0
    jmp end
label1:
    mov rbx, 0
end:
    ret

; Expected:
; global main
; type main, function
; section .text
; main:
;     cmp rax, 5
;     mov rbx, 0
;     ret