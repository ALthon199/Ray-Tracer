#include <raylib.h>
#include "cuda/GpuScene.h"
#include "cuda/cuda_compat.h"
#include "cuda/GpuMaterial.h"
#include "cuda/GpuUtility.h"
#include "cuda/GpuPrimitives.h"
#include <curand_kernel.h>
#include "Vector.h"

namespace rt{

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


DEVICE void hit_triangle(GpuHitRecord& record, const GpuTriangle& triangle, const Ray& ray){
    Vec3 E1 = triangle.v2 - triangle.v1;
    Vec3 E2 = triangle.v3 - triangle.v1;

    // Normal of triangle
    Vec3 P = ray.direction.cross(E2);
    float det = E1.dot(P);

    if (std::abs(det) <= 0.001){
        return;
    }

    Vec3 T = ray.origin - triangle.v1;
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
        record.mat_index = triangle.material_index;
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
    for (GpuTriangle* faces : mesh_face_allocations) {
        cudaFree(faces);
    }
    mesh_face_allocations.clear();

    if (spheres) {
        cudaFree(spheres);
        spheres = nullptr;
    }

    if (triangles) {
        cudaFree(triangles);
        triangles = nullptr;
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
        if (object.type == GpuObjectType::TRIANGLE && object.index < triangles_count) {
            hit_triangle(record, triangles[object.index], ray);
        }
        if (object.type == GpuObjectType::MESH && object.index < meshes_count) {
            const GpuMeshData& mesh = meshes[object.index];
            for (size_t face = 0; face < mesh.tri_count; ++face) {
                const float previous_time = record.time;
                hit_triangle(record, triangles[mesh.tri_start + face], ray);
                if (record.time != previous_time) {
                    record.mat_index = triangles[mesh.tri_start + face].material_index;
                }
            }
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

            case GpuObjectType::TRIANGLE: {
                if (object.index >= triangles_count) {
                    break;
                }

                const GpuTriangle& light = triangles[object.index];
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
    return GpuView{spheres, spheres_count, triangles, triangles_count, meshes, meshes_count,
        objects, objects_count, materials, materials_count, light_indices, light_count};
}



void DeviceScene::upload_scene(const HostScene& scene){
    cudaMalloc(&objects, sizeof(GpuObject) * scene.get_objects().size());
    objects_count = scene.get_objects().size();

    cudaMalloc(&spheres, sizeof(GpuSphere) * scene.get_spheres().size());
    spheres_count = scene.get_spheres().size();

    cudaMalloc(&triangles, sizeof(GpuTriangle) * scene.get_triangles().size());
    triangles_count = scene.get_triangles().size();

    cudaMalloc(&materials, sizeof(GpuMaterial) * scene.get_materials().size());
    materials_count = scene.get_materials().size();

    cudaMalloc(&light_indices, sizeof(int) * scene.get_light_indices().size());
    light_count =  scene.get_light_indices().size();

    cudaMemcpy(objects, scene.get_objects().data(), sizeof(GpuObject) * scene.get_objects().size(), cudaMemcpyHostToDevice);
    cudaMemcpy(spheres, scene.get_spheres().data(), sizeof(GpuSphere) * scene.get_spheres().size(), cudaMemcpyHostToDevice);
    cudaMemcpy(triangles, scene.get_triangles().data(), sizeof(GpuTriangle) * scene.get_triangles().size(), cudaMemcpyHostToDevice);
    cudaMemcpy(materials, scene.get_materials().data(),  sizeof(GpuMaterial) * scene.get_materials().size(), cudaMemcpyHostToDevice);
    cudaMemcpy(light_indices, scene.get_light_indices().data(), sizeof(int) * scene.get_light_indices().size(), cudaMemcpyHostToDevice);

    meshes_count = scene.get_meshes().size();
    if (meshes_count > 0) {
        cudaMalloc(&meshes, sizeof(GpuMeshData) * meshes_count);
        cudaMemcpy(meshes, scene.get_meshes().data(),
                   sizeof(GpuMeshData) * meshes_count,
                   cudaMemcpyHostToDevice);
    }
}
std::vector<GpuMaterial> HostScene::get_materials() const {
    std::vector<GpuMaterial> flattend_materials(materials.size());

    for (const auto& kv : materials){
        flattend_materials[kv.second] = *kv.first;
    }

    return flattend_materials;
}

void HostScene::add_sphere(GpuSphere sphere, const GpuMaterial* material){
    

    auto kv = materials.find(material);

    if (kv != materials.end()){
        sphere.material_index = kv -> second;
    }
    else{
        sphere.material_index = materials.size();
        materials[material] = materials.size();
    }
     

    if (material -> get_type() == GpuMaterialType::EMISSIVE){
        light_indices.push_back(objects.size());
    }


    GpuObject object = {GpuObjectType::SPHERE, spheres.size()};
    spheres.push_back(sphere);
    objects.push_back(object);
}


void HostScene::add_triangle(GpuTriangle triangle, const GpuMaterial* material){
    

    auto kv = materials.find(material);

    if (kv != materials.end()){
        triangle.material_index = kv -> second;
    }
    else{
        triangle.material_index = materials.size();
        materials[material] = materials.size();
    }
     

    if (material -> get_type() == GpuMaterialType::EMISSIVE){
        light_indices.push_back(objects.size());
    }


    GpuObject object = {GpuObjectType::TRIANGLE, triangles.size()};
    triangles.push_back(triangle);
    objects.push_back(object);
}


void HostScene::add_mesh(HostMesh mesh, const GpuMaterial* override_mat){

    // Add all triangles of mesh to scene triangels
    // Track how many triangles and where the triangles start
    
    size_t tri_start = triangles.size();
    size_t tri_count = mesh.get_triangles().size();
    const std::vector<GpuTriangle>& mesh_triangles = mesh.get_triangles();
    const std::vector<const GpuMaterial*>& mesh_materials = mesh.get_materials();
    for (size_t i = 0; i < tri_count; ++i){
        const GpuMaterial* material = mesh_materials[i];
        const GpuMaterial* selected_material = override_mat;
        if (material != nullptr && material->get_type() != GpuMaterialType::NONE) {
            selected_material = material;
        }
        add_triangle(mesh_triangles[i], selected_material);
    }


    GpuObject object = {GpuObjectType::MESH, meshes.size()};
    meshes.push_back(GpuMeshData{tri_start, tri_count});
    objects.push_back(object);
}


}
