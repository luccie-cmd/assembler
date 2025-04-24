global PCIreadConfigByte
global PCIreadConfigWord
; global PCIreadConfig
extern usePciOld
extern dbgPrintf
extern pcieSegmentBases
extern abort
type PCIreadConfigByte, function
type PCIreadConfigWord, function
type PCIreadConfig, function
type dbgPrintf, function
type abort, function
type usePciOld, object
type pcieSegmentBases, object
arguments dbgPrintf, 6
arguments abort, 0
arguments PCIreadConfigByte, 5
arguments PCIreadConfigWord, 5
arguments PCIreadConfig, 5
section .text
PCIreadConfigByte:
    call PCIreadConfig
    ; movzx rax, al
    ret

PCIreadConfigWord:
    call PCIreadConfig
    ; movzx rax, ax
    ret

; rdi: segment
; rsi: bus
; rdx: device
; rcx: function
; r8: offset
PCIreadConfig:
    ; movzx rax, byte [usePciOld]
    ; test rax, rax
    ; jz .new
.old:
    mov rdi, str0
    call dbgPrintf
    call abort
.new:
    ; prefetchnta [pcieSegmentBases]
    push r15
    shl rdi, 4
    ; lea r15, pcieSegmentBases
    ; add r15, rdi
    ; shr rdi, 4
    mov r15, [r15+rdi]
    push rsi
    shl rsi, 20
    shl rdx, 15
    shl rcx, 12
    ; or rsi, rdx
    ; or rsi, rcx
    ; shr rcx, 12
    ; shr rdx, 15
    add r15, rsi
    pop rsi
    add r15, r8
    mov rax, [r15]
    pop r15
    ret

section .rodata
type str0, object
str0: db "TODO: Old PCI config reading", 0x0a, 0