#pragma once

#include <stdbool.h>
#include <stdint.h>

struct __attribute__((packed)) pixel { 
    uint8_t b, g, r; 
};

struct image {
  uint64_t width, height;
  struct pixel* data;
};

struct image* image_create(uint64_t width, uint64_t height);

struct pixel* image_get_pixel(struct image const* image, uint64_t height, uint64_t width);

void image_destroy(struct image* image);
