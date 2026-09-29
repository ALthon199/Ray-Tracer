
#pragma once
#include <raylib.h>
#include "Vector.h"
#include "Camera.h"
#include "Viewport.h"
#include "cuda_compat.h"
#include "gpu_scene.h"

namespace rt {

struct KernelData {
    Vec3 position;
    Vec3 forward_vector;
    Vec3 up_vector;
    Vec3 right_vector;
    float viewport_dx;
    float viewport_dy;
    float viewport_depth;
    int width;
    int height;
   
};

void render_pixels(int width, int height, ::Color* pixels, Camera camera, Viewport viewport, GpuScene scene);

}