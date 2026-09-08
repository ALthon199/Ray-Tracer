#pragma once
#include "Triangle.h"
#include "Hittable.h"

namespace rt {

class Mesh : public Hittable {
    public:
    
        Mesh(std::vector<std::unique_ptr<Triangle>> faces, std::shared_ptr<Material> base_material)
        : faces(std::move(faces)), Hittable(base_material)
        {
        }
        
        void hit(const Ray& ray, HitRecord& record) const override;
        Vec3 sample_point() const override;
    
    private:
        std::vector<std::unique_ptr<Triangle>> faces;


};

}

