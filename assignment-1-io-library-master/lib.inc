section .text

; Принимает код возврата и завершает текущий процесс
exit: 
    mov  rax, 60
    syscall

; Принимает указатель на нуль-терминированную строку, возвращает её длину
string_length:
    xor rax, rax
    .count:
      cmp byte [rdi+rax], 0
      je .end
      inc rax
      jmp .count
    .end:
      ret

; Принимает указатель на нуль-терминированную строку, выводит её в stdout
print_string:
    push rdi
    call string_length
    pop rsi
    mov  rdx, rax
    mov  rax, 1
    mov  rdi, 1
    syscall
    ret

; Принимает код символа и выводит его в stdout
print_char:
    push rdi
    mov  rdx, 1
    mov  rax, 1
    mov  rdi, 1
    mov rsi, rsp
    syscall
    pop rdi
    ret

; Переводит строку (выводит символ с кодом 0xA)
print_newline:
    mov rdi, '\n'
    jmp print_char

; Выводит беззнаковое 8-байтовое число в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII коды.
print_uint:
    mov rax, rdi
    dec rsp
    mov byte[rsp], 0x0
    mov rcx, 10
    mov rsi, 1
    .loop:
        xor rdx, rdx
        div rcx
        add rdx, '0'
        dec rsp
        mov [rsp], dl
        inc rsi
        test rax, rax
        jz .end
        jmp .loop
    .end:
        mov rdi, rsp
        push rsi
        call print_string
        pop rsi
        add rsp, rsi
        ret

; Выводит знаковое 8-байтовое число в десятичном формате 
print_int:
    xor rax, rax
    test rdi, rdi
    jns print_uint
    neg rdi
    push rdi
    mov rdi, '-'
    call print_char
    pop rdi
    jmp print_uint

; Принимает два указателя на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
; rdi, rsi - указатели на сравниваемые строки, rdx - счётчик текущего символа
string_equals:
    xor rax, rax
    xor rdx, rdx
    .loop: 
        mov al, [rsi + rdx]
        mov cl, [rdi + rdx]
        inc rdx
        cmp al, cl
        jne .not_equal
        cmp al, 0
        je .equal
        jmp .loop
    .equal:
        mov rax, 1
        ret
    .not_equal:
        mov rax, 0
        ret        
    ret

; Читает один символ из stdin и возвращает его. Возвращает 0 если достигнут конец потока
read_char:
   xor rax, rax
    xor rdi, rdi
    dec rsp
    mov rsi, rsp
    mov rdx, 1
    syscall
    test rax, rax
    jz .eof
    mov al, [rsp]
    jmp .end
    .eof:
        xor rax, rax
    .end:
        inc rsp
    ret
    
; Принимает: адрес начала буфера, размер буфера
; Читает в буфер слово из stdin, пропуская пробельные символы в начале, .
; Пробельные символы это пробел 0x20, табуляция 0x9 и перевод строки 0xA.
; Останавливается и возвращает 0 если слово слишком большое для буфера
; При успехе возвращает адрес буфера в rax, длину слова в rdx.
; При неудаче возвращает 0 в rax
; Эта функция должна дописывать к слову нуль-терминатор
; rsi - размер буфера, rdi - адрес начала буфера, rdx - счётчик длины слова
read_word:
    xor rdx, rdx
    xor rax, rax
    .loop:
        push rsi
        push rdi
        push rdx
        call read_char
        pop rdx
        pop rdi
        pop rsi

        cmp rax, 0
        je .end
        cmp rax, ' '
        je .space
        cmp rax, '\n'
        je .space
        cmp rax, `\t`
        je .space

        mov  byte[rdi + rdx], al
        inc rdx
        cmp rdx, rsi 
        jge .overflow
        jmp .loop
    .space:
        cmp rdx, 0
        jne .end
        jmp .loop
    .overflow:
        xor rax, rax
        xor rdx, rdx
        ret
    .end:
        mov byte[rdi + rdx], 0
        mov rax, rdi
        ret

; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax: число, rdx : его длину в символах
; rdx = 0 если число прочитать не удалось
parse_uint:
    xor rax, rax
    xor rcx, rcx
    xor r8, r8
    mov r9, 10
    .loop:
        mov r8b, byte[rdi + rcx]
        cmp r8b, '0'
        jb .end
        cmp r8b, '9'
        ja .end
        sub r8b, '0'
        inc rcx
        mul r9
        add rax, r8
        jmp .loop
    .end:
        mov rdx, rcx
        ret


; Принимает указатель на строку, пытается
; прочитать из её начала знаковое число.
; Если есть знак, пробелы между ним и числом не разрешены.
; Возвращает в rax: число, rdx : его длину в символах (включая знак, если он был) 
; rdx = 0 если число прочитать не удалось
parse_int:
    cmp byte[rdi], '-'
    je .negative
    cmp byte[rdi], '+'
    je .positive
    jmp parse_uint
    .positive:
        inc rdi
        call parse_uint
        inc rdx
        ret
    .negative:
        inc rdi
        call parse_uint
        inc rdx
        neg rax
        ret 

; Принимает указатель на строку, указатель на буфер и длину буфера
; rdi - указатель на строку, rsi - указатель на буфер, rdx - длина буфера
; Копирует строку в буфер
; Возвращает длину строки если она умещается в буфер, иначе 0
string_copy:
    xor rax, rax
    .loop:
        cmp rdx, rax
        js .overflow
        mov r8b, [rdi + rax]
        mov [rsi + rax], r8b
        cmp r8b, 0
        je .end
        inc rax
        jmp .loop
    .overflow:
        mov rax, 0
        ret
    .end:
        ret