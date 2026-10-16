#pragma once

#include "Vector.h"
#include <unordered_map>
#include <curand_kernel.h>
#include <vector>
#include <raylib.h>
#include "cuda_compat.h"

namespace rt {
class GpuMaterial;

enum class GpuObjectType {
    SPHERE,
    TRIANGLE,
    MESH
};


struct GpuObject {
    GpuObjectType type;
    size_t index;
};

// Seperate material and material index, so eventually materials can be reused
struct GpuSphere {
    Vec3 position;
    Color color;
    float radius;
    int material_index;
    
    DEVICE Vec3 sample_point(curandStatePhilox4_32_10_t& state) const;
};

struct GpuTriangle {
    Vec3 v1, v2, v3;
    int material_index;

    DEVICE Vec3 sample_point(curandStatePhilox4_32_10_t& state) const;
};

struct GpuHitRecord {
    Vec3 normal;
    Color color;
    int mat_index;
    float time;

    DEVICE GpuHitRecord(){
        normal = Vec3();
        time = -1.0f;

    }
};

DEVICE void hit_sphere(GpuHitRecord& record, const GpuSphere& sphere, const Ray& ray);
DEVICE void hit_triangle(GpuHitRecord& record, const GpuTriangle& triangle, const Ray& ray);
DEVICE ::Color raylib_color_from_Vec3(Vec3 vec);



// Read only view
struct GpuView {
    const GpuSphere* spheres;
    size_t spheres_count;

    const GpuTriangle* triangles = nullptr;
    size_t triangles_count;


    const GpuObject* objects;
    size_t objects_count;

    const GpuMaterial* materials;
    size_t materials_count;

    const int* light_indices;
    size_t light_count;

    DEVICE bool ray_hit(const Ray& ray, GpuHitRecord& record) const;
    DEVICE Color calculate_direct_lighting(const Vec3& pos, const Vec3& surface_normal, curandStatePhilox4_32_10_t& state) const;
};


// Easier to use scene for adding objects
class HostScene {
    public:
        HostScene() = default;
        
        void add_sphere(GpuSphere sphere, GpuMaterial* material);
        void add_triangle(GpuTriangle triangle, GpuMaterial* material);

        const std::vector<GpuSphere>& get_spheres() const {
            return spheres;
        }

        const std::vector<GpuTriangle>get_triangles() const {
            return triangles;
        }

        const std::vector<GpuObject>& get_objects() const {
            return objects;
        }

        std::vector<GpuMaterial> get_materials() const;

        const std::vector<int>& get_light_indices() const {
            return light_indices;
        }
    private:
    
        // Maintain internal list of all objects for when moving to GPU
        std::vector<GpuSphere> spheres;
        std::vector<GpuTriangle> triangles;

        // Maintain a map so that materials can be shared
        std::unordered_map<GpuMaterial*, int> materials;

        std::vector<GpuObject> objects;
        std::vector<int> light_indices;
};

class DeviceScene {
    public:
        ~DeviceScene();
        GpuView view() const;
        void upload_scene(const HostScene& scene);

    private:
        GpuMaterial* materials = nullptr;
        size_t materials_count;

        GpuSphere* spheres = nullptr;
        size_t spheres_count;

        GpuTriangle* triangles = nullptr;
        size_t triangles_count;

        GpuObject* objects = nullptr;
        size_t objects_count;

        int* light_indices = nullptr;
        size_t light_count;

      
};


}
