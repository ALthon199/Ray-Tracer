#pragma once

#include <raylib.h>
#include <vector>
#include <cassert>
#include "Vector.h"

namespace rt{
class ImageBuffer{
    public: 
        ImageBuffer(int window_width, int window_height) :
            width(window_width), height(window_height), pixels(window_width * window_height)
        {

        }

        void set_pixel(int row, int col, const rt::Color& pixel){
            
            pixels[row * width + col] = ::Color{static_cast<unsigned char>(pixel.x), static_cast<unsigned char>(pixel.y), static_cast<unsigned char>(pixel.z), 255};
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