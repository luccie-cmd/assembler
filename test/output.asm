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
    mov qword [rdi], 10
    mov dword [rdi+1], 10
    mov word  [rdi+rcx], 10
    mov byte  [rdi+rcx*2], 10
    mov [rdi+rcx*2+10], rax
    mov r8, qword [rdi]
    mov r8d, dword [rdi+1]
    mov r8w, word [rdi+rcx]
    mov r8b, byte [rdi+rcx*2]
    mov rax, [rdi+rcx*2+10]
    mov qword [rdi], r8
    mov dword [rdi+1], r8d
    mov word [rdi+rcx], r8w
    mov byte [rdi+rcx*2], r8b
    mov [rdi+rcx*2+10], r8w
    mov rdi, str0
    call hello
    jmp .local
.local: ; test local labels
    ret

section .rodata
type str0, object
str0: db "Hello", 10, 0,
"Warning: Symbol name '.text' may conflict with section names and confuse linkers or debuggers."