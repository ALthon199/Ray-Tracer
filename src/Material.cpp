#include "Material.h"
#include "Hittable.h"
#include "Utility.h"
namespace rt {

bool Diffuse::scatter(const Ray& ray, const HitRecord& record, ScatterRecord& s_record) const{
    const Vec3& hit_pos = ray.ray_at(record.time);
    const Vec3& normal = record.normal;
    Vec3 scatter_dir = normal + random_unit_vec();
    Vec3 new_pos = hit_pos.hit_offset(normal);
    s_record.scattered_ray = Ray(new_pos, scatter_dir.normalize());
    s_record.attentuation = albedo;
    return true;
}

MaterialType Diffuse::get_type() const{
    return MaterialType::DIFFUSE;
}

Vec3 metal_reflect(const Vec3& ray_dir, const Vec3& normal){
    return ray_dir - normal * normal.dot(ray_dir) * 2;
}



bool Metal::scatter(const Ray& ray, const HitRecord& record, ScatterRecord& s_record) const{
    
    const Vec3& hit_pos = ray.ray_at(record.time);
    const Vec3& normal = record.normal;
    Vec3 scatter_dir = metal_reflect(ray.direction, normal) + random_unit_vec() * fuzz;
    Vec3 new_pos = hit_pos.hit_offset(normal);

    s_record.scattered_ray = Ray(new_pos, scatter_dir.normalize());
    s_record.attentuation = albedo;

    bool is_shooting_away = scatter_dir.dot(normal) > 0;
    return is_shooting_away;
}

MaterialType Metal::get_type() const{
    return MaterialType::METAL;
}

Vec3 refract(const Vec3& ray_dir, const Vec3& normal, float refract_ratio){
    Vec3 perpendicular = (ray_dir + normal * (ray_dir.dot(normal) * -1)) * refract_ratio;
    float k = clamp(1 -  perpendicular.magnitude() * perpendicular.magnitude(), 0.0f, 1.0f);
    Vec3 parallel = normal * (-1) * std::sqrt(k);
    return (parallel + perpendicular);
}

float schlick_reflectance(float refract_ratio, float cosine){
    float r0 = (1 - refract_ratio) / (1 + refract_ratio);
    r0 *= r0;

    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
}
bool Dielectric::scatter(const Ray& ray, const HitRecord& record, ScatterRecord& s_record) const {
   
    bool going_in = ray.direction.dot(record.normal) < 0.0f;


    Vec3 outward_normal = going_in ? record.normal : record.normal * -1.0f;
    float refract_ratio = going_in ? (1.0f / refraction_index) : refraction_index;

    float cos_theta = std::fmin((ray.direction * -1.0f).dot(outward_normal), 1.0f);
    float reflectance = schlick_reflectance(refract_ratio, cos_theta);
    float sin_theta = std::sqrt(1.0f - cos_theta * cos_theta);
    const Vec3& hit_pos = ray.ray_at(record.time);
    if (refract_ratio * sin_theta > 1.0f || reflectance > random_num(0.0f, 1.0f)) {
        Vec3 new_pos = hit_pos.hit_offset(outward_normal);
        Vec3 new_dir = metal_reflect(ray.direction, outward_normal);
        s_record.scattered_ray = Ray(new_pos, new_dir.normalize());

    } else {
        Vec3 new_pos = hit_pos.hit_offset(outward_normal);
        Vec3 new_dir = refract(ray.direction, outward_normal, refract_ratio);
        s_record.scattered_ray = Ray(new_pos, new_dir.normalize());
    }

    s_record.attentuation = Vec3(1.0f, 1.0f, 1.0f);
    return true;
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