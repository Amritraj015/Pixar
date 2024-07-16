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

    struct Circle {
        public:
            uint32_t *pixels;
            size_t pixels_width;
            size_t pixels_height;
            size_t radius;
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

    void fill_circle(Circle *circle, uint32_t color) {
        // if (radius >= width || radius >= height) radius = width > height ? height / 2 : width / 2;

        int radius_sq = circle->radius * circle->radius;

        for (int y = 0; y < circle->pixels_height; y++) {
            for (int x = 0; x < circle->pixels_width; x++) {
                int x_sq = (circle->x_coordinate - x) * (circle->x_coordinate - x);
                int y_sq = (circle->y_coordinate - y) * (circle->y_coordinate - y);

                if (x_sq + y_sq <= radius_sq) {
                    circle->pixels[x + y * circle->pixels_width] = color;
                }
            }
        }
    }

    void fill_rectangle(Rectangle *rect, uint32_t color) {
        // If the X coordinate of the top left corner of the rectangle
        // is out of bounds of the surface then, start drawing at X = 0.
        if (rect->x_coordinate < 0 || rect->x_coordinate >= rect->pixels_width) rect->x_coordinate = 0;

        // If the Y coordinate of the top left corner of the rectangle
        // is out of bounds of the surface then, start drawing at Y = 0.
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
            // Try opening the file in "Write/Binary" mode.
            file = fopen(filePath, "wb");

            // If the file could not be opened, then return with an error.
            if (file == NULL) RETURN_DEFER(errno)

            // Write the required PPM header to the file.
            fprintf(file, "P6\n%zu %zu 255\n", width, height);

            // If the header could not be written, the return with an error.
            if (ferror(file)) RETURN_DEFER(errno)

            for (size_t i = 0; i < width * height; ++i) {
                // In hexadecimal ARGB is represented as 0xAABBGGRR.
                // PPM file format does not support Alpha values.
                // Hence, we need to right shift each pixel by 0, 8 and 16
                // bits to extract Red, Green and Blue pixels respectively.

                uint32_t pixel = pixels[i];

                // Extract the Red, Green and Blue bytes from the pixel color.
                uint8_t bytes[3] = {
                    (uint8_t)((pixel >> (8 * 0)) & 0xFF),
                    (uint8_t)((pixel >> (8 * 1)) & 0xFF),
                    (uint8_t)((pixel >> (8 * 2)) & 0xFF),
                };

                // Try writing these bytes to the file.
                fwrite(bytes, sizeof(bytes), 1, file);

                // If writing failed then, return with an error.
                if (ferror(file)) RETURN_DEFER(errno)
            }
        }

    defer:
        if (file) fclose(file);
        return result;
    }
} // namespace Px
