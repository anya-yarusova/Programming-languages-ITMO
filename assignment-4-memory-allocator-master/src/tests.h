#pragma once

#define TESTS_COUNT 5
#define HEAP_INITIAL_SIZE (4096 * 3)

#include "mem.h"
#include "mem_internals.h"
#include "util.h"
#include <unistd.h>

// До теста
void test_before(size_t number, const char* const message);
// После тестаs если тест прошел
void test_after_success(size_t number, const char* const message);
// После теста, если тест не прошел
void test_after_fail(size_t number, const char* const message);

// Тест обычное успешное выделение памяти
void test_malloc_1(void* heap_start);

// Освобождение одного блока из нескольких выделенных.
void test_free_1(void* heap_start);

// Освобождение двух блоков из нескольких выделенных.
void test_free_2(void* heap_start);

// Память закончилась, новый регион памяти расширяет старый.
void test_malloc_2(void* heap_start);

// Память закончилась, старый регион памяти не расширить из-за другого выделенного диапазона адресов, новый регион выделяется в другом месте.
void test_malloc_3(void* heap_start);


// Массив тестов
void (*tests[TESTS_COUNT])(void*) = {
    test_malloc_1,
    test_free_1,
    test_free_2,
    test_malloc_2,
    test_malloc_3,
};