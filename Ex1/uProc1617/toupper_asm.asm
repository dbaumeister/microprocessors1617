global toupper_singlebyte__asm
global toupper_4atonce_asm

section .data

section .text

toupper_singlebyte__asm:
    push ebp
    mov ebp, esp
    mov esi, [esp+8]   ; argument 1
    ;mov esi, ecx; use fastcall argument is in eax, or sometimes ecx because gcc
    ;push eax
next_s:
    mov edi, esi; mov src addr to dest addr for stosd
    lodsb ; load one char
    test al, al ; test if al is 0
    je exit_s ; end of the string
    cmp al, 0x61 ; cmp to 'a'
    jb next_s ; char is smaller 'a'
    cmp al, 0x7a ; cmp to 'z'
    ja next_s ; char is greater 'z'
    xor al, 0x20
    stosb; store byte
    jmp next_s; is not 0 => jump to next
exit_s:
    pop ebp             ; restore old frame pointer
    ;pop eax;
    ret

toupper_4atonce_asm:
    push ebp
    mov ebp, esp
    mov esi, [esp+8]   ; argument 1
    ;mov esi, ecx; use fastcall argument is in eax
    ;push eax
next_4:
    mov edi, esi; mov src addr to dest addr for stosd
    lodsd; load 4 char string (ptr in esi) to eax
    test al, al ; test if al is 0
    je exit_4 ; end of the string
    cmp al, 0x61 ; cmp to 'a'
    jb next_4 ; char is smaller 'a'
    cmp al, 0x7a ; cmp to 'z'
    ja next_4 ; char is greater 'z'
    xor eax, 0x20202020
    stosd ; store eax to string in edi
    jmp next_4; is not 0 => jump to next
exit_4:
    pop ebp             ; restore old frame pointer
    ;pop eax
    ret