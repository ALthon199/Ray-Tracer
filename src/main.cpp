
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include "Sphere.h"
#include "Vector.h"
#include "Hittable.h"
#include "Camera.h"
#include "Renderer.h"
#include "PPM.h"
#include "Viewport.h"
#include "ImageBuffer.h"
#include <raylib.h>
#include <raymath.h>

#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 180


int main(int argc, char** argv) {
    
    bool ppm_mode = false;
    std::string ppm_filename = "output.ppm";
    for (int i = 1; i < argc; i++){
        std::string arg = argv[i];
        if (arg == "ppm"){
            ppm_mode = true;
        }
    }

  
    rt::Vec3 global_up = rt::Vec3(0, 1, 0);
    rt::Camera camera = rt::Camera();
    rt::Renderer rt_renderer = rt::Renderer(5, 3);
    rt::HittableList world = rt::HittableList();
    rt::Vec3 forward = camera.get_forward_vector();
    rt::Vec3 right   = camera.get_right_vector();
    rt::Vec3 up      = camera.get_up_vector();
    
    world.add_hittable(std::make_unique<rt::Sphere>(rt::Vec3(0, -200, -5), rt::Vec3(1.0, 0, 0), 199));
    world.add_hittable(std::make_unique<rt::Sphere>(rt::Vec3(0, -0.5, -3), rt::Vec3(1.0, 0 , 0), 0.5));
    
    
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
    }



    InitWindow(WINDOW_WIDTH * 4, WINDOW_HEIGHT * 4, "raylib example - basic window");
    Image canvas = GenImageColor(WINDOW_WIDTH, WINDOW_HEIGHT, BLACK);
    Texture2D texture = LoadTextureFromImage(canvas);
    UnloadImage(canvas);
    DisableCursor();
   
    SetTargetFPS(30);
    while (!WindowShouldClose())
    {  
        Vector2 mouse_delta = GetMouseDelta();
        float length = Vector2Length(mouse_delta);
        if (length > 0){
            camera.update_pitch_yaw(-0.05 * mouse_delta.y/length, 0.05 * mouse_delta.x/length);
        }

       
        rt::Vec3 forward = camera.get_forward_vector();
        rt::Vec3 right   = camera.get_right_vector();
        rt::Vec3 up      = camera.get_up_vector();


        if (IsKeyDown(KEY_RIGHT)){
            camera.update_pos(right.x * 0.05, right.y * 0.05, right.z * 0.05);   
        }

        if (IsKeyDown(KEY_LEFT)){
            camera.update_pos(-right.x * 0.05, -right.y * 0.05, -right.z * 0.05);
            
        }
        if (IsKeyDown(KEY_UP)){
            camera.update_pos(forward.x * 0.05, forward.y * 0.05, forward.z * 0.05);   
        }

        if (IsKeyDown(KEY_DOWN)){
            camera.update_pos(-forward.x * 0.05, -forward.y * 0.05, -forward.z * 0.05);
            
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
            DrawTexturePro(texture, {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}, {0, 0, 4 * WINDOW_WIDTH, 4 * WINDOW_HEIGHT}, {0,0}, 0.0f, WHITE);
            DrawFPS(0, 0);
        EndDrawing();
    }

    CloseWindow();
    // Close the file
  

    return 0;
}
