
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <memory>
#include "sphere.h"
#include "vector.h"
#include "hittable.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 360


int main() {   

    std::ofstream outFile("output.ppm");

    if (!outFile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

    outFile << "P3\n" << WINDOW_WIDTH << "\n" << WINDOW_HEIGHT << "\n255\n";

    float aspect_ratio = static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT;
  
    float viewport_height = 2.0;
    float viewport_width = 2.0 * aspect_ratio;
    float viewport_depth = -1;

    Vec3 top_left = Vec3(-viewport_width/2, viewport_height/2, -1);
    
    float viewport_dy = viewport_height/WINDOW_HEIGHT;
    float viewport_dx = viewport_width/WINDOW_WIDTH;
    int rays_hit = 0;

    std::vector<std::unique_ptr<Hittable>> hit_list;

    hit_list.push_back(std::make_unique<Sphere>(Vec3(0, 1, -1), Vec3(255, 0, 0), 1.2));
    hit_list.push_back(std::make_unique<Sphere>(Vec3(0, 1, -3), Vec3(0, 255, 0), 2));
    hit_list.push_back(std::make_unique<Sphere>(Vec3(0, -200.5, 30), Vec3(100, 100, 100), 200));

    for (int y = 0; y < WINDOW_HEIGHT; y++){
        for (int x = 0; x < WINDOW_WIDTH; x++){
           
            HitRecord record = {-1, Vec3(), Vec3()};
            Vec3 direction = (top_left + Vec3(x * viewport_dx, -y * viewport_dy, viewport_depth)).normalize();
           
            Vec3 origin = Vec3(0.0f, 0.0f, 0.0f);

            Ray camera_ray = Ray(origin, direction);
            
            for (auto i = hit_list.begin(); i != hit_list.end(); ++i){
                (*i) -> hit(camera_ray, record);
            }
            if (record.time == -1){
                outFile << rgb_map(camera_ray.ray_color().x) << " " << rgb_map(camera_ray.ray_color().y)  <<  " " << rgb_map(camera_ray.ray_color().z) << "\n";
            }
            else{
                outFile << (int)(record.color.x) << " " << (int)(record.color.y) << " " << int(record.color.z) << "\n"; 
            }
        }
    }

    // Close the file
    outFile.close();

    std::cout << "Image successfully saved to output.ppm!\n" ;

    return 0;
}