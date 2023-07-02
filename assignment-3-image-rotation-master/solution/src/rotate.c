#include "../include/rotate.h"
#include "../include/image.h"
#include <stdio.h>

struct image* rotate(struct image* const source) {
    if (source == NULL) {
        fprintf(stderr, "%s\n", "Invalid source");
        return NULL;
    }
    // why source is not NULL?
    struct image* rotated_image = image_create(source->height, source->width);
    for (uint64_t i = 0; i < rotated_image->height; i++) {
        for (uint64_t j = 0; j < rotated_image->width; j++) {
            struct pixel* rotated_pixel = image_get_pixel(rotated_image, i, j);
            struct pixel* source_pixel = image_get_pixel(source, source->height - j - 1, i);
            rotated_pixel->r = source_pixel->r;
            rotated_pixel->g = source_pixel->g;
            rotated_pixel->b = source_pixel->b;
        }
    }
    return rotated_image;
}
