#pragma once
#include "Hittable.h"
#include "Material.h"
#include "Vector.h"
#include <memory>

namespace rt{
class Sphere : public Hittable{
    public: 
        Sphere(Vec3 pos, float radius, std::shared_ptr<Material> material);
        Vec3 sample_point() const override;
        MaterialType get_material_type() const;
        void hit(const Ray& ray, HitRecord& record) const override;
        
    
    private:
        Vec3 position;
        float radius;
        
};
}
