#include "Vector.h"
#include "cuda/GpuScene.h"
#include "cuda/cuda_compat.h"
#include <curand_kernel.h>
namespace rt {

enum class GpuMaterialType {
    DIFFUSE,
    METAL,
    DIELECTRIC,
    EMISSIVE,
    NONE
};

struct GpuDiffuse {
    Vec3 albedo;
};

struct GpuMetal {
    Vec3 albedo;
    float fuzz;
};

struct GpuDielectric {
    float refraction_index;
};

struct GpuEmissive {
    Vec3 albedo;
    float intensity;
};

union GpuMaterialData{
    HD GpuMaterialData() {} 

    GpuDiffuse diffuse;
    GpuMetal metal;
    GpuDielectric dielectric;
    GpuEmissive emissive;
};

struct GpuScatterRecord {
    Color attentuation;
    Ray scattered_ray;
};

class GpuMaterial {
    public:
        HD GpuMaterial();
        HD GpuMaterial(GpuMaterialType type, GpuMaterialData material_data);
        HD GpuMaterialType get_type() const;
        DEVICE bool scatter(const Ray& ray, const GpuHitRecord& record, GpuScatterRecord& s_record, curandStatePhilox4_32_10_t& state) const;
        HD const GpuMaterialData& get_material() const;
        HD GpuMaterial& operator=(const GpuMaterial& other){
            type = other.type;
            material_data = other.material_data;
            return *this;
        }
    private:
        GpuMaterialType type;
        GpuMaterialData material_data;

};

DEVICE Vec3 g_metal_reflect(const Vec3& ray_dir, const Vec3& normal);

DEVICE Vec3 g_refract(const Vec3& ray_dir, const Vec3& normal, float refract_ratio);
   
DEVICE float g_schlick_reflectance(float refract_ratio, float cosine);

}
