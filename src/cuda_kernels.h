
#pragma once
#include <raylib.h>
#include "Vector.h"
#include "Camera.h"
#include "Viewport.h"

namespace rt {

struct KernelData {
    Vec3 position;
    Vec3 forward_vector;
    Vec3 up_vector;
    Vec3 right_vector;
    float viewport_dx;
    float viewport_dy;
    float viewport_depth;

   
};

void render_pixels(int width, int height, ::Color* pixels, Camera camera, Viewport viewport);

}