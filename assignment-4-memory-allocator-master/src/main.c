#include "mem.h"
#include "tests.h"

int main() {
    void* heap_start = heap_init(HEAP_INITIAL_SIZE);
    if (heap_start == NULL) {
        err("Failed to initialize heap");
        return 1;
    }
    
    for (size_t i = 0; i < TESTS_COUNT; i++) {
        tests[i](heap_start);
    }
    return 0;
}
