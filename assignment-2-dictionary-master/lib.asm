
global exit
global string_length
global print_string
global print_string_err
global print_char
global print_newline
global print_uint
global print_int
global string_equals
global read_char
global read_word
global parse_int
global parse_uint
global string_copy

section .text



; Принимает код возврата и завершает текущий процесс
exit: 
    mov  rax, 60
    syscall

; Принимает указатель на нуль-терминированную строку, возвращает её длину
string_length:
    xor rax, rax
    .loop_string_length:
        cmp byte [rdi + rax], 0 ; Сравниваем текущий байт с нулем
        je .end_string_length ; Если равны, то выходим
        inc rax ; Если не равен, то увеличиваем счётчик
        jmp .loop_string_length ; И переходим к следующему байту
    .end_string_length:
        ret

; Принимает указатель на нуль-терминированную строку, выводит её в stdout
print_string:
    push rdi
    call string_length ; Вычисляем длину строки
    pop rsi
    mov  rdx, rax ; Передаём длину строки в rdx
    mov  rax, 1 ; Вызываем write в stdout
    mov  rdi, 1
    syscall
    ret

; Принимает указатель на нуль-терминированную строку, выводит её в stderr
print_string_err:
    push rdi
    call string_length ; Вычисляем длину строки
    pop rsi
    mov  rdx, rax ; Передаём длину строки в rdx
    mov  rax, 1 ; Вызываем write в stderr
    mov  rdi, 2
    syscall
    ret

; Принимает код символа и выводит его в stdout
print_char:
    push rdi ; Сохраняем код символа
    mov  rdx, 1 ; Длина строки равна 1
    mov  rax, 1 ; Вызываем write в stdout
    mov  rdi, 1
    mov rsi, rsp ; Выводим символ из вершины стека(код символа, который сохранили ранее)
    syscall
    pop rdi ; Освобождаем место в стеке
    ret

; Переводит строку (выводит символ с кодом 0xA)
print_newline:
    mov rdi, 0xA ; Код символа перевода строки
    call print_char ; Выводим символ
    ret

; Выводит беззнаковое 8-байтовое число в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII коды.
print_uint:
    mov rax, rdi ; Сохраняем число
    dec rsp ; Выделяем место в стеке
    mov byte[rsp], 0x0 ; Записываем нуль-терминатор
    mov rcx, 10 ; Делитель
    mov rsi, 1 ; Счётчик
    .loop_print_uint: ; Цикл деления
        xor rdx, rdx
        div rcx ; Делим rax на rcx
        add rdx, '0' ; Переводим цифру в ASCII код
        dec rsp ; Выделяем место в стеке
        mov [rsp], dl ; Записываем цифру в стек
        inc rsi ; Увеличиваем счётчик
        test rax, rax ; Проверяем, не равно ли rax нулю
        jz .end_print_uint ; Если равно, то выходим из цикла
        jmp .loop_print_uint ; Иначе продолжаем деление
    .end_print_uint:
        mov rdi, rsp
        push rsi
        call print_string  ; Выводим строку
        pop rsi
        add rsp, rsi ; Освобождаем место в стеке
        ret

; Выводит знаковое 8-байтовое число в десятичном формате 
print_int:
    xor rax, rax 
    test rdi, rdi ; Проверяем, не равно ли rdi нулю
    jns .positive ; Если больше, то переходим к выводу положительного числа
    jmp .negative ; Иначе переходим к выводу отрицательного числа
    .negative:
        neg rdi ; Делаем число положительным
        push rdi ; Сохраняем число
        mov rdi, '-' ; Передаём в rdi минус
        call print_char ; Выводим минус
        pop rdi ; Восстанавливаем число
        call print_uint ; Выводим число
        ret
    .positive:
        call print_uint ; Выводим число
        ret

; Принимает два указателя на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
; rdi, rsi - указатели на сравниваемые строки, rdx - счётчик текущего символа
string_equals:
    xor rax, rax
    xor rdx, rdx
    .loop_string_equals: ; Цикл сравнения
        mov al, [rsi + rdx] ; Загружаем текущий символ первой строки в al
        mov cl, [rdi + rdx] ; Загружаем текущий символ второй строки в cl
        inc rdx ; Увеличиваем счётчик текущего символа
        cmp al, cl ; Сравниваем текущие символы
        jne .not_equal ; Если они не равны, то переходим к выводу 0
        cmp al, 0 ; Сравниваем текущий символ с нулём
        je .equal ; Если они равны(дошли до конца строк), то переходим к выводу 1
        jmp .loop_string_equals ; Иначе продолжаем сравнение
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
    dec rsp ; Выделяем место в стеке
    mov rsi, rsp ; Передаём указатель на выделенное место в rsi
    mov rdx, 1  ; Передаём длину одного символа(= 1) в rdx
    syscall ; Вызываем системный вызов read
    test rax, rax ; Проверяем, не равно ли rax нулю
    jz .eof_read_char ; Если равно, то переходим к выводу 0
    mov al, [rsp] ; Загружаем прочитанный символ в al
    jmp .end_read_char
    .eof_read_char:
        xor rax, rax
    .end_read_char:
        inc rsp ; Освобождаем место в стеке
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
    .loop_read_word: ; Цикл пропуска пробельных символов
        push rsi
        push rdi
        push rdx
        call read_char ; Вызываем функцию чтения символа
        pop rdx
        pop rdi
        pop rsi

        cmp rax, 0 ; Сравниваем rax с концом строки
        je .end_read_word ; Если они равны, то переходим к завершению функции
        cmp rax, 0x20 ; Сравниваем rax с пробелом
        je .space_read_word ; Если они равны, то переходим к проверке на начало строки
        cmp rax, 0xA ; Сравниваем rax с переводом строки
        je .space_read_word ; Если они равны, то переходим к проверке на начало строки
        cmp rax, 0x9 ; Сравниваем rax с табуляцией
        je .space_read_word ; Если они равны, то переходим к проверке на начало строки

        mov  byte[rdi + rdx], al ; Записываем прочитанный символ в буфер
        inc rdx ; Увеличиваем счётчик длины слова
        cmp rdx, rsi ; Сравниваем счётчик длины слова с размером буфера
        jge .overflow_read_word ; Если счётчик больше или равен размеру буфера, то переходим к завершению функции
        jmp .loop_read_word ; Переходим к началу цикла
    .space_read_word:
        cmp rdx, 0 ; Сравниваем счётчик длины слова с нулём
        jne .end_read_word ; Если счётчик не равен нулю, то переходим к завершению функции
        jmp .loop_read_word ; Переходим к началу цикла
    .overflow_read_word:
        xor rax, rax
        xor rdx, rdx
        ret
    .end_read_word:
        mov byte[rdi + rdx], 0 ; Записываем нуль-терминатор в буфер
        mov rax, rdi ; Записываем адрес начала буфера в rax
        ret

; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax: число, rdx : его длину в символах
; rdx = 0 если число прочитать не удалось
parse_uint:
    xor rax, rax 
    xor rcx, rcx
    xor r8, r8
    mov r9, 10 ; Десятичная система счисления
    .loop_parse_uint: ; Цикл чтения числа
        mov r8b, byte[rdi + rcx]  ; Считываем очередной символ
        cmp r8b, '0' ; Сравниваем его с нулём
        jb .end_parse_uint ; Если он меньше, то переходим к завершению функции
        cmp r8b, '9' ; Сравниваем его с девяткой
        ja .end_parse_uint ; Если он больше, то переходим к завершению функции
        sub r8b, '0' ; Вычитаем из него нуль
        inc rcx ; Увеличиваем счётчик прочитанных символов
        mul r9 ; Умножаем rax на 10
        add rax, r8 ; Добавляем к rax очередной символ
        jmp .loop_parse_uint ; Переходим к следующей цифре
    .end_parse_uint:
        mov rdx, rcx ; Записываем длину числа в rdx
        ret

; Принимает указатель на строку, пытается
; прочитать из её начала знаковое число.
; Если есть знак, пробелы между ним и числом не разрешены.
; Возвращает в rax: число, rdx : его длину в символах (включая знак, если он был) 
; rdx = 0 если число прочитать не удалось
parse_int:
    cmp byte[rdi], '-' ; Сравниваем первый символ с минусом
    je .negative_parse_int ; Если он равен минусу, то переходим к чтению отрицательного числа
    cmp byte[rdi], '+' ; Сравниваем первый символ с плюсом
    je .positive_parse_int ; Если он равен плюсу, то переходим к чтению положительного числа
    jmp .unsigned_parse_int ; Если ни один из символов не равен, то переходим к чтению беззнакового числа
    .unsigned_parse_int:
        call parse_uint ; Читаем беззнаковое число
        ret
    .positive_parse_int:
        inc rdi ; Увеличиваем указатель на строку на 1
        call parse_uint ; Читаем беззнаковое число
        inc rdx ; Увеличиваем длину числа на 1
        ret
    .negative_parse_int:
        inc rdi ; Увеличиваем указатель на строку на 1
        call parse_uint ; Читаем беззнаковое число
        inc rdx ; Увеличиваем длину числа на 1
        neg rax ; Инвертируем число
        ret 

; Принимает указатель на строку, указатель на буфер и длину буфера
; rdi - указатель на строку, rsi - указатель на буфер, rdx - длина буфера
; Копирует строку в буфер
; Возвращает длину строки если она умещается в буфер, иначе 0
string_copy:
    xor rax, rax
    push rdx
    push rsi
    push rdi
    call string_length ; Вычисляем длину строки
    pop rdi
    pop rsi
    pop rdx
    inc rax ; Увеличиваем длину строки на 1(за счёт нулевого символа)
    cmp rdx, rax ; Сравниваем длину буфера с длиной строки
    js .overflow_string_copy ; Если длина строки больше длины буфера, то переходим к обработке ошибки
    xor rcx, rcx ; Обнуляем счётчик
    .loop_string_copy: ; Цикл копирования
        mov r8b, [rdi + rcx] ; Копируем символ(для сравнения)
        mov [rsi + rcx], r8b ; Копируем символ
        inc rcx ; Увеличиваем счётчик
        cmp r8b, 0 ; Сравниваем символ с концом строки
        je .end_string_copy ; Если символ равен концу строки, то переходим к завершению
        jmp .loop_string_copy ; Если символ не равен концу строки, то переходим к копированию следующего символа
    .overflow_string_copy:
        mov rax, 0 ; Записываем 0 в rax, если строка не помещается в буфер
        ret
    .end_string_copy:
        ret