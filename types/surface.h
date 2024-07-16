#pragma once

#include <cstdint>
#include <cstdio>

namespace Px {
    class Surface {
        public:
            size_t width, height;
            uint32_t *pixels;

            static Surface *create(size_t width, size_t height) {
                uint32_t pixels[width * height];
                Surface *surface = new Surface(width, height);

                return surface;
            }

            ~Surface() {
                delete[] pixels;
                pixels = nullptr;
            };

        private:
            Surface(size_t width, size_t height) : width{ width }, height{ height }, pixels{ new uint32_t[width * height] } {};
    };
} // namespace Px
