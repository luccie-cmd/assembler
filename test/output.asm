; test globals
global main
type main, function
arguments main, 4

; .text {
;   declare function, hello;
;   define function, main(i64 %rdi, i64 %rsi, i64 %rdx, i64 %rcx) {
;       __begin__:
;           %0 = i64 0
;           alias %rax, %0
;           %1 = i64 0
;           alias %rdi, %1
;           store i64 %1, i64 10
;           %2 = add %1, i64 1
;           store i64 %2, i32 10
;           %3 = add %1, i64 %rcx
;           store i64 %3, i16 10
;           %4 = imul i64 %rcx, i64 2
;           %5 = add i64 %4, i64 %1
;           store i64 %5, i8 10
;           %6 = imul i64 %rcx, i64 2
;           %7 = add i64 %6, i64 %1
;           %8 = add i64 %7, i32 10
;           store i64 %8, i64 %0
;           ret i64 %0
;   }
; };
; .rodata {
;   define object, str0;
;   value str0 = "Hello\0a\00Warning: Symbol name '.text' may conflict with section names and confuse linkers or debuggers.";
; }
; test externs
extern hello
type hello, function
arguments hello, 1
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
.local2:
    call hello
    jmp .local
.local: ; test local labels
    ret

section .rodata
type str0, object
str0: db "Hello", 10, 0,
"Warning: Symbol name '.text' may conflict with section names and confuse linkers or debuggers."