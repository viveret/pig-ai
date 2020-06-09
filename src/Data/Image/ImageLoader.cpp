#include "ImageLoader.hpp"
using namespace PigAI;

#include <png++-0.2.9/png.hpp>

ImageData* PigAI::Load_Image(const std::string& path) {
    std::cout << "loading image " << path << std::endl;
    png::image< png::rgb_pixel > image(path.c_str());
    char *data = new char[3 * image.get_width() * image.get_height()];
    for (int y = 0; y < image.get_height(); y++) {
        for (int x = 0; x < image.get_width(); x++) {
            auto pixel = image[y][x];
            data[y * image.get_width() * 3 + x * 3 + 0] = pixel.red;
            data[y * image.get_width() * 3 + x * 3 + 1] = pixel.green;
            data[y * image.get_width() * 3 + x * 3 + 2] = pixel.blue;
        }
    }
    auto ret = new ImageData {
        -1, -1, image.get_width(), image.get_height(),
        3, path, data
    };
    std::cout << "properties: " << ret->str() << std::endl;
    return ret;
}