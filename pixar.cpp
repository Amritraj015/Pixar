#include <errno.h>
#include "types/rectangle.h"
#include "types/circle.h"
#include "types/line.h"
#include "types/surface.h"

namespace Px {
    typedef int Errno;

#define RETURN_DEFER(value)                                                                                                                \
    {                                                                                                                                      \
        result = value;                                                                                                                    \
        goto defer;                                                                                                                        \
    }

    void draw_line(Surface *surface, Line *line, uint32_t color) {
        float slope = (line->y2 - line->y1) / (line->x2 - line->x1);
        // float constant =

        for (int y = 0; y < surface->height; y++) {
            for (int x = 0; x < surface->width; x++) {
                // if (y == slope)
            }
        }
    }

    void fill(Surface *surface, uint32_t color) {
        for (size_t i = 0; i < surface->width * surface->height; ++i) {
            surface->pixels[i] = color;
        }
    }

    void fill_circle(Surface *surface, Circle *circle, uint32_t color) {
        int radius_sq = circle->radius * circle->radius;

        for (int y = 0; y < surface->height; y++) {
            for (int x = 0; x < surface->width; x++) {
                int x_sq = (circle->x - x) * (circle->x - x);
                int y_sq = (circle->y - y) * (circle->y - y);

                if (x_sq + y_sq <= radius_sq) {
                    surface->pixels[x + y * surface->width] = color;
                }
            }
        }
    }

    void fill_rectangle(Surface *surface, Rectangle *rect, uint32_t color) {
        // If the X coordinate of the top left corner of the rectangle
        // is out of bounds of the surface then, start drawing at X = 0.
        if (rect->x < 0 || rect->x >= surface->width) rect->x = 0;

        // If the Y coordinate of the top left corner of the rectangle
        // is out of bounds of the surface then, start drawing at Y = 0.
        if (rect->y < 0 || rect->y >= surface->height) rect->y = 0;

        int x_max = rect->width + rect->x;
        int y_max = rect->height + rect->y;

        if (x_max > surface->width) x_max = surface->width;
        if (y_max > surface->height) y_max = surface->height;

        for (int y = rect->y; y < y_max; y++) {
            for (int x = rect->x; x < x_max; x++) {
                surface->pixels[x + y * surface->width] = color;
            }
        }
    }

    Errno save_as_ppm(Surface *surface, const char *filePath) {
        int result = 0;
        FILE *file = NULL;

        {
            // Try opening the file in "Write/Binary" mode.
            file = fopen(filePath, "wb");

            // If the file could not be opened, then return with an error.
            if (file == NULL) RETURN_DEFER(errno)

            // Write the required PPM header to the file.
            fprintf(file, "P6\n%zu %zu 255\n", surface->width, surface->height);

            // If the header could not be written, the return with an error.
            if (ferror(file)) RETURN_DEFER(errno)

            for (size_t i = 0; i < surface->width * surface->height; ++i) {
                // In hexadecimal ARGB is represented as 0xAABBGGRR.
                // PPM file format does not support Alpha values.
                // Hence, we need to right shift each pixel by 0, 8 and 16
                // bits to extract Red, Green and Blue pixels respectively.

                uint32_t pixel = surface->pixels[i];

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
