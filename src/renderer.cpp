#include "renderer.h"
#include "hittable.h"
#include "camera.h"
#include "vector.h"


namespace rt{
    Color Renderer::pixel_color(const Ray& ray, const HittableList& world, const Camera& camera){
        HitRecord record = {-1, Vec3(), Vec3()};
        const std::vector<std::unique_ptr<Hittable>>& hit_list = world.get_hit_list();
        for (auto i = hit_list.begin(); i != hit_list.end(); ++i){        
            (*i) -> hit(ray, record);
        }

        float r;
        float g;
        float b;

        if (record.time == -1){
            r = rt::rgb_map(ray.ray_color().x);
            g = rt::rgb_map(ray.ray_color().y);
            b = rt::rgb_map(ray.ray_color().z);
       
        }
        else{
            r = static_cast<float>(record.color.x);
            g =  static_cast<float>(record.color.y);
            b = static_cast<float>(record.color.z);
         
        }
        
        Color pixel = Color(r, g, b);
     
        return pixel;
    }
}


