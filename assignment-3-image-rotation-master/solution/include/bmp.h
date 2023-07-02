#pragma once

#include "../include/image.h"
#include <stdio.h>

#define BMP_SIGNATURE 0x4D42

struct __attribute__((packed))  bmp_header {
        uint16_t bfType;
        uint32_t bfileSize;
        uint32_t bfReserved;
        uint32_t bOffBits;
        uint32_t biSize;
        uint32_t biWidth;
        uint32_t biHeight;
        uint16_t biPlanes;
        uint16_t biBitCount;
        uint32_t biCompression;
        uint32_t biSizeImage;
        uint32_t biXPelsPerMeter;
        uint32_t biYPelsPerMeter;
        uint32_t biClrUsed;
        uint32_t biClrImportant;
};

enum read_status  {
  READ_OK = 0,
  READ_INVALID_SIGNATURE,
  READ_INVALID_BITS,
  READ_INVALID_HEADER,
  READ_INVALID_BODY,
  READ_INVALID_PADDING
  };

enum read_status read_header(FILE* input_file, struct bmp_header* header);
enum read_status read_body(FILE* input_file, struct image* image);  

enum  write_status  {
  WRITE_OK = 0,
  WRITE_ERROR_HEADER,
  WRITE_ERROR_BODY,
  WRITE_ERROR_PADDING
};

enum write_status to_bmp(FILE* out, struct image const* image);
