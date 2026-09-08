#pragma once
#include "Material.h"
#include "Vector.h"
#include "Hittable.h"

namespace rt {

class Triangle : public Hittable {
    public:

        void hit(const Ray& ray, HitRecord& record) const override;
        void hit(const Ray& ray, HitRecord& record, std::shared_ptr<Material> material);
        Vec3 sample_point() const override;   

        Triangle(Vec3 v1, Vec3 v2, Vec3 v3, std::shared_ptr<Material> material) : 
        v1(v1), v2(v2), v3(v3), Hittable(material)
        {}

    private:
        Vec3 v1, v2, v3;

        
        
           
};

}
