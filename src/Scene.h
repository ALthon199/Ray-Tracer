#pragma once
#include "Hittable.h"
#include "Vector.h"
#include "Light.h"
namespace rt {

class Scene {
    public:
        void add_hittable(std::unique_ptr<Hittable> object);
        void add_light(std::unique_ptr<Light> light);
        const std::vector<std::unique_ptr<Hittable>>& get_hit_list() const;
        bool ray_hit(const Ray& ray, HitRecord& record) const; 
        Color direct_lighting(const Vec3& pos, const Vec3& surface_normal) const; 
       
    private:
        std::vector<std::unique_ptr<Hittable>> hit_list;
        std::vector<Light*> light_list;
};

}