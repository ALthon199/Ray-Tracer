#pragma once
#include <iostream>
#include <raylib.h>
#include <vector>
#include <cassert>
#include <cmath>
#include "Vector.h"

namespace rt{
class ImageBuffer{
    public: 
        ImageBuffer(int window_width, int window_height) :
            width(window_width), height(window_height), pixels(window_width * window_height)
        {
        }
        void set_pixel(int row, int col, const rt::Color& pixel){
            // std::cout << pixel.x << " " << pixel.y << " " << pixel.z << "\n";
            float r = std::sqrt(pixel.x);
            float g = std::sqrt(pixel.y);
            float b = std::sqrt(pixel.z);
            pixels[row * width + col] = ::Color{static_cast<unsigned char>(rgb_map(r)), static_cast<unsigned char>(rgb_map(g)), static_cast<unsigned char>(rgb_map(b)), 255};
        }
        int get_width() const{
            return width;
        }
        int get_height() const{
            return height;
        }
        const std::vector<::Color>& get_pixels() const{
            return pixels;
        }

    private:
        int width;
        int height;
        std::vector<::Color> pixels;
};
}