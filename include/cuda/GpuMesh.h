#pragma once
#include "Vector.h"
#include "cuda/cuda_compat.h"
#include "cuda/GpuPrimitives.h"
#include <curand_kernel.h>
#include <vector>


namespace rt {
class GpuMaterial;

// Mesh data which can be easily copied to GPU data when upload scene happened
struct GpuMeshData {
    size_t tri_start;
    size_t tri_count;
};

// Easy to use host for adding triangles and meshes
class HostMesh{
    public:
        void add_triangle(const GpuTriangle& tri, const GpuMaterial* material){
            faces.push_back(tri);
            materials.push_back(material);
        }
        const std::vector<GpuTriangle>& get_triangles() const {
            return faces;
        }
        const std::vector<const GpuMaterial*>& get_materials() const {
            return materials;
        }
    private:
        std::vector<const GpuMaterial*> materials;
        std::vector<GpuTriangle> faces;
};



}

