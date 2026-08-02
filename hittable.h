#ifndef HITTABLE_H
#define HITTABLE_H

#include "vector.h"

namespace rt{

struct HitRecord {  
    float time;
    Vec3 normal;
    Vec3 color;
};

class Hittable{
    public:
        virtual ~Hittable() = default;
        virtual void hit(const Ray& ray, HitRecord& record) const = 0;
};

}
#endif