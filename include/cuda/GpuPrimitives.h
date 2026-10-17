#pragma once
#include "Vector.h"
#include "cuda_compat.h"
#include <curand_kernel.h>


namespace rt {

struct GpuTriangle {
    Vec3 v1, v2, v3;
    int material_index;

    DEVICE Vec3 sample_point(curandStatePhilox4_32_10_t& state) const;
};

struct GpuSphere {
    Vec3 position;
    Color color;
    float radius;
    int material_index;

    DEVICE Vec3 sample_point(curandStatePhilox4_32_10_t& state) const;
};

}

