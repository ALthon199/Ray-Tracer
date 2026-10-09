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


// Read only view
struct GpuView{
    const GpuSphere* spheres;
    size_t spheres_count;

    const GpuObject* objects;
    size_t objects_count;
};


// Easier to use scene for adding objects
class HostScene {
    public:
        HostScene() = default;
        
        void add_sphere(GpuSphere sphere);
        const std::vector<GpuSphere>& get_spheres() const {
            return spheres;
        }
        const std::vector<GpuObject>& get_objects() const {
            return objects;
        }
    private:
        std::vector<GpuSphere> spheres;
        // Maintain internal list of all objects for when moving to GPU
        std::vector<GpuObject> objects;
};

class DeviceScene {
    public:
        ~DeviceScene();
        GpuView view() const;
        void upload_scene(const HostScene& scene);

    private:
        GpuSphere* spheres = nullptr;
        size_t spheres_count;

        GpuObject* objects = nullptr;
        size_t objects_count;
        
       
};


}
