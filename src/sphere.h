#pragma once
#include "Hittable.h"
#include "Vector.h"

namespace rt{
class Sphere : public Hittable{
    private:
        Vec3 position;
        Vec3 color;
        float radius;

    public: 
    
        Sphere(Vec3 pos, Vec3 rgb, float radius);
        Vec3 get_origin() const;
        void hit(const Ray& ray, HitRecord& record) const;
        Vec3 sphere_color() const;
  
};
}
