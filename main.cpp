#include <iostream>
#include <fstream>
#include <cmath>
#include "sphere.h"
#include "vector.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720


int main() {   


    std::ofstream outFile("output.ppm");

    if (!outFile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

    
    outFile << "P3\n" << WINDOW_WIDTH << "\n" << WINDOW_HEIGHT << "\n255\n";

    float aspect_ratio = static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT;
  
    int rays_hit = 0;
    Sphere test = Sphere(Vec3(0, 5, 10), Vec3(255, 0, 0), 6);
    for (int y = 0; y < WINDOW_HEIGHT; y++){
        for (int x = 0; x < WINDOW_WIDTH; x++){
            float ray_x = aspect_ratio * ((static_cast<float>(x) / WINDOW_WIDTH) * 2 - 1);
            float ray_y = 1 - 2 * (static_cast<float>(y) / WINDOW_HEIGHT);
            float ray_z = 1;
            
            Vec3 direction = Vec3(ray_x, ray_y, ray_z).normalize();
            Vec3 origin = Vec3(0.0f, 0.0f, 0.0f);

            Ray camera_ray = Ray(origin, direction);

            float t = test.sphere_hit(camera_ray);

            if (t == -1){
                outFile << 0 << " " << 0 << " " << 0 << "\n";
            }
            else{
                rays_hit += 1;
                Vec3 intersection = camera_ray.ray_at(t);
                Vec3 normal =  intersection - test.get_origin();
             
                normal.normalize();

                int r = ((normal.x + 1) / 2) * 255;
                int g = ((normal.y + 1) / 2) * 255;
                int b = ((normal.z + 1) / 2) * 255;
                if (rays_hit % 50 == 0){
                    std::cout << normal.x << + " "<< normal.y << " " << normal.z << "\n";
                }
                outFile << r << " " << g << " " << b << "\n";

            }
           

            



            
        }
    }

    // Close the file
    outFile.close();

    std::cout << "Image successfully saved to output.ppm!\n" << rays_hit;

    return 0;
}