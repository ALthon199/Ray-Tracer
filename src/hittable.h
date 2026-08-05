#pragma once

#include "vector.h"
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

}

