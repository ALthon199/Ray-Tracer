#pragma once
#include "Vector.h"
#include "Hittable.h"
namespace rt {
    
// For the sake of simplicity, light sources will be spheres
class Light : public Hittable{
    public: 
        Light(Vec3 pos, Vec3 rgb, float radius);
        Vec3 get_origin() const;
        void hit(const Ray& ray, HitRecord& record) const;
        Vec3 get_light_color() const;

    private:
        Vec3 position;
        Vec3 color;
        float radius;

};
}

