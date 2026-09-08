#include "Mesh.h"
#include "Utility.h"

namespace rt {

void Mesh::hit(const Ray& ray, HitRecord& record) const {
    for (int i = 0; i < faces.size(); i++){
        if (faces[i] -> get_material() == nullptr) {
            faces[i] -> hit(ray, record, material);
        }
        else {
            faces[i] -> hit(ray, record);
        }
    }
}

Vec3 Mesh::sample_point() const {
    size_t num = static_cast<size_t>(random_num(0.0f, faces.size() - 1));
    return faces[num] -> sample_point();
}

}