#include "Renderer.h"
#include "Hittable.h"
#include "Scene.h"
#include "Viewport.h"
#include "ImageBuffer.h"
#include "Camera.h"
#include "Vector.h"
#include "Utility.h"
#include <raylib.h>

namespace rt{
    Color Renderer::calculate_color(const Ray& ray, const Scene& world, int bounces, bool is_primary) const{
        if (bounces <= 0) return Color();

        HitRecord record = HitRecord();
        ScatterRecord s_record = ScatterRecord{Vec3(), Ray(Vec3(), Vec3())};
        bool hit = world.ray_hit(ray, record);
        if (!hit) return Color(0.8, 0.8, 0.8);
        
        Color direct_lighting;
        Color indirect_lighting;
        bool scattered;
        Vec3 hit_pos = ray.ray_at(record.time);

        switch(record.material -> get_type()){
            case MaterialType::EMISSIVE:
                return is_primary ? record.material -> emitted() : Color();
        
            case MaterialType::DIFFUSE:          
                direct_lighting = world.direct_lighting(hit_pos + record.normal * 0.001f, record.normal);
                scattered = record.material -> scatter(ray, record, s_record);
                indirect_lighting = calculate_color(s_record.scattered_ray, world, bounces - 1, false);
                break;

            case MaterialType::METAL:
                direct_lighting = Vec3();
                scattered = record.material -> scatter(ray, record, s_record);
                if (!scattered){
                    return Vec3();
                }
                indirect_lighting = calculate_color(s_record.scattered_ray, world, bounces - 1, true);
                break;
        }
        
       
        // std::cout << "INDIRECT" << indirect_lighting.x << " " << indirect_lighting.y << " " << indirect_lighting.z <<"\n";
        Color total_lighting = indirect_lighting + direct_lighting;
        return s_record.attentuation * total_lighting;
        
  
    }

    void Renderer::render_frame(const Scene& world, const Camera& camera, const Viewport& viewport, ImageBuffer& pixels) const{
       
        Vec3 forward = camera.get_forward_vector();
        Vec3 right = camera.get_right_vector();
        Vec3 up = camera.get_up_vector();
        int width = pixels.get_width();
        int height = pixels.get_height();
        
        
        float dx = viewport.get_viewport_dx();
        float dy = viewport.get_viewport_dy();
        float viewport_depth = viewport.get_viewport_depth();
        float viewport_height = viewport.get_viewport_height();
        float viewport_width = viewport.get_viewport_width();

        float scale = 1.0 / ssp;

        Vec3 top_left = camera.get_position() + forward * viewport_depth + up * (viewport_height/2) - right * (viewport_width/2);
        for (int y = 0; y < height; y++){
            for (int x = 0; x < width; x++){
                Color pixel = Color(0, 0, 0);
                for (int sam = 0; sam < ssp; sam++){   
                    float pixel_x = x + random_num(0.0f, 1.0f);
                    float pixel_y = y + random_num(0.0f, 1.0f);
                    Vec3 target = top_left + right * pixel_x * dx - up * pixel_y * dy;
                    Vec3 direction = target - camera.get_position();
                    Ray sample = Ray(camera.get_position(), direction.normalize());
                        
                    Color sample_color = calculate_color(sample, world, max_bounces, true);
                   
                    pixel += sample_color;  
            
                }
               
                pixel *= scale;
                color_clamp(pixel, 0.0f, 1.0f);
                pixels.set_pixel(y, x, pixel);
            }
        }
    
    }
}

