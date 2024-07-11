#include <cstdio>
#include <errno.h>
#include <cstdint>

namespace Px {
    typedef int Errno;
    struct Rectangle {
        public:
            uint32_t *pixels;
            size_t pixels_width;
            size_t pixels_height;
            size_t rect_width;
            size_t rect_height;
            size_t x_coordinate;
            size_t y_coordinate;
    };

#define RETURN_DEFER(value)                                                                                                                \
    {                                                                                                                                      \
        result = value;                                                                                                                    \
        goto defer;                                                                                                                        \
    }

    void fill(uint32_t *pixels, size_t width, size_t height, uint32_t color) {
        for (size_t i = 0; i < width * height; ++i) {
            pixels[i] = color;
        }
    }

    // void fill_checkers(uint32_t *pixels, uint32_t color1, uint32_t color2) {
    //     int x_max = rect->rect_width + rect->x_coordinate;
    //     int y_max = rect->rect_height + rect->y_coordinate;
    //
    //     for (int i = 0; i < rect->pixels_height; i++) {
    //         int y = rect->y_coordinate + i;
    //
    //         if (y <= y_max) {
    //             for (int j = 0; j < rect->pixels_width; j++) {
    //                 int x = rect->x_coordinate + j;
    //
    //                 if (x <= x_max) {
    //                     rect->pixels[y * rect->pixels_width + x] = color;
    //                 }
    //             }
    //         }
    //     }
    // }

    void fill_rectangle(Rectangle *rect, uint32_t color) {
        if (rect->x_coordinate < 0 || rect->x_coordinate >= rect->pixels_width) rect->x_coordinate = 0;
        if (rect->y_coordinate < 0 || rect->y_coordinate >= rect->pixels_height) rect->y_coordinate = 0;

        int x_max = rect->rect_width + rect->x_coordinate;
        int y_max = rect->rect_height + rect->y_coordinate;

        if (x_max > rect->pixels_width) x_max = rect->pixels_width;
        if (y_max > rect->pixels_height) y_max = rect->pixels_height;

        for (int y = rect->y_coordinate; y < y_max; y++) {
            for (int x = rect->x_coordinate; x < x_max; x++) {
                rect->pixels[x + y * rect->pixels_width] = color;
            }
        }
    }

    Errno save_as_ppm(uint32_t *pixels, size_t width, size_t height, const char *filePath) {
        int result = 0;
        FILE *file = NULL;

        {
            file = fopen(filePath, "wb");
            if (file == NULL) RETURN_DEFER(errno)

            fprintf(file, "P6\n%zu %zu 255\n", width, height);
            if (ferror(file)) RETURN_DEFER(errno)

            for (size_t i = 0; i < width * height; ++i) {
                // In hexadecimal ARGB is represented as 0xAABBGGRR.
                // PPM file format does not support Alpha values.
                // Hence, we need to right shift each pixel by 0, 8 and 16
                // bits to extract Red, Green and Blue pixels respectively.

                uint32_t pixel = pixels[i];
                uint8_t bytes[3] = {
                    (uint8_t)((pixel >> (8 * 0)) & 0xFF),
                    (uint8_t)((pixel >> (8 * 1)) & 0xFF),
                    (uint8_t)((pixel >> (8 * 2)) & 0xFF),
                };

                fwrite(bytes, sizeof(bytes), 1, file);
                if (ferror(file)) RETURN_DEFER(errno)
            }
        }

    defer:
        if (file) fclose(file);
        return result;
    }

} // namespace Px
