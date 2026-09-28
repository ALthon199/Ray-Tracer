#pragma once

#include "vector.h"
#include <raylib.h>
#include "cuda_compat.h"

namespace rt{

struct GpuSphere {
    Vec3 position;
    Color color;
    float radius;
    int material_index;
};

struct GpuHitRecord {
    Vec3 normal;
    Color color;
    float time;

    DEVICE GpuHitRecord(){
        normal = Vec3();
        time = -1.0f;
    }
};

DEVICE void hit_sphere(GpuHitRecord& record, const GpuSphere& sphere, const Ray& ray);
DEVICE ::Color raylib_color_from_Vec3(Vec3 vec);
struct GpuTriangle {};
struct GpuMaterial {};

struct GpuScene {
    GpuSphere* spheres;
    size_t spheres_count;
};

}