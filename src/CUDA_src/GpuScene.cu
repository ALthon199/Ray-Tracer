#include "GpuScene.h"
#include "../cuda_compat.h"
#include "../Vector.h"

namespace rt{
DEVICE void hit_sphere(GpuHitRecord& record, const GpuSphere& sphere, const Ray& ray){
    
    const Vec3& position = sphere.position;
    float radius = sphere.radius;
    float a = ray.direction.dot(ray.direction);
    float b = 2 * ray.direction.dot(ray.origin - position); 
    float c = (ray.origin - position).dot(ray.origin - position) - radius * radius;
    float det = b * b - 4 * a * c; 
    // No hits
    if (det < 0.0f) return;

    float t1 = (-b + std::sqrt(det)) / (2 * a);
    float t2 = (-b - std::sqrt(det)) / (2 * a);

    if (t1 < 0.0f && t2 < 0.0f) return;

    float t;
    if (t2 > 0.0f) t = t2;
    else t = t1;
   
    Vec3 P = ray.ray_at(t);
    Vec3 normal = P - position;
    Vec3 unit = normal.normalize();
    
    if (record.time <= -0.999f || t < record.time){
        record.color = sphere.color;
        record.time = t;
        record.normal = unit;
    }
}

DEVICE ::Color raylib_color_from_Vec3(Vec3 vec){
    ::Color color = {
        static_cast<unsigned char>(vec.x * 255.99),
        static_cast<unsigned char>(vec.y * 255.99),
        static_cast<unsigned char>(vec.z * 255.99),
        255
    };

    return color;
}
}