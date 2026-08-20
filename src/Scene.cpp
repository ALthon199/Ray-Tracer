#include "Scene.h"
#include "Material.h"


namespace rt {

void Scene::add_hittable(std::unique_ptr<Hittable> object){
    if (object -> get_material() -> get_type() == MaterialType :: EMISSIVE){
        light_list.push_back(object.get());
    }
    hit_list.push_back(std::move(object));
}



const std::vector<std::unique_ptr<Hittable>>& Scene::get_hit_list() const{
    return hit_list;
}

// Returns true on hit and updates record, returns false otherwise
bool Scene::ray_hit(const Ray& ray, HitRecord& record) const{
    for (const auto& obj: hit_list){
        obj -> hit(ray, record);
    }
    
    // Time has to be positive for a hit
    if (record.time <= 0.0f){
        return false;
    }
    return true;
}

Color Scene::direct_lighting(const Vec3& pos, const Vec3& surface_normal) const{
    Color direct_lighting = Vec3();
 
    for (const auto& light: light_list){
        HitRecord light_record = HitRecord();
        
        Vec3 source = pos;
        Vec3 target = light -> sample_point();
        Vec3 direction = target - source;
        float dist = direction.magnitude();

        Ray lighting_ray = Ray(source, direction);
        lighting_ray.direction.normalize();

        bool hit = ray_hit(lighting_ray, light_record);
        if (!hit) continue;
        if (!(light_record.material -> get_type() == MaterialType::EMISSIVE)) continue;
    
        float norm_dot_light = lighting_ray.direction.dot(surface_normal);
        norm_dot_light = clamp(norm_dot_light, 0.0f, 1.0f);
        float attenuation = 1.0f / (1.0f + 0.1f * dist + 0.01f * dist * dist);
        direct_lighting += light_record.material ->emitted() * norm_dot_light * attenuation;
    }


    return direct_lighting;
}

}