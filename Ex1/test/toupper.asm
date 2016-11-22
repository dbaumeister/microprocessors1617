global toupper_asm

section .data

section .text

toupper_asm:
    ;mov ebp, esp; for correct debugging
    push ebp
    ;mov esi, [esp+4]   ; argument 1
    mov esi, eax; use fastcall argument is in eax
next:
    mov edi, esi; mov src addr to dest addr for stosd
    lodsd; load string (ptr in esi) to eax
    ;mov ecx, [eax] ; load 4 chars from ptr
    test al, al ; test if al is 0
    je exit
    xor eax, 0x20202020
    stosd ; store eax to string in edi
    ;add esi, 4 ; advance t
    jmp next; is not 0 => jump to next
exit:
    ;mov esp,ebp         ; free space for locals
    pop ebp             ; restore old frame pointer
    ret