#include "Sphere.h"
#include "Vector.h"
#include "Hittable.h"
#include "Utility.h"
#include "Material.h"

namespace rt{
Sphere::Sphere(Vec3 position, float radius, std::shared_ptr<Material> material):
    position(position), radius(radius), Hittable(material)
{

}

MaterialType Sphere::get_material_type() const {
    return material -> get_type();
}

// Returns the time t >= 0 of where the ray hits, returns -1 if doesnt hit
void Sphere::hit(const Ray& ray, HitRecord& record) const {
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
      
        record.time = t;
        record.normal = unit;
        
        record.material = material;
    }
}

Vec3 Sphere::sample_point() const{
    return position + random_unit_vec() * radius;
}


}
