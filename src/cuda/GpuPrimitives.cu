#include <raylib.h>
#include "cuda/GpuScene.h"
#include "cuda/cuda_compat.h"
#include "cuda/GpuMaterial.h"
#include "cuda/GpuUtility.h"
#include "cuda/GpuPrimitives.h"
#include <curand_kernel.h>
#include "Vector.h"


namespace rt{

DEVICE Vec3 GpuSphere::sample_point(curandStatePhilox4_32_10_t& state) const{
    Vec3 unit_vec = g_random_unit_vec(state);
    return position + unit_vec * radius;
}

DEVICE Vec3 GpuTriangle::sample_point(curandStatePhilox4_32_10_t& state) const{
  
    float a = g_random_num(state, 0.0f,1.0f);
    float b= g_random_num(state, 0.0f, 1.0f - a);
    float c = 1.0f - a - b;
    return (v1 * a) + (v2 * b) + (v3 * c);
}


}