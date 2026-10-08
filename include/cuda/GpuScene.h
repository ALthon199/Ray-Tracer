#pragma once

#include "../Vector.h"
#include <vector>
#include <raylib.h>
#include "cuda_compat.h"

namespace rt {

enum class GpuObjectType {
    SPHERE,
    TRIANGLE,
    MESH
};

struct GpuObject {
    GpuObjectType type;
    size_t index;
};

struct GpuSphere {
    Vec3 position;
    Color color;
    float radius;
    int material_index;
};

struct GpuHitRecord {
    Vec3 normal;
    Color color;
    float time;

    DEVICE GpuHitRecord(){
        normal = Vec3();
        time = -1.0f;
    }
};

DEVICE void hit_sphere(GpuHitRecord& record, const GpuSphere& sphere, const Ray& ray);
DEVICE ::Color raylib_color_from_Vec3(Vec3 vec);
struct GpuTriangle {};
struct GpuMaterial {};



// Scene ready to use for GPU
struct GpuScene {
    const GpuSphere* spheres;
    size_t spheres_count;

    const GpuObject* objects;
    size_t objects_count;

    GpuSphere* write_spheres;
    GpuObject* write_objects;
};

// Easier to use scene for adding objects
class HostScene {
    public:
        HostScene() = default;
        void update_gpu_scene(GpuScene& scene) const;
        void add_sphere(GpuSphere sphere);
    
    private:
        std::vector<GpuSphere> spheres;

        // Maintain internal list of all objects for when moving to GPU
        std::vector<GpuObject> objects;
};

}
