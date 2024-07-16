#include <cstring>
#include "pixar.cpp"

int main() {
    size_t width = 800;
    size_t height = 800;
    uint32_t pixels[width * height];
    const char *rectanglesFileName = "rectangles.ppm";
    const char *circlesFileName = "circles.ppm";

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
    Px::Errno err = Px::save_as_ppm(pixels, width, height, rectanglesFileName);

    Px::fill(pixels, width, height, 0x00000000);
    Px::Circle circle = {
        .pixels = pixels,
        .pixels_width = width,
        .pixels_height = height,
        .radius = 10,
        .x_coordinate = 0,
        .y_coordinate = 0,
    };

    for (int x = 0; x < width; x += 100) {
        circle.x_coordinate = x + 50;
        circle.radius = circle.radius + 5;

        for (int y = 0; y < height; y += 100) {
            circle.y_coordinate = y + 50;

            Px::fill_circle(&circle, 0x0000FF00);
        }
    }

    err = Px::save_as_ppm(pixels, width, height, circlesFileName);

    if (err) {
        printf("Could not save file %s, %s\n", rectanglesFileName, strerror(err));
        return 1;
    }

    return 0;
}
