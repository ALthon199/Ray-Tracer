#pragma once

#include "Vector.h"
#include "Utility.h"
#include <memory>
namespace rt {

struct HitRecord {  
    bool hit_light;
    float time;
    Vec3 normal;
    Vec3 color;
};

class Hittable {
    public:
        virtual ~Hittable() = default;
        virtual void hit(const Ray& ray, HitRecord& record) const = 0;
};


}

