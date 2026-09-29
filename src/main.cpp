
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include "Sphere.h"
#include "Triangle.h"
#include "Mesh.h"
#include "Vector.h"
#include "Hittable.h"
#include "Scene.h"
#include "Camera.h"
#include "Renderer.h"
#include "PPM.h"
#include "Viewport.h"
#include "ImageBuffer.h"
#include "Material.h"
#ifdef HAS_CUDA
#include "cuda_kernels.h"
#include "gpu_scene.h"
#include "CudaRenderer.h"

#endif
#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 780


int main(int argc, char** argv) {
  
    bool ppm_mode = false;
    bool cuda_test = false;
    std::string ppm_filename = "check Oh..ppm";
    for (int i = 1; i < argc; i++){
        std::string arg = argv[i];
        if (arg == "ppm"){
            ppm_mode = true;
        }
        if (arg == "cuda"){
            cuda_test = true;
        }
    }

  
    rt::Vec3 global_up = rt::Vec3(0, 1, 0);
    rt::Camera camera = rt::Camera();
    rt::Renderer rt_renderer = rt::Renderer(1, 1);
    rt::Scene world = rt::Scene();
    rt::Vec3 forward = camera.get_forward_vector();
    rt::Vec3 right   = camera.get_right_vector();
    rt::Vec3 up      = camera.get_up_vector();
    
    std::shared_ptr<rt::Material> diffuse_red = std::make_shared<rt::Diffuse>(rt::Vec3(1.0, 0, 0));
    std::shared_ptr<rt::Material> diffuse_blue = std::make_shared<rt::Diffuse>(rt::Vec3(0, 0, 1.0));
    std::shared_ptr<rt::Material> diffuse_green= std::make_shared<rt::Diffuse>(rt::Vec3(0, 1.0, 0));
    std::shared_ptr<rt::Material> diffuse_yellow = std::make_shared<rt::Diffuse>(rt::Vec3(0.5, 0.5, 0));
    std::shared_ptr<rt::Material> diffuse_teal = std::make_shared<rt::Diffuse>(rt::Vec3(0.0, 0.5, 0.5));
    std::shared_ptr<rt::Material> metal_gold = std::make_shared<rt::Metal>(rt::Vec3(0.8, 0.6, 0.2), 0);
    std::shared_ptr<rt::Material> soap =std::make_shared<rt::Dielectric>(-0.9);    
    std::shared_ptr<rt::Material> glass = std::make_shared<rt::Dielectric>(1.6);    
    std::shared_ptr<rt::Material> glass_2= std::make_shared<rt::Dielectric>(1.8);    
    std::shared_ptr<rt::Material> light = std::make_shared<rt::Emissive>(rt::Vec3(1.0, 1.0, 1.0), 10);
    


 
    world.add_hittable(std::make_unique<rt::Sphere>(rt::Vec3(1, 0.5, -3), 0.5, diffuse_green));
    // world.add_hittable(std::make_unique<rt::Sphere>(rt::Vec3(-1, -0.5, -3), 0.5, glass));
    // world.add_hittable(std::make_unique<rt::Sphere>(rt::Vec3(-2, 0, -2), 1.5, glass_2));
    world.add_hittable(std::make_unique<rt::Sphere>(rt::Vec3(3, 0.5, -6), 0.8, metal_gold));
    world.add_hittable(std::make_unique<rt::Sphere>(rt::Vec3(0, -200, -5), 199, diffuse_teal));
    world.add_hittable(std::make_unique<rt::Sphere>(rt::Vec3(0, -1, -3), 0.9, diffuse_blue));
    world.add_hittable(std::make_unique<rt::Sphere>(rt::Vec3(1, -0.5, -3), 0.5, glass));
    world.add_hittable(std::make_unique<rt::Sphere>(rt::Vec3(0, 1, -1), 0.8, light));
  
    
    std::vector<std::unique_ptr<rt::Triangle>> faces;
    faces.reserve(12);

    // 8 Corner Vertices
    rt::Vec3 p0(-3.3f, -1.0f, -3.7f); // Front-bottom-left
    rt::Vec3 p1(-1.7f, -1.0f, -3.7f); // Front-bottom-right
    rt::Vec3 p2(-1.7f,  0.6f, -3.7f); // Front-top-right
    rt::Vec3 p3(-3.3f,  0.6f, -3.7f); // Front-top-left
    rt::Vec3 p4(-3.3f, -1.0f, -5.3f); // Back-bottom-left
    rt::Vec3 p5(-1.7f, -1.0f, -5.3f); // Back-bottom-right
    rt::Vec3 p6(-1.7f,  0.6f, -5.3f); // Back-top-right
    rt::Vec3 p7(-3.3f,  0.6f, -5.3f); // Back-top-left

    // Front Face (+Z)
    faces.push_back(std::make_unique<rt::Triangle>(p0, p1, p2, nullptr));
    faces.push_back(std::make_unique<rt::Triangle>(p0, p2, p3, nullptr));

    // Right Face (+X)
    faces.push_back(std::make_unique<rt::Triangle>(p1, p5, p6, glass));
    faces.push_back(std::make_unique<rt::Triangle>(p1, p6, p2, glass));

    // Back Face (-Z)
    faces.push_back(std::make_unique<rt::Triangle>(p5, p4, p7, nullptr));
    faces.push_back(std::make_unique<rt::Triangle>(p5, p7, p6, nullptr));

    // Left Face (-X)
    faces.push_back(std::make_unique<rt::Triangle>(p4, p0, p3, glass));
    faces.push_back(std::make_unique<rt::Triangle>(p4, p3, p7, glass));

    // Top Face (+Y)
    faces.push_back(std::make_unique<rt::Triangle>(p3, p2, p6, nullptr));
    faces.push_back(std::make_unique<rt::Triangle>(p3, p6, p7, nullptr));

    // Bottom Face (-Y)
    faces.push_back(std::make_unique<rt::Triangle>(p4, p5, p1, nullptr));
    faces.push_back(std::make_unique<rt::Triangle>(p4, p1, p0, nullptr));

    // Move faces into Mesh
    world.add_hittable(std::make_unique<rt::Mesh>(std::move(faces), diffuse_teal));












    rt::Viewport viewport = rt::Viewport(0.8, WINDOW_WIDTH, WINDOW_HEIGHT);
    rt::ImageBuffer pixels = rt::ImageBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);

    float aspect_ratio = static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT;
  
    float viewport_height = 0.8;
    float viewport_width = viewport_height * aspect_ratio;
    float viewport_depth = 1;

    rt::Vec3 top_left = rt::Vec3(-viewport_width/2, viewport_height/2, -1);
    
    float viewport_dy = viewport_height/WINDOW_HEIGHT;
    float viewport_dx = viewport_width/WINDOW_WIDTH;
    

    if (ppm_mode){ 
        rt_renderer.render_frame(world, camera, viewport, pixels);
        output_ppm(ppm_filename, pixels.get_pixels(), WINDOW_WIDTH, WINDOW_HEIGHT);
        return 0;
    }
#ifdef HAS_CUDA
    else if (cuda_test){
        ppm_filename = "cuda_test.ppm";
        std::vector<rt::GpuSphere> spheres;
        spheres.push_back(rt::GpuSphere{rt::Vec3(1, 0.5, -2),rt::Vec3(0, 0, 1), 1.0f, 0});
        spheres.push_back(rt::GpuSphere{rt::Vec3(1, -200, -2),rt::Vec3(1, 1, 1), 199.0f, 0});
        spheres.push_back(rt::GpuSphere{rt::Vec3(-10, 3, -6),rt::Vec3(1, 0, 1), 2.0f, 0});
        spheres.push_back(rt::GpuSphere{rt::Vec3(10, -2, 8),rt::Vec3(0, 1, 1), 6.0f, 0});
        rt::GpuScene scene;

        scene.spheres = spheres.data();
        scene.spheres_count = spheres.size();

        Color* host_pixels = (Color*)malloc(sizeof(Color) * WINDOW_HEIGHT * WINDOW_WIDTH);
        
        rt::CudaRenderer renderer;
        rt::KernelData kernel_data;

        renderer.initialize(&scene, WINDOW_WIDTH, WINDOW_HEIGHT);
        
        InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "raylib example - basic window");
        Image canvas = GenImageColor(WINDOW_WIDTH, WINDOW_HEIGHT, BLACK);
        Texture2D texture = LoadTextureFromImage(canvas);
        UnloadImage(canvas);
        DisableCursor();
    
        SetTargetFPS(1000);
        float dt;
        while (!WindowShouldClose())
        {  
          
            dt = 1.0 / GetFPS();
            Vector2 mouse_delta = GetMouseDelta();
            float length = Vector2Length(mouse_delta);
            if (length > 0){
                camera.update_pitch_yaw(-2.0 * dt * mouse_delta.y/length, 2.0 * dt * mouse_delta.x/length);
            }
            
            rt::Vec3 forward = camera.get_forward_vector();
            rt::Vec3 right   = camera.get_right_vector();
            rt::Vec3 up      = camera.get_up_vector();


            if (IsKeyDown(KEY_RIGHT)){
                camera.update_pos(right.x * dt * 1.50, right.y * dt * 1.50, right.z * dt * 1.50);   
            }

            if (IsKeyDown(KEY_LEFT)){
                camera.update_pos(-right.x * dt * 1.50, -right.y * dt * 1.50, -right.z * dt * 1.50);
                
            }
            if (IsKeyDown(KEY_UP)){
                camera.update_pos(forward.x * dt * 1.50, forward.y * dt * 1.50, forward.z * dt * 1.50);   
            }

            if (IsKeyDown(KEY_DOWN)){
                camera.update_pos(-forward.x * dt * 1.50, -forward.y * dt * 1.50, -forward.z * dt * 1.50);
                
            }

            if (IsKeyPressed(KEY_P)){
                std::cout << "Saving output";
                output_ppm(ppm_filename, pixels.get_pixels(), WINDOW_WIDTH, WINDOW_HEIGHT);
            }

            
            kernel_data.forward_vector = camera.get_forward_vector();
            kernel_data.up_vector = camera.get_up_vector();
            kernel_data.right_vector = camera.get_right_vector();
            kernel_data.viewport_depth = viewport.get_viewport_depth();
            kernel_data.viewport_dx = viewport.get_viewport_dx();
            kernel_data.viewport_dy = viewport.get_viewport_dy();
            kernel_data.position = camera.get_position();
            renderer.render(kernel_data, host_pixels);
            
            UpdateTexture(texture, host_pixels);
            BeginDrawing();
                ClearBackground(BLACK);
                DrawTexturePro(texture, {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}, {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}, {0,0}, 0.0f, WHITE);
                DrawFPS(0, 0);
            EndDrawing();
        }

        CloseWindow();
        // Close the file
        free(host_pixels);
        renderer.shutdown();
        

        return 0;
    }
#endif
    else{


        InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "raylib example - basic window");
        Image canvas = GenImageColor(WINDOW_WIDTH, WINDOW_HEIGHT, BLACK);
        Texture2D texture = LoadTextureFromImage(canvas);
        UnloadImage(canvas);
        DisableCursor();
    
        SetTargetFPS(120);
        while (!WindowShouldClose())
        {  
            Vector2 mouse_delta = GetMouseDelta();
            float length = Vector2Length(mouse_delta);
            if (length > 0){
                camera.update_pitch_yaw(-1.50 * mouse_delta.y/length, 1.50 * mouse_delta.x/length);
            }

        
            rt::Vec3 forward = camera.get_forward_vector();
            rt::Vec3 right   = camera.get_right_vector();
            rt::Vec3 up      = camera.get_up_vector();


            if (IsKeyDown(KEY_RIGHT)){
                camera.update_pos(right.x * 1.50, right.y * 1.50, right.z * 1.50);   
            }

            if (IsKeyDown(KEY_LEFT)){
                camera.update_pos(-right.x * 1.50, -right.y * 1.50, -right.z * 1.50);
                
            }
            if (IsKeyDown(KEY_UP)){
                camera.update_pos(forward.x * 1.50, forward.y * 1.50, forward.z * 1.50);   
            }

            if (IsKeyDown(KEY_DOWN)){
                camera.update_pos(-forward.x * 1.50, -forward.y * 1.50, -forward.z * 1.50);
                
            }

            if (IsKeyPressed(KEY_P)){
                std::cout << "Saving output";
                output_ppm(ppm_filename, pixels.get_pixels(), WINDOW_WIDTH, WINDOW_HEIGHT);
            }

            top_left = camera.get_position() + forward * viewport_depth + up * (viewport_height/2) - right * (viewport_width/2);

            rt_renderer.render_frame(world, camera, viewport, pixels);
            
        

            UpdateTexture(texture, pixels.get_pixels().data());
            BeginDrawing();
                ClearBackground(BLACK);
                DrawTexturePro(texture, {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}, {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}, {0,0}, 0.0f, WHITE);
                DrawFPS(0, 0);
            EndDrawing();
        }

        CloseWindow();
        // Close the file
    

        return 0;
    }
}
