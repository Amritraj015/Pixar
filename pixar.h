#pragma once

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <errno.h>

#include "types/circle.h"
#include "types/line.h"
#include "types/rectangle.h"

namespace Px {
    typedef int Errno;

// A macro to return an error if file operations fail.
#define RETURN_DEFER(value)                                                                                                                \
    {                                                                                                                                      \
        result = value;                                                                                                                    \
        goto defer;                                                                                                                        \
    }

    /** A class to represent a surface which can be drawn on. */
    class Surface {
        public:
            /** Width of the current surface. */
            size_t width;

            /** Height of the current surface. */
            size_t height;

            /** Pixels to represent the surface. */
            uint32_t **pixels;

            /**
             * Creates a new surface.
             * @param width - Width of the surface in pixels.
             * @param height - Height of the surface in pixels.
             * */
            static Surface *create(size_t width, size_t height) {
                return new Surface(width, height);
            }

            /**
             * Colors all pixels of the provided surface with a given color.
             * @param color - A color to apply to all pixels.
             * */
            void apply_color(uint32_t color) {
                for (size_t i = 0; i < this->width; ++i) {
                    for (size_t j = 0; j < this->height; ++j) {
                        this->pixels[i][j] = color;
                    }
                }
            }

            /**
             * Draws a rectangle on the surface.
             * @param rect - Rectangle information to be drawn.
             * @param color - Color to apply to the rectangle.
             * */
            void draw_rectangle(Rectangle *rect, uint32_t color) {
                // If the X coordinate of the top left corner of the rectangle
                // is out of bounds of the surface then, start drawing at X = 0.
                if (rect->x < 0 || rect->x >= this->width) rect->x = 0;

                // If the Y coordinate of the top left corner of the rectangle
                // is out of bounds of the surface then, start drawing at Y = 0.
                if (rect->y < 0 || rect->y >= this->height) rect->y = 0;

                // Calculate X and Y bounds.
                int x_max = rect->width + rect->x;
                int y_max = rect->height + rect->y;

                // If X and Y bounds lie outside of the surface,
                // then set them to the "width" and "height" of the surface respectively.
                if (x_max > this->width) x_max = this->width;
                if (y_max > this->height) y_max = this->height;

                // Color the pixels.
                for (int y = rect->y; y < y_max; y++) {
                    for (int x = rect->x; x < x_max; x++) {
                        this->pixels[x][y] = color;
                    }
                }
            }

            /**
             * Draws a circle on the surface.
             * @param circle - Information about the circle to be drawn.
             * @param color - Color to apply to the circle.
             * */
            void draw_circle(Circle *circle, uint32_t color) {
                // Calculate the square of the radius of the circle.
                int radius_sq = circle->radius * circle->radius;

                for (int y = 0; y < this->height; y++) {
                    for (int x = 0; x < this->width; x++) {
                        // Find if the current point lies inside of the circle.
                        int x_sq = (circle->x - x) * (circle->x - x);
                        int y_sq = (circle->y - y) * (circle->y - y);

                        // If the point lies inside of the circle,
                        // then apply the required color to it.
                        if (x_sq + y_sq <= radius_sq) {
                            this->pixels[x][y] = color;
                        }
                    }
                }
            }

            /**
             * Draws a line on the surface.
             * @param line - Information about the line to be drawn.
             * @param color - Color for the line to be drawn.
             * */
            void draw_line(Line *line, uint32_t color) {
                bool isVerticalLine = line->x1 == line->x2;

                // Calculate the slope (m) and constant (c) for
                // the line from the equations:
                // 1. m = (y2 - y1)/(x2 - x1) and
                // 2. y = (m * x) + c; respectively.
                float slope = (line->y2 - line->y1) / (line->x2 - line->x1);
                float constant = line->y1 - (slope * line->x1);

                // Find the start and end points of the line.
                int startX = line->x1 > line->x2 ? line->x2 : line->x1;
                int startY = line->y1 > line->y2 ? line->y2 : line->y1;
                int endX = line->x1 > line->x2 ? line->x1 : line->x2;
                int endY = line->y1 > line->y2 ? line->y1 : line->y2;

                // Make sure that the starting points are in bounds of the surface.
                if (startX < 0) startX = 0;
                if (startY < 0) startY = 0;
                if (startX >= this->width) startX = this->width - 1;
                if (startY >= this->height) startY = this->height - 1;

                // Make sure that the end points are in bounds of the surface.
                if (endX < 0) endX = 0;
                if (endY < 0) endY = 0;
                if (endX >= this->width) endX = this->width - 1;
                if (endY >= this->height) endY = this->height - 1;

                // If the line is vertical then simply draw a vertical line.
                if (isVerticalLine) {
                    for (int y = startY; y <= endY; y++) {
                        for (int x = startX; x <= endX; x++) {
                            this->pixels[x][y] = color;
                        }
                    }
                } else {
                    // Else, find if a given point lies on the line.
                    // If that is the case, then color it.
                    for (int y = startY; y <= endY; y++) {
                        for (int x = startX; x <= endX; x++) {
                            int equation = y - (slope * x) - constant;

                            // if equation is equal to 0,
                            // then this point lies on the line.
                            if (equation == 0) {
                                // Color the pixel.
                                this->pixels[x][y] = color;
                            }
                        }
                    }
                }
            }

            /**
             * Saves the current state of the surface as a PPM file.
             * @param filePath - Destination path where to save the PPM file.
             * */
            Errno save_as_ppm(const char *filePath) {
                int result = 0;
                FILE *file = NULL;

                {
                    // Try opening the file in "Write/Binary" mode.
                    file = fopen(filePath, "wb");

                    // If the file could not be opened, then return with an error.
                    if (file == NULL) RETURN_DEFER(errno)

                    // Write the required PPM header to the file.
                    fprintf(file, "P6\n%zu %zu 255\n", this->width, this->height);

                    // If the header could not be written, the return with an error.
                    if (ferror(file)) RETURN_DEFER(errno)

                    for (size_t i = 0; i < this->width; ++i) {
                        for (size_t j = 0; j < this->height; ++j) {
                            // In hexadecimal ARGB is represented as 0xAABBGGRR.
                            // PPM file format does not support Alpha values.
                            // Hence, we need to right shift each pixel by 0, 8 and 16
                            // bits to extract Red, Green and Blue pixels respectively.

                            uint32_t pixel = this->pixels[i][j];

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
                }

            defer:
                if (file) fclose(file);
                return result;
            }

            /**
             * Destructor that cleans up the surface.
             * */
            ~Surface() {
                if (nullptr != this->pixels) {
                    for (int i = 0; i < width; i++) {
                        delete[] this->pixels[i];
                    }

                    delete[] pixels;
                    pixels = nullptr;
                }
            };

        private:
            /**
             * Creates a new instance of `Surface`.
             * @param width - Width of the surface to created.
             * @prarm height - Height of the surface to be created.
             * */
            Surface(size_t width, size_t height) {
                this->width = width;
                this->height = height;
                this->pixels = new uint32_t *[width];

                for (int i = 0; i < width; i++) {
                    this->pixels[i] = new uint32_t[height];
                }
            };
    };
} // namespace Px
