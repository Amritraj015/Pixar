#include <cstring>
#include "pixar.cpp"

int main() {
    size_t width = 800;
    size_t height = 800;
    uint32_t pixels[width * height];
    const char *fileName = "dist/output.ppm";

    Px::fill(pixels, width, height, 0x00FFBCBC);
    Px::Rectangle rect = {
        .pixels = pixels,
        .pixels_width = width,
        .pixels_height = height,
        .rect_width = 600,
        .rect_height = 600,
        .x_coordinate = 100,
        .y_coordinate = 100,
    };

    Px::fill_rectangle(&rect, 0x000000FF);

    rect.rect_width = 400;
    rect.rect_height = 400;
    rect.x_coordinate = 200;
    rect.y_coordinate = 200;

    Px::fill_rectangle(&rect, 0x0000FFFF);

    rect.rect_width = 200;
    rect.rect_height = 200;
    rect.x_coordinate = 300;
    rect.y_coordinate = 300;

    Px::fill_rectangle(&rect, 0x00FF00FF);

    Px::Errno err = Px::save_as_ppm(pixels, width, height, fileName);

    if (err) {
        printf("Could not save file %s, %s\n", fileName, strerror(err));
        return 1;
    }

    return 0;
}
