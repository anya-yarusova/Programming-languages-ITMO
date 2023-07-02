%include "lib.inc"
%include "colon.inc"
%include "words.inc"

%define BUFFER_SIZE 256

section .bss
    buffer: resb BUFFER_SIZE

global _start
extern find_word

section .rodata
    too_long: db "Word too long", 0
    not_found: db "Word not found in dictionary", 0
    found: db "Word found: ", 0
    prompt: db "Enter a word: ", 0 ; prompt for user input

section .text
_start:
.prompt_main:
    mov rdi, prompt ; print prompt
    call print_string
.read_main: ; read a word from stdin
    ;sub rsp, BUFFER_SIZE ; allocate space for the word
    ;mov rdi, rsp ; pass the address of the buffer to read_word
    mov rdi, buffer ; pass the address of the buffer to read_word
    mov rsi, BUFFER_SIZE ; pass the size of the buffer to read_word
    call read_word ; read a word from stdin
    test rax, rax ; check if the word is too long
    je .too_long_main ; if so, print an error message and exit

    ;mov rdi, rsp ; pass the address of the word to find_word
    mov rdi, buffer ; pass the address of the word to find_word
    mov rsi, first ; pass the address of the first word to find_word
    call find_word ; find the word in the dictionary
    ;add rsp, BUFFER_SIZE ; deallocate the buffer
    test rax, rax ; check if the word was found
    je .not_found_main ; if not, print an error message and exit
    jne .found_main ; if so, print a success message and exit
 .too_long_main:
     mov rdi, too_long
     ;add rsp, BUFFER_SIZE ; deallocate the buffer
     jmp .error_end_main
.not_found_main:    
    mov rdi, not_found
    jmp .error_end_main
.found_main:
    push rax ; save the address of the element
    mov rdi, found ; print the success message
    call print_string
    pop rax ; restore the address of the element
    mov rdi, rax
    add rdi, 8 ; skip the first 8 bytes of the element(the next pointer)
    push rdi
    call string_length ; get the length of the key word
    pop rdi
    add rdi, rax ; add the length of the key word to the address of the element
    inc rdi ; skip the null terminator
    call print_string ; print the value
    call print_newline
    xor rdi, rdi ; exit with status 0
    call exit
.error_end_main:
    call print_string_err ; print the error message
    call print_newline
    mov rdi, 1 ; exit with status 1
    call exit

