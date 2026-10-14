#include <raylib.h>
#include "cuda/GpuScene.h"
#include "cuda/cuda_compat.h"
#include "cuda/GpuMaterial.h"
#include "cuda/GpuUtility.h"
#include <curand_kernel.h>
#include "Vector.h"

namespace rt{

DEVICE Vec3 GpuSphere::sample_point(curandStatePhilox4_32_10_t& state) const{
    Vec3 unit_vec = g_random_unit_vec(state);
    return position + unit_vec * radius;
}

// Returns true on hit and updates record, returns false otherwise
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
        record.mat_index = sphere.material_index;
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

DeviceScene::~DeviceScene(){
    if (spheres) {
        cudaFree(spheres);
        spheres = nullptr;
    }

    if (objects) {
        cudaFree(objects);
        objects = nullptr;
    }

    if (materials){
        cudaFree(materials);
        materials = nullptr;
    }

    if (light_indices){
        cudaFree(light_indices);
        light_indices = nullptr;
    }
}

DEVICE bool GpuView::ray_hit(const Ray& ray, GpuHitRecord& record) const {
    for (size_t i = 0; i < objects_count; ++i) {
        const GpuObject& object = objects[i];
        if (object.type == GpuObjectType::SPHERE && object.index < spheres_count) {
            hit_sphere(record, spheres[object.index], ray);
        }
    }
    return record.time > 0.0f;
}

DEVICE Color GpuView::calculate_direct_lighting(const Vec3& pos, const Vec3& surface_normal, curandStatePhilox4_32_10_t& state) const{
    Color direct_lighting = Vec3();
 
    for (int i = 0; i < light_count; ++i){
        GpuHitRecord light_record = GpuHitRecord();
        int light_index = light_indices[i];
        const GpuObject& object = objects[light_index];
        

        switch(object.type){
            case GpuObjectType::SPHERE: {
                if (object.index >= spheres_count) {
                    break;
                }

                const GpuSphere& light = spheres[object.index];
                Vec3 source = pos;
                Vec3 target = light.sample_point(state);
                Vec3 direction = target - source;
                float dist = direction.magnitude();

                Ray lighting_ray = Ray(source, direction);
                lighting_ray.direction.normalize();

                bool hit = ray_hit(lighting_ray, light_record);
                if (!hit) continue;
                const GpuMaterial& material = materials[light_record.mat_index];

                if (!(material.get_type() == GpuMaterialType::EMISSIVE)) continue;
            
                float norm_dot_light = lighting_ray.direction.dot(surface_normal);
                norm_dot_light = g_clamp(norm_dot_light, 0.0f, 1.0f);
                float attenuation = 1.0f / (1.0f + 0.1f * dist + 0.01f * dist * dist);

                const GpuEmissive& src = material.get_material().emissive;
                
                direct_lighting +=  src.albedo * src.intensity * norm_dot_light * attenuation;
                break;
            }
        }

        
    }
    return direct_lighting; 
}




GpuView DeviceScene::view() const {
    return GpuView{spheres, spheres_count, objects, objects_count, 
        materials, materials_count, light_indices, light_count};
}



void DeviceScene::upload_scene(const HostScene& scene){
    cudaMalloc(&objects, sizeof(GpuObject) * scene.get_objects().size());
    objects_count = scene.get_objects().size();

    cudaMalloc(&spheres, sizeof(GpuSphere) * scene.get_spheres().size());
    spheres_count = scene.get_spheres().size();

    cudaMalloc(&materials, sizeof(GpuMaterial) * scene.get_materials().size());
    materials_count = scene.get_materials().size();

    cudaMalloc(&light_indices, sizeof(int) * scene.get_light_indices().size());
    light_count =  scene.get_light_indices().size();

    cudaMemcpy(objects, scene.get_objects().data(), sizeof(GpuObject) * scene.get_objects().size(), cudaMemcpyHostToDevice);
    cudaMemcpy(spheres, scene.get_spheres().data(), sizeof(GpuSphere) * scene.get_spheres().size(), cudaMemcpyHostToDevice);
    cudaMemcpy(materials, scene.get_materials().data(),  sizeof(GpuMaterial) * scene.get_materials().size(), cudaMemcpyHostToDevice);
    cudaMemcpy(light_indices, scene.get_light_indices().data(), sizeof(int) * scene.get_light_indices().size(), cudaMemcpyHostToDevice);

}


void HostScene::add_sphere(GpuSphere sphere, GpuMaterial material){
    
    sphere.material_index = materials.size();
    GpuObject object = {GpuObjectType::SPHERE, spheres.size()};
    
    if (material.get_type() == GpuMaterialType::EMISSIVE){
        light_indices.push_back(objects.size());
    }
    materials.push_back(material);
    spheres.push_back(sphere);
    objects.push_back(object);
   
}


}
