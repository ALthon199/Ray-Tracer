#include <raylib.h>
#include <cuda_runtime.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "cuda_kernels.h"
#include "../Camera.h"
#include "../Vector.h"
#include "../Viewport.h"
#include "../cuda_compat.h"
#include "GpuScene.h"

namespace rt {

__global__ void calculate_pixel_idx(::Color* result, KernelData kernel_data, int width, int height, GpuScene scene) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int col = i % width;
    int row = i / width;

    Vec3& right = kernel_data.right_vector;
    Vec3& up = kernel_data.up_vector;
    Vec3& forward = kernel_data.forward_vector;

    float dx = kernel_data.viewport_dx;
    float dy = kernel_data.viewport_dy;
    float depth = kernel_data.viewport_depth;
    Vec3 topleft = kernel_data.position - right * (width/2) * (dx) + up * (height/2) * (dy);
    Vec3 target = topleft + forward * depth + up * (-row) * dy + right * (col) * dx;

    Ray ray = Ray(kernel_data.position, target - kernel_data.position);
    ray.direction.normalize();

    GpuHitRecord record = GpuHitRecord();
    for (int i = 0; i < scene.spheres_count; i++) {
       hit_sphere(record, scene.spheres[i], ray);
    }    
   
    if (record.time > 0.0f) {
        result[i] = raylib_color_from_Vec3(record.color);
    } else {
        result[i] = raylib_color_from_Vec3(ray.ray_base_color());
    }
}








void render_pixels(int width, int height, ::Color* pixels, Camera camera, Viewport viewport, GpuScene scene) {

    ::Color* gpu_result;
    GpuSphere* gpu_spheres; 
    
        
    cudaMalloc(&gpu_spheres, sizeof(GpuSphere) * scene.spheres_count);
    cudaMalloc(&gpu_result, width * height * sizeof(::Color));
    cudaMemcpy(gpu_spheres, scene.spheres, sizeof(GpuSphere) * scene.spheres_count, cudaMemcpyHostToDevice);

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    // Setup kernel params
    KernelData kernel_data;
    
    kernel_data.viewport_depth = viewport.get_viewport_depth();
    kernel_data.viewport_dx = viewport.get_viewport_dx();
    kernel_data.viewport_dy = viewport.get_viewport_dy();
    
    kernel_data.position = camera.get_position();
    kernel_data.forward_vector = camera.get_forward_vector();
    kernel_data.up_vector = camera.get_up_vector();
    kernel_data.right_vector = camera.get_right_vector();

    // Setup scene params
    GpuScene gpu_scene;
    gpu_scene.spheres = gpu_spheres;
    gpu_scene.spheres_count = scene.spheres_count;


    cudaEventRecord(start);
    calculate_pixel_idx<<<width, height>>>(gpu_result, kernel_data, width, height, gpu_scene);
    cudaEventRecord(stop);
    cudaEventSynchronize(stop);




    cudaMemcpy(pixels, gpu_result, width * height * sizeof(::Color), cudaMemcpyDeviceToHost);
    cudaFree(gpu_spheres);
    cudaFree(gpu_result);
    cudaEventDestroy(start);
    cudaEventDestroy(stop);


}
}