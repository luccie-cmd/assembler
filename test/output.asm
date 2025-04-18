; test globals
global main
type main, function
arguments main, 4

; .text {
;   declare function, @hello(i64 %rdi);
;   define function, @main(i64 %rdi, i64 %rsi, i64 %rdx, i64 %rcx) {
;       __begin__:
;           %0 = i64 0
;           alias %rax, %0
;           %1 = i64 0
;           alias %rdi, %1
;           store %1, i64 10
;           %2 = add i64 %1, i64 1
;           store %2, i32 10
;           %3 = add i64 %1, i64 %rcx
;           store %3, i16 10
;           %4 = imul i64 %rcx, i64 2
;           %5 = add i64 %4, i64 %1
;           store %5, i8 10
;           %6 = imul i64 %rcx, i64 2
;           %7 = add i64 %6, i64 %1
;           %8 = add i64 %7, i32 10
;           store %8, i64 %0
;           %9 = load i64 %1
;           %10 = add i64 %1, 1
;           %11 = load i32 %10
;           %12 = add i64 %1, i64 %rcx
;           %13 = load i16 %12
;           %14 = imul i64 %rcx, i64 2
;           %15 = add i64 %14, i64 %1
;           %16 = load i16 %15
;           %17 = imul i64 %rcx, i64 2
;           %18 = add i64 %17, i64 %1
;           %19 = add i64 %18, i64 10
;           %20 = load i16 %19
;           store %1, i64 %9
;           %21 = add i64 %1, i64 1
;           store %21, i32 %11
;           %22 = add i64 %1, i64 %rcx
;           store %22, i16 %13
;           %23 = imul i64 %rcx, i64 2
;           %24 = add i64 %23, i64 %1
;           store %24, i8 %16
;           %25 = ptr @str0
;       .local2:
;           %26 = call @hello (%25 as i64)
;           br .local
;       .local:
;           ret i64 %26
;   }
; };
; .rodata {
;   define object, @str0;
;   value @str0 = "Hello\0a\00Warning: Symbol name '.text' may conflict with section names and confuse linkers or debuggers.";
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