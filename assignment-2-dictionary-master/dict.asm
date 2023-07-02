%include "lib.inc"

global find_word

section .text

; rdi - pointer to the string(key value), rsi - pointer to the first element of the dictionary
; rax - pointer to the value, or 0 if not found
find_word:
test rsi, rsi ; if rsi == 0
jz .not_found_find_word ; jump to .not_found
.loop_find_word:
    push rdi ; save key pointer
    push rsi ; save dictionary pointer
    add rsi, 8 ; move to key value
    call string_equals ; compare key with the current key
    pop rsi ; restore dictionary pointer
    pop rdi ; restore key pointer
    test rax, rax ; if rax == 0
    jnz .found_find_word ; jump to .found
    mov rsi, [rsi] ; get next element
    test rsi, rsi ; if rsi == 0
    jz .not_found_find_word ; jump to .not_found
    jnz .loop_find_word ; jump to .loop
.found_find_word:
    mov rax, rsi ; rax = pointer to the value
    ret
.not_found_find_word:
    xor rax, rax ; rax = 0
    ret    
