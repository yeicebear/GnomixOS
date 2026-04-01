bits 64

/* gdt table with null, code, data descriptors */
section .data
    align 8
    gdt_table:
        dq 0x0000000000000000
        dq 0x00209a0000000000
        dq 0x0000920000000000
    gdt_end:

    gdt_descriptor:
        dw gdt_end - gdt_table - 1
        dq gdt_table

section .text
    global setup_gdt
    global flush_gdt

setup_gdt:
    lgdt [rel gdt_descriptor]
    ret

flush_gdt:
    mov rax, 0x10
    mov ds, rax
    mov es, rax
    mov fs, rax
    mov gs, rax
    mov ss, rax
    pop rdi
    mov rsi, 0x8
    push rsi
    push rdi
    retfq
