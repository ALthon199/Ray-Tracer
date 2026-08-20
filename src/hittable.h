#pragma once

#include "Vector.h"
#include "Utility.h"
#include "Material.h"
#include <memory>

namespace rt {

struct HitRecord {  
    std::shared_ptr<Material> material;
    Vec3 normal;
  
    float time;
  

    HitRecord(){
        material = nullptr;
        normal = Vec3();
        time = -1.0f;
     
    }
};

class Hittable {
    public:
        Hittable(std::shared_ptr<Material> material):
            material(material)
        {

        }
        virtual ~Hittable() = default;
        virtual void hit(const Ray& ray, HitRecord& record) const = 0;
        virtual Vec3 sample_point() const = 0;
        virtual std::shared_ptr<Material> get_material() const {
            return material;
        }

    protected:
        std::shared_ptr<Material> material;
    
};


}

