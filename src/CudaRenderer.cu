#include "CudaRenderer.h"
#include "gpu_scene.h"
#include "cuda_kernels.h"
namespace rt {

    void CudaRenderer::initialize(const GpuScene* scene, int width, int height){
        this->width = width;
        this->height = height;

        world = (GpuScene*)malloc(sizeof(GpuScene));
        world->spheres_count = scene->spheres_count;
        cudaMalloc(&world->spheres, sizeof(GpuSphere) * scene ->spheres_count);
        cudaMalloc(&device_pixels, sizeof(::Color) * width * height);
       
        cudaMemcpy(world->spheres, scene->spheres, sizeof(GpuSphere) * scene->spheres_count, cudaMemcpyHostToDevice);
    }

  
    __global__ void render_kernel(KernelData kernel_data, GpuScene scene, ::Color* result) {
        int i = blockIdx.x * blockDim.x + threadIdx.x;
        
        int width = kernel_data.width;
        int height = kernel_data.height;
        int row = i / width;
        int col = i % width;
        if (i >= width * height) {
            return;
        }
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


    void CudaRenderer::render(KernelData data, ::Color* host_pixels) {
        data.width = width;
        data.height = height;
        int threads = 256;
        int total_pixels = width * height;
        int blocks = (total_pixels + threads - 1) / threads;

        render_kernel<<<blocks, threads>>>(data, *world, device_pixels);
        
        cudaDeviceSynchronize();
     
     
        cudaMemcpy(host_pixels, device_pixels, sizeof(::Color) * width * height, cudaMemcpyDeviceToHost);
    }

    void CudaRenderer::shutdown(){
        cudaFree(world->spheres);
        free(world);
        cudaFree(device_pixels);
    }

}