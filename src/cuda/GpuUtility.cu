#include "cuda/GpuUtility.h"
#include <curand_kernel.h>

namespace rt {
    DEVICE Vec3 g_random_unit_vec(curandStatePhilox4_32_10_t& state){
        float x = 1.0f - 2 * curand_uniform(&state);
        float y = 1.0f - 2 * curand_uniform(&state);
        float z = 1.0f - 2 * curand_uniform(&state);

        return Vec3(x,y,z).normalize();
    }
    DEVICE float g_random_num(curandStatePhilox4_32_10_t& state, float min, float max){
        return (curand_uniform(&state) * (max - min) + min);
    }
    DEVICE float g_clamp(float val, float min, float max){
        if (min > max){
            printf("MIN cannot be greater than MAX");
        }
        if (val < min){
            return min;
        }
        if (val > max){
            return max;
        }

        return val;
    }

}