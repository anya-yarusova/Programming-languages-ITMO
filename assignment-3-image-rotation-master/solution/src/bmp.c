#include "../include/bmp.h"
#include "../include/image.h"
#include <stdlib.h>

static size_t get_padding(size_t width) {
    if (width % 4 == 0) {
        return 0;
    } else {
        return 4 - ((width * sizeof(struct pixel)) % 4);
    }
}

enum read_status read_header(FILE* input_file, struct bmp_header* header) {
    if (input_file == NULL || header == NULL) {
        return READ_INVALID_HEADER;
    }
    if (fread(header, sizeof(struct bmp_header), 1, input_file) != 1) {
        return READ_INVALID_HEADER;
    }
    if (header->bfType != BMP_SIGNATURE) {
        return READ_INVALID_SIGNATURE;
    }
    return READ_OK;
}

enum read_status read_body(FILE* input_file, struct image* image) {
    if (input_file == NULL || image == NULL) {
        return READ_INVALID_BITS;
    }

    size_t padding = get_padding(image->width);

    for (size_t i = 0; i < image->height; i++) {
        for (size_t j = 0; j < image->width; j++) {
            if (fread(image_get_pixel(image, i, j), sizeof(struct pixel), 1, input_file) != 1) {
                return READ_INVALID_BODY;
            }
        }
        if (fseek(input_file, (long)padding, SEEK_CUR) != 0) {
            return READ_INVALID_PADDING;
        }
    }
    return READ_OK;
}


static struct bmp_header create_header(struct image const* image) {
    struct bmp_header header = {0};
    header.bfType = 0x4D42;
    header.bfileSize = sizeof(struct bmp_header) + sizeof(struct pixel) * image->width * image->height
                        + get_padding(image->width) * image->height;
    header.bfReserved = 0;
    header.bOffBits = sizeof(struct bmp_header);
    header.biSize = sizeof(struct bmp_header) - 14;
    header.biWidth = image->width;
    header.biHeight = image->height;
    header.biPlanes = 1;
    header.biBitCount = 24;
    header.biCompression = 0;
    header.biSizeImage = sizeof(struct pixel) * image->width * image->height + get_padding(image->width) * image->height;
    header.biXPelsPerMeter = 0;
    header.biYPelsPerMeter = 0;
    header.biClrUsed = 0;
    header.biClrImportant = 0;
    return header;
}

enum write_status write_header(FILE* output_file, struct image const* image) {
    if (output_file == NULL || image == NULL) {
        return WRITE_ERROR_HEADER;
    }
    struct bmp_header header = create_header(image);
    if (fwrite(&header, sizeof(struct bmp_header), 1, output_file) != 1) {
        return WRITE_ERROR_HEADER;
    }
    return WRITE_OK;
}

enum write_status write_padding(FILE* output_file, size_t padding) {
    if (output_file == NULL) {
        return WRITE_ERROR_PADDING;
    }
    uint8_t padding_zero = 0;
    for (size_t i = 0; i < padding; i++) {
        if (fwrite(&padding_zero, sizeof(uint8_t), 1, output_file) != 1) {
            return WRITE_ERROR_PADDING;
        }
    }
    return WRITE_OK;
}

enum write_status write_body(FILE* output_file, struct image const* image) {
    if (output_file == NULL || image == NULL) {
        return WRITE_ERROR_BODY;
    }
    size_t padding = get_padding(image->width);
    for (size_t i = 0; i < image->height; i++) {
        for (size_t j = 0; j < image->width; j++) {
            if (fwrite(image_get_pixel(image, i, j), sizeof(struct pixel), 1, output_file) != 1) {
                return WRITE_ERROR_BODY;
            }
        }
        enum write_status write_padding_result = write_padding(output_file, padding);
        if (write_padding_result != WRITE_OK) {
            return write_padding_result;
        }
    }
    return WRITE_OK;
}

enum write_status to_bmp(FILE* out, struct image const* image) {
    enum write_status header_write_result = write_header(out, image);
    if (header_write_result != WRITE_OK) {
        return header_write_result;
    }
    enum write_status body_write_result = write_body(out, image);
    return body_write_result;
}
