#ifndef SPHERE_H
#define SPHERE_H

#include "vector.h"
class Sphere{
    private:
        Vec3 position;
        Vec3 color;
        float radius;

    public:
        Sphere(Vec3 pos, Vec3 rgb, float radius);

        Vec3 get_origin() const;
        float sphere_hit (const Ray& ray) const;
        Vec3 sphere_color() const;
  
};

#endif