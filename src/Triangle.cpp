#include "Triangle.h"
#include "Hittable.h"
#include <cmath>

namespace rt {

void Triangle::hit(const Ray& ray, HitRecord& record) const{
    Vec3 E1 = v2 - v1;
    Vec3 E2 = v3 - v1;

    // Normal of triangle
    Vec3 P = ray.direction.cross(E2);
    float det = E1.dot(P);

    if (std::abs(det) <= 0.001){
        return;
    }

    Vec3 T = ray.origin - v1;
    float u = (T.dot(P)) * (1/det);
    
    if (u < 0.0f || u > 1.0f){
        return;
    }

    Vec3 Q = T.cross(E1);
    float v = (ray.direction.dot(Q)) * (1/det);
    if (v < 0.0f || u + v > 1.0f){
        return;
    }

    float t = (E2.dot(Q)) * (1/det);

    if (t < 0.0f){
        return; 
    }

    if (record.time <= -0.999f || t < record.time){
        record.time = t;
        record.normal = E1.cross(E2).normalize();

        if (record.normal.dot(ray.direction) > 0.0f){
            record.normal = record.normal * -1.0f;
        }
        record.material = material;
    }
}
void Triangle::hit(const Ray& ray, HitRecord& record, std::shared_ptr<Material> override_material){
Vec3 E1 = v2 - v1;
    Vec3 E2 = v3 - v1;

    // Normal of triangle
    Vec3 P = ray.direction.cross(E2);
    float det = E1.dot(P);

    if (std::abs(det) <= 0.001){
        return;
    }

    Vec3 T = ray.origin - v1;
    float u = (T.dot(P)) * (1/det);
    
    if (u < 0.0f || u > 1.0f){
        return;
    }

    Vec3 Q = T.cross(E1);
    float v = (ray.direction.dot(Q)) * (1/det);
    if (v < 0.0f || u + v > 1.0f){
        return;
    }

    float t = (E2.dot(Q)) * (1/det);

    if (t < 0.0f){
        return; 
    }

    if (record.time <= -0.999f || t < record.time){
        record.time = t;
        record.normal = E1.cross(E2).normalize();

        if (record.normal.dot(ray.direction) > 0.0f){
            record.normal = record.normal * -1.0f;
        }
        record.material = override_material;
    }
}
Vec3 Triangle::sample_point() const {
    float a = random_num(0.0f, 1.0f);
    float b = random_num(0.0f, 1.0f - a);
    float c = 1.0f - b - a;

    return (v1 * a) + (v2 * b) + (v3 * c);
}

}
