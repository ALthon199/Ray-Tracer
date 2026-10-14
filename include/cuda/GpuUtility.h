#include "Vector.h"
#include <curand_kernel.h>
#include "cuda_compat.h"
namespace rt {

DEVICE Vec3 g_random_unit_vec(curandStatePhilox4_32_10_t& state);
DEVICE float g_random_num(curandStatePhilox4_32_10_t& state, float min, float max);
DEVICE float g_clamp(float val, float min, float max);
}