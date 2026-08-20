#include "Material.h"
#include "Hittable.h"
#include "Utility.h"
namespace rt {

bool Diffuse::scatter(const Ray& ray, const HitRecord& record, ScatterRecord& s_record) const{
    const Vec3& hit_pos = ray.ray_at(record.time);
    const Vec3& normal = record.normal;
    Vec3 scatter_dir = normal + random_unit_vec();
    Vec3 new_pos = hit_pos + normal * 0.001f;

    s_record.scattered_ray = Ray(new_pos, scatter_dir.normalize());
    s_record.attentuation = albedo;
    return true;
}

MaterialType Diffuse::get_type() const{
    return MaterialType::DIFFUSE;
}

static Vec3 metal_reflect(const Vec3& ray_dir, const Vec3& normal){
    return ray_dir - normal * normal.dot(ray_dir) * 2;
}

bool Metal::scatter(const Ray& ray, const HitRecord& record, ScatterRecord& s_record) const{
    
    const Vec3& hit_pos = ray.ray_at(record.time);
    const Vec3& normal = record.normal;
    Vec3 scatter_dir = metal_reflect(ray.direction, normal) + random_unit_vec() * fuzz;
    Vec3 new_pos = hit_pos + normal * 0.001f;

    s_record.scattered_ray = Ray(new_pos, scatter_dir.normalize());
    s_record.attentuation = albedo;

    bool is_shooting_away = scatter_dir.dot(normal) > 0;
    return is_shooting_away;
}

MaterialType Metal::get_type() const{
    return MaterialType::METAL;
}

bool Dielectric::scatter(const Ray& ray, const HitRecord& record, ScatterRecord& s_record) const{
    return false;
}

MaterialType Dielectric::get_type() const{
    return MaterialType::DIELECTRIC;
}

bool Emissive::scatter(const Ray& ray, const HitRecord& record, ScatterRecord& s_record) const{
    return true;
}

Color Emissive::emitted() const{
    return albedo * intensity;
}

MaterialType Emissive::get_type() const{
    return MaterialType::EMISSIVE;
}

}