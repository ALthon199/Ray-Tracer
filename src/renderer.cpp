#include "Renderer.h"
#include "Hittable.h"
#include "Viewport.h"
#include "ImageBuffer.h"
#include "Camera.h"
#include "Vector.h"
#include "Utility.h"
#include <raylib.h>

namespace rt{
    void Renderer::render_frame(const HittableList& world, const Camera& camera, const Viewport& viewport, ImageBuffer& pixels) const{
       
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
                    float pixel_x = x + (sam + 0.5f)/ssp;
                    float pixel_y = y + (sam + 0.5f)/ssp;
                    Vec3 target = top_left + right * pixel_x * dx - up * pixel_y * dy;
                    Vec3 direction = target - camera.get_position();
                    Ray sample = Ray(camera.get_position(), direction.normalize());
                    
                
                    
                    Color sample_color = world.calculate_color(sample, max_bounces);
                   
                    pixel += sample_color;  
            
                }
               
                pixel *= scale;
                pixels.set_pixel(y, x, pixel);
            }
        }
    
    }
}

