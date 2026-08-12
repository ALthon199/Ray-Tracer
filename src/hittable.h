#pragma once

#include "Vector.h"
#include "Utility.h"
#include <memory>
namespace rt {

struct HitRecord {  
    float time;
    Vec3 normal;
    Vec3 color;
};

class Hittable {
    public:
        virtual ~Hittable() = default;
        virtual void hit(const Ray& ray, HitRecord& record) const = 0;
};

class HittableList {
    public:
        void add_hittable(std::unique_ptr<Hittable> object);
        Color calculate_color(const Ray& ray, int bounces) const;
        const std::vector<std::unique_ptr<Hittable>>& get_hit_list() const;

    private:
        std::vector<std::unique_ptr<Hittable>> hit_list;
};


inline void HittableList::add_hittable(std::unique_ptr<Hittable> object){
    hit_list.push_back(std::move(object));
}

inline const std::vector<std::unique_ptr<Hittable>>& HittableList::get_hit_list() const{
    return hit_list;
}

inline Color HittableList::calculate_color(const Ray& ray, int bounces) const{
    HitRecord record = HitRecord{-1.0f, Vec3(), Vec3()};
    if (bounces <= 0){
        return Vec3(0, 0, 0);
    }

    for (auto i = hit_list.begin(); i != hit_list.end(); ++i){        
        (*i) -> hit(ray, record);
    }

    if (record.time <= 0.0001f){
        return ray.ray_base_color();
    }
    else{

        Vec3 new_dir = random_unit_vec();
        if (new_dir.dot(record.normal) < 0.0f){
            new_dir *= -1;
        }
        Vec3 new_pos = ray.ray_at(record.time) + new_dir * 0.001f;
        Ray new_ray = Ray(new_pos, new_dir);
        Color x = record.color * calculate_color(new_ray, bounces - 1);
        // std::cout << x.x << " " << x.y << " " << x.z << "\n";
        return x;
    }
}

}

