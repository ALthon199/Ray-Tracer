#include "cuda/CudaRenderer.h"
#include "cuda/GpuScene.h"
#include "cuda/cuda_kernels.h"
#include <curand_kernel.h>

namespace rt {

    void CudaRenderer::initialize(const GpuScene* scene, int width, int height, int spp){
        this->width = width;
        this->height = height;
        this->spp = spp;
        
        world.spheres_count = scene->spheres_count;
        cudaMalloc(&world.spheres, sizeof(GpuSphere) * scene ->spheres_count);
        cudaMalloc(&device_pixels, sizeof(::Color) * width * height);
        cudaMemcpy(world.spheres, scene->spheres, sizeof(GpuSphere) * scene->spheres_count, cudaMemcpyHostToDevice);
    }

  
    __global__ void render_kernel(KernelData kernel_data, GpuScene scene, ::Color* result) {
        int i = blockIdx.x * blockDim.x + threadIdx.x;
        if (i >= kernel_data.width * kernel_data.height) {
            return;
        }

        curandStatePhilox4_32_10_t local_state;
        curand_init(1234ULL, i, 0, &local_state);

        int width = kernel_data.width;
        int height = kernel_data.height;
        int row = i / width;
        int col = i % width;

        Vec3& right = kernel_data.right_vector;
        Vec3& up = kernel_data.up_vector;
        Vec3& forward = kernel_data.forward_vector;

        float dx = kernel_data.viewport_dx;
        float dy = kernel_data.viewport_dy;
        float depth = kernel_data.viewport_depth;
        
        rt::Color pixel = rt::Color();
       
        for (int s = 0; s < kernel_data.spp; s++) {
            
            float rx = curand_uniform(&local_state);
            float ry = curand_uniform(&local_state);
            
       
            float pixel_row = row + ry;
            float pixel_col = col + rx;
            Vec3 topleft = kernel_data.position - right * (width/2) * (dx) + up * (height/2) * (dy);
            Vec3 target = topleft + forward * depth + up * (-pixel_row) * dy + right * (pixel_col) * dx;

            Ray ray = Ray(kernel_data.position, target - kernel_data.position);
            ray.direction.normalize();

            GpuHitRecord record = GpuHitRecord();
            for (int i = 0; i < scene.spheres_count; i++) {
                hit_sphere(record, scene.spheres[i], ray);
            }    
            
            if (record.time > 0.0f) {
                pixel += (record.color);
            } else {
                pixel += (ray.ray_base_color());
            }
        }
      
        
        result[i] = raylib_color_from_Vec3(pixel * (1.0/kernel_data.spp));
    }


    void CudaRenderer::render(KernelData data, ::Color* host_pixels) {
        data.width = width;
        data.height = height;
        data.spp = spp;
        int threads = 256;
        int total_pixels = width * height;
        int blocks = (total_pixels + threads - 1) / threads;

        render_kernel<<<blocks, threads>>>(data, world, device_pixels);
        
        cudaDeviceSynchronize();
        cudaError_t error = cudaDeviceSynchronize();
        
        cudaMemcpy(host_pixels, device_pixels, sizeof(::Color) * width * height, cudaMemcpyDeviceToHost);
    }

    void CudaRenderer::shutdown(){
        cudaFree(world.spheres);
        cudaFree(device_pixels);
    }

}