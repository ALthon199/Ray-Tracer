
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include <raylib.h>
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
#include "cuda/cuda_kernels.h"
#include "cuda/GpuScene.h"
#include "cuda/GpuMaterial.h"
#include "cuda/CudaRenderer.h"
#include "cuda/GpuPrimitives.h"
#include "cuda/GpuMesh.h"
#endif
#include <stdlib.h>
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 780
#define MOUSE_SENSITIVITY 0.0025f


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
    

    if (ppm_mode && !cuda_test){ 
        rt_renderer.render_frame(world, camera, viewport, pixels);
        output_ppm(ppm_filename, pixels.get_pixels(), WINDOW_WIDTH, WINDOW_HEIGHT);
        return 0;
    }
#ifdef HAS_CUDA
    else if (cuda_test){
        
        ppm_filename = "cuda_test.ppm";
        rt::HostScene host_scene;

        // GPU materials are value types. Build each material's union payload,
        // then attach the material when adding the corresponding sphere.
        rt::GpuMaterialData green_data{};
        green_data.diffuse = rt::GpuDiffuse{rt::Vec3(0.0f, 0.8f, 0.1f)};
        rt::GpuMaterial green(rt::GpuMaterialType::DIFFUSE, green_data);

        rt::GpuMaterialData ground_data{};
        ground_data.diffuse = rt::GpuDiffuse{rt::Vec3(0.7f, 0.7f, 0.7f)};
        rt::GpuMaterial ground(rt::GpuMaterialType::DIFFUSE, ground_data);

        rt::GpuMaterialData metal_data{};
        metal_data.metal = rt::GpuMetal{rt::Vec3(0.8f, 0.6f, 0.2f), 0.0f};
        rt::GpuMaterial metal(rt::GpuMaterialType::METAL, metal_data);

        rt::GpuMaterialData glass_data{};
        glass_data.dielectric = rt::GpuDielectric{1.5f};
        rt::GpuMaterial glass(rt::GpuMaterialType::DIELECTRIC, glass_data);

        rt::GpuMaterialData light_data{};
        light_data.emissive = rt::GpuEmissive{rt::Vec3(1.0f, 1.0f, 1.0f), 1.0f};
        rt::GpuMaterial light(rt::GpuMaterialType::EMISSIVE, light_data);

        rt::GpuMaterialData triangle_data{};
        triangle_data.diffuse = rt::GpuDiffuse{rt::Vec3(0.05f, 0.1f, 0.8f)};
        rt::GpuMaterial triangle_material(
            rt::GpuMaterialType::DIFFUSE,
            triangle_data
        );

        rt::GpuMaterialData none_data{};
        rt::GpuMaterial none_material(rt::GpuMaterialType::NONE, none_data);

        host_scene.add_sphere(
            rt::GpuSphere{rt::Vec3(3.0f, 0.0f, -6.0f), rt::Vec3(), 1.0f, 0},
            &green);
        host_scene.add_sphere(
            rt::GpuSphere{rt::Vec3(0.0f, -200.0f, -6.0f), rt::Vec3(), 199.0f, 0},
            &ground);
        host_scene.add_sphere(
            rt::GpuSphere{rt::Vec3(-3.0f, 0.0f, -6.0f), rt::Vec3(), 1.0f, 0},
            &metal);
        host_scene.add_sphere(
            rt::GpuSphere{rt::Vec3(0.0f, 0.0f, -6.0f), rt::Vec3(), 1.0f, 0},
            &glass);
        host_scene.add_sphere(
            rt::GpuSphere{rt::Vec3(0.0f, 4.0f, -4.0f), rt::Vec3(), 0.75f, 0},
            &light);

        // A cube mesh behind the glass: six faces, two triangles per face.
        rt::HostMesh cube_mesh;
        const rt::Vec3 c(0.0f, 0.0f, -10.0f);
        const float h = 1.25f;
        const rt::Vec3 p000(c.x - h, c.y - h, c.z - h);
        const rt::Vec3 p001(c.x - h, c.y - h, c.z + h);
        const rt::Vec3 p010(c.x - h, c.y + h, c.z - h);
        const rt::Vec3 p011(c.x - h, c.y + h, c.z + h);
        const rt::Vec3 p100(c.x + h, c.y - h, c.z - h);
        const rt::Vec3 p101(c.x + h, c.y - h, c.z + h);
        const rt::Vec3 p110(c.x + h, c.y + h, c.z - h);
        const rt::Vec3 p111(c.x + h, c.y + h, c.z + h);

        auto add_quad = [&](const rt::Vec3& a, const rt::Vec3& b,
                            const rt::Vec3& d, const rt::Vec3& e,
                            const rt::GpuMaterial* material) {
            cube_mesh.add_triangle(rt::GpuTriangle{a, b, d, 0}, material);
            cube_mesh.add_triangle(rt::GpuTriangle{a, d, e, 0}, material);
        };

        add_quad(p000, p100, p110, p010, &metal);           // back
        add_quad(p001, p011, p111, p101, &glass);           // front
        add_quad(p000, p010, p011, p001, &green);           // left
        add_quad(p100, p101, p111, p110, &triangle_material);// right
        add_quad(p010, p110, p111, p011, &none_material);   // top: override
        add_quad(p000, p001, p101, p100, &ground);          // bottom

        host_scene.add_mesh(cube_mesh, &triangle_material);
        rt::DeviceScene device_scene;

        device_scene.upload_scene(host_scene);
        

        Color* host_pixels = (Color*)malloc(sizeof(Color) * WINDOW_HEIGHT * WINDOW_WIDTH);
        
        rt::CudaRenderer renderer;
        rt::KernelData kernel_data;

        renderer.initialize(WINDOW_WIDTH, WINDOW_HEIGHT, 2000, 10);

        // Initialize the camera data for a one-shot CUDA PPM render.
        kernel_data.forward_vector = camera.get_forward_vector();
        kernel_data.up_vector = camera.get_up_vector();
        kernel_data.right_vector = camera.get_right_vector();
        kernel_data.viewport_depth = viewport.get_viewport_depth();
        kernel_data.viewport_dx = viewport.get_viewport_dx();
        kernel_data.viewport_dy = viewport.get_viewport_dy();
        kernel_data.position = camera.get_position();

        if (ppm_mode){
            renderer.render_HD(device_scene.view(), kernel_data, host_pixels);
            std::vector<::Color> cuda_pixels(
                host_pixels,
                host_pixels + WINDOW_WIDTH * WINDOW_HEIGHT
            );
            output_ppm(ppm_filename, cuda_pixels, WINDOW_WIDTH, WINDOW_HEIGHT);

            free(host_pixels);
            renderer.shutdown();
            return 0;
        }
        
        InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "raylib example - basic window");
        Image canvas = GenImageColor(WINDOW_WIDTH, WINDOW_HEIGHT, BLACK);
        Texture2D texture = LoadTextureFromImage(canvas);
        UnloadImage(canvas);
        DisableCursor();
    
        SetTargetFPS(1000);
        float dt;
        while (!WindowShouldClose())
        {  
            bool changed_position = false;
            dt = GetFrameTime();;
            Vector2 mouse_delta = GetMouseDelta();
            
            // Mouse delta is already frame-to-frame movement; do not multiply
            // it by frame time, or the sensitivity changes with FPS.
            camera.update_pitch_yaw(-MOUSE_SENSITIVITY * mouse_delta.y,
                                    MOUSE_SENSITIVITY * mouse_delta.x);
            if (abs(mouse_delta.y) > 0.0001 || abs(mouse_delta.x) > 0.0001){
                changed_position = true;
            }
            
            rt::Vec3 forward = camera.get_forward_vector();
            rt::Vec3 right   = camera.get_right_vector();
            rt::Vec3 up      = camera.get_up_vector();


            if (IsKeyDown(KEY_RIGHT)){
                camera.update_pos(right.x * dt * 1.50, right.y * dt * 1.50, right.z * dt * 1.50);   
                changed_position = true;
            }

            if (IsKeyDown(KEY_LEFT)){
                camera.update_pos(-right.x * dt * 1.50, -right.y * dt * 1.50, -right.z * dt * 1.50);
                changed_position = true;
                
            }
            if (IsKeyDown(KEY_UP)){
                camera.update_pos(forward.x * dt * 1.50, forward.y * dt * 1.50, forward.z * dt * 1.50);   
                changed_position = true;
            }

            if (IsKeyDown(KEY_DOWN)){
                camera.update_pos(-forward.x * dt * 1.50, -forward.y * dt * 1.50, -forward.z * dt * 1.50);
                changed_position = true;
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

            if (changed_position) renderer.render(device_scene.view(), kernel_data, host_pixels, true);
            else renderer.render(device_scene.view(), kernel_data, host_pixels, false);
            
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
            camera.update_pitch_yaw(-MOUSE_SENSITIVITY * mouse_delta.y,
                                    MOUSE_SENSITIVITY * mouse_delta.x);

        
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
