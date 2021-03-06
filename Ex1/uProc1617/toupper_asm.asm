global toupper_singlebyte__asm
;global toupper_4atonce_asm
global toupper_4atonce_unsafe_asm

section .data

section .text

toupper_singlebyte__asm:
    push ebp
    mov ebp, esp
    mov esi, [esp+8]   ; argument 1
    ;mov esi, ecx; use fastcall argument is in eax, or sometimes ecx because gcc
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
    pop ebp; restore old frame pointer
    ret

; toupper_4atonce_asm:
;     push ebp
;     mov ebp, esp
;     mov esi, [esp+8]   ; argument 1
;     ;mov esi, ecx; use fastcall argument is in eax
; next_4:
;     mov edi, esi; mov src addr to dest addr for stosd
;     lodsd; load 4 char string (ptr in esi) to eax
;     test eax,eax ; test if al is 0
;     je exit_4 ; end of the string
;     cmp al, 0x61 ; cmp to 'a'
;     jb next_4 ; char is smaller 'a'
;     cmp al, 0x7a ; cmp to 'z'
;     ja next_4 ; char is greater 'z'
;     xor eax, 0x20202020
;     stosd ; store eax to string in edi
;     jmp next_4; is not 0 => jump to next
; exit_4:
;     pop ebp             ; restore old frame pointer
;     ret

toupper_4atonce_unsafe_asm:
    push ebp
    mov ebp, esp
    mov esi, [esp+8]   ; argument 1
next_4_unsafe:
    mov edi, esi; mov src addr to dest addr for stosd
    lodsd; load 4 char string (ptr in esi) to eax
    test eax,eax ; test if eax is 0 (str terminator)
    je exit_4_unsafe ;end of string
    and eax, 0xDFDFDFDF
    stosd
    jmp next_4_unsafe
exit_4_unsafe:
    pop ebp             ; restore old frame pointer
    ret
