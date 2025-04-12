; test globals
global main
type main, function
; test externs
extern hello
type hello, function
section .text
main:
    mov rax, 0
    xor rdi, rdi
    mov [rdi], 10
    mov [rdi+1], 10
    mov [rdi+rcx], 10
    mov [rdi+rcx*2], 10
    mov [rdi+rcx*2+10], 10
    mov [rdi+rcx*2+10*2], 10
    mov rdi, str0
    call hello
    jmp .local
.local: ; test local labels
    ret

section .rodata
type str0, object
str0: db "Hello", 10, 0,
"Warning: Symbol name '.text' may conflict with section names and confuse linkers or debuggers."