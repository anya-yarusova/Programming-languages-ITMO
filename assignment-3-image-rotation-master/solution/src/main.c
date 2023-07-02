#include "../include/bmp.h"
#include "../include/image.h"
#include "../include/rotate.h"


int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "%s\n", "Invalid number of arguments");
        return 1;
    }

    FILE* input_file = fopen(argv[1], "rb");
    if (input_file == NULL) {
        fprintf(stderr, "%s\n", "Invalid input file");
        return 1;
    }

    FILE* output_file = fopen(argv[2], "wb");
    if (output_file == NULL) {
        fprintf(stderr, "%s\n", "Invalid output file");
        return 1;
    }

    
    struct bmp_header header = {0};
    enum read_status header_read_result = read_header(input_file, &header);
    if (header_read_result != READ_OK) {
        fprintf(stderr, "%s\n", "Invalid header, file is not a valid BMP");
        return 1;
    }

    struct image* image = image_create(header.biWidth, header.biHeight);
    if (image == NULL) {
        fprintf(stderr, "%s\n", "Invalid image, image_create failed");
        return 1;
    }
    
    enum read_status body_read_result = read_body(input_file, image);
    if (body_read_result != READ_OK) {
        fprintf(stderr, "%s\n", "Invalid body, failed to read");
        return 1;
    }

    if (fclose(input_file) != 0) {
        fprintf(stderr, "%s\n", "Failed to close input file");
        return 1;
    }

    struct image* rotated_image = rotate(image);

    if (rotated_image == NULL) {
        fprintf(stderr, "%s\n", "Failed to rotate image");
        return 1;
    }

    enum write_status write_status = to_bmp(output_file, rotated_image);
    
    if (write_status != WRITE_OK) {
        fprintf(stderr, "%s\n", "Failed to write to output file");
        return 1;
    }

    if (fclose(output_file) != 0) {
        fprintf(stderr, "%s\n", "Failed to close output file");
        return 1;
    }

    image_destroy(image);
    image_destroy(rotated_image);

    return 0;
}
