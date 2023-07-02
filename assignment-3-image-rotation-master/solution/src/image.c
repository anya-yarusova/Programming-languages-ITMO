#include "../include/image.h"

#include <stdlib.h>

struct image* image_create(uint64_t width, uint64_t height) {
    struct image* image = malloc(sizeof(struct image));
    if (image == NULL) {
        return NULL;
    }
    image->width = width;
    image->height = height;
    image->data = malloc(width * height * sizeof(struct pixel));
    if (image->data == NULL) {
        free(image->data);
        free(image);
        return NULL;
    }
    return image;
}


struct pixel* image_get_pixel(struct image const* image, uint64_t height, uint64_t width) {
    if (image == NULL || width >= image->width || height >= image->height) {
        return NULL;
    }
    return &image->data[height * image->width + width];
}

void image_destroy(struct image* image) {
    free(image->data);
    free(image);
}
