global main
type main, function
type func, function
section .text
func:
    ret
main:
    call func
    ret

; Expected:
; global main
; type main, function
; type func, function
; section .text
; func:
;     ret
; main:
;     jmp func