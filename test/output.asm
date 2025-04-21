; .text {
;     2 function @hello(i64 %rdi);
;     1 function @main(i64 %rdi, i64 %rsi, i64 %rdx, i64 %rcx){
;         main:
;             %0 = const i64 0
;             %1 = const i64 0
;             store i64 %1, i64 10
;             %2 = add i64 1, i64 %1
;             store i32 %2, i64 10
;             %3 = imul i64 %rcx, i64 1
;             %4 = add i64 %1, i64 %3
;             store i16 %4, i64 10
;             %5 = imul i64 %rcx, i64 2
;             %6 = add i64 %1, i64 %5
;             store i8 %6, i64 10
;             %7 = imul i64 %rcx, i64 2
;             %8 = add i64 %1, i64 %7
;             %9 = add i64 10, i64 %8
;             store i64 %9, i64 %0
;             %10 = load i64 %1
;             %11 = add i64 1, i64 %10
;             %12 = load i32 %11
;             %13 = imul i64 %rcx, i64 1
;             %14 = add i64 %1, i64 %13
;             %15 = load i16 %14
;             %16 = imul i64 %rcx, i64 2
;             %17 = add i64 %1, i64 %16
;             %18 = load i8 %17
;             %19 = imul i64 %rcx, i64 2
;             %20 = add i64 %1, i64 %19
;             %21 = add i64 10, i64 %20
;             %22 = load i64 %21
;             store i64 %1, i64 %10
;             %23 = add i64 1, i64 %22
;             store i32 %23, i32 %12
;             %24 = imul i64 %rcx, i64 1
;             %25 = add i64 %1, i64 %24
;             store i16 %25, i16 %15
;             %26 = imul i64 %rcx, i64 2
;             %27 = add i64 %1, i64 %26
;             store i8 %27, i8 %18
;             %28 = const ptr @str0
;             br label @main.local2
;         main.local2:
;             %29 = call label @hello
;             br label @main.local
;         main.local:
;             ret i64 %29
;     }
; }
; .rodata {
; }
; test globals
global main
type main, function
arguments main, 4
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