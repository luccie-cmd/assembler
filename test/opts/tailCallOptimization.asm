global main
type main, function
arguments main, 0
type func, function
arguments func, 0
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