#define _DEFAULT_SOURCE

#include "tests.h"

#define QUERY_TEST 100
#define QUERY_TEST_MALLOC_3 30000


// I don't know why this function is static in mem.c
static struct block_header* block_get_header(void* contents) {
  return (struct block_header*) (((uint8_t*)contents)-offsetof(struct block_header, contents));
}

void test_before(size_t number, const char* const message) {
    debug("Test %d: %s", number, message);
    debug("Runnning test %d", number);
}

void test_after_success(size_t number, const char* const message) {
    debug("Test %d passed", number);
    debug("Test %d: %s", number, message);
}

void test_after_fail(size_t number, const char* const message) {
    err("Test %d failed", number);
    err("Test %d: %s", number, message);
}


void test_malloc_1(void* heap_start) {
    test_before(1, "Simlple allocation");

    void* ptr = _malloc(QUERY_TEST);
    if (ptr == NULL) {
        test_after_fail(1, "_malloc returned NULL");
        return;
    }
    debug_heap(stderr, heap_start);

    struct block_header* header = block_get_header(ptr);
    if (header->is_free) {
        test_after_fail(1, "Block is not marked as allocated");
        return;
    }
    if (header->capacity.bytes < QUERY_TEST) {
        test_after_fail(1, "Block capacity is less than requested");
        return;
    }

    _free(ptr);
    debug_heap(stderr, heap_start);

    if(!header->is_free) {
        test_after_fail(1, "Block is not marked as free");
        return;
    }

    test_after_success(1, "Successfully allocated");
}

void test_free_1(void* heap_start) {
    test_before(2, "Free one block from several allocated");

    void* ptr1 = _malloc(QUERY_TEST);
    void* ptr2 = _malloc(QUERY_TEST);
    void* ptr3 = _malloc(QUERY_TEST);
    if (ptr1 == NULL || ptr2 == NULL || ptr3 == NULL) {
        test_after_fail(2, "_malloc returned NULL");
        return;
    }

    struct block_header* header1 = block_get_header(ptr1);
    struct block_header* header2 = block_get_header(ptr2);
    struct block_header* header3 = block_get_header(ptr3);
    debug_heap(stderr, heap_start);
    _free(ptr2);
    debug_heap(stderr, heap_start);
    if(!header2->is_free) {
        test_after_fail(2, "Second block is not marked as free");
        return;
    }
    if(header1->is_free || header3->is_free) {
        test_after_fail(2, "First or third block is marked as free");
        return;
    }

    _free(ptr1);
    _free(ptr3);
    test_after_success(2, "Successfully freed");
}

void test_free_2(void* heap_start) {
    test_before(3, "Free two blocks from several allocated");

    void* ptr1 = _malloc(QUERY_TEST);
    void* ptr2 = _malloc(QUERY_TEST);
    void* ptr3 = _malloc(QUERY_TEST);
    if (ptr1 == NULL || ptr2 == NULL || ptr3 == NULL) {
        test_after_fail(3, "_malloc returned NULL");
        return;
    }

    struct block_header* header1 = block_get_header(ptr1);
    struct block_header* header2 = block_get_header(ptr2);
    struct block_header* header3 = block_get_header(ptr3);
    debug_heap(stderr, heap_start);
    _free(ptr2);
    _free(ptr1);
    debug_heap(stderr, heap_start);
    if(header3->is_free) {
        test_after_fail(3, "Third block is marked as free");
        _free(ptr3);
        return;
    }
    if(!header1->is_free) {
        test_after_fail(3, "First block is not marked as free");
        _free(ptr3);
        return;
    }
    if (header1->next != header3 && header2 != header3) {
        test_after_fail(3, "First block is not linked to third block, not correctly merged");
        _free(ptr3);
        return;
    }
    _free(ptr3);
    test_after_success(3, "Successfully freed"); 
}

void test_malloc_2(void* heap_start) {
    test_before(4, "Allocate memory when there is no free space");

    void* ptr = _malloc(HEAP_INITIAL_SIZE + QUERY_TEST);
    if (ptr == NULL) {
        test_after_fail(4, "_malloc returned NULL");
        return;
    }
    debug_heap(stderr, heap_start);

    struct block_header* header = block_get_header(ptr);
    if (header->is_free) {
        test_after_fail(4, "Block is not marked as allocated");
        return;
    }
    if (header->capacity.bytes < HEAP_INITIAL_SIZE + QUERY_TEST) {
        test_after_fail(4, "Block capacity is less than requested");
        return;
    }
    if ((void*) header > (void*) ((uint8_t*) heap_start + HEAP_INITIAL_SIZE)) {
        test_after_fail(4, "Block is not in the old region");
        return;
    }

    _free(ptr);
    test_after_success(4, "Successfully allocated");
}

void test_malloc_3(void* heap_start) {
    test_before(5, "Allocate memory when there is no free space and old region cannot be extended");

    void* result = mmap((void*) ((uint8_t*) heap_start + HEAP_INITIAL_SIZE * 3), getpagesize(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE , 0, 0 );
    if (result != MAP_FAILED) {
        if (result != (void*) ((uint8_t*) heap_start + HEAP_INITIAL_SIZE * 3)) {
            test_after_fail(5, "Cannot allocate memory");
            return;
        }
    } else { 
        test_after_fail(5, "Cannot allocate memory");
        return;
    }

    void* ptr = _malloc(QUERY_TEST_MALLOC_3);
    if (ptr == NULL) {
        test_after_fail(5, "_malloc returned NULL");
        return;
    }
    debug_heap(stderr, heap_start);

    struct block_header* header = block_get_header(ptr);
    if (header->is_free) {
        test_after_fail(5, "Block is not marked as allocated");
        return;
    }
    if (header->capacity.bytes < QUERY_TEST_MALLOC_3) {
        test_after_fail(5, "Block capacity is less than requested");
        return;
    }
    if ((void*) header < (void*) ((uint8_t*) heap_start + HEAP_INITIAL_SIZE * 3)) {
        test_after_fail(4, "Block is in the old region");
        return;
    }

    _free(ptr);
    test_after_success(5, "Successfully allocated");
}
