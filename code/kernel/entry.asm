[BITS 32]
global _start
extern kmain

section .text
_start:
    call kmain
.halt_loop:
    hlt
    jmp .halt_loop
