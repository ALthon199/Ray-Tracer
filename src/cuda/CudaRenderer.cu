#include "cuda/CudaRenderer.h"
#include "cuda/GpuScene.h"
#include "cuda/GpuMaterial.h"
#include "cuda/cuda_kernels.h"
#include "cuda/GpuUtility.h"
#include <curand_kernel.h>

namespace rt {

    void CudaRenderer::initialize(int width, int height, int max_spp, int bounces){
        this->width = width;
        this->height = height;
        this->max_spp =  max_spp;
        this->max_bounces = bounces;
        cudaMalloc(&device_pixels, sizeof(::Color) * width * height);
        cudaMalloc(&accumulated_sum, sizeof(rt::Color) * width * height);
    }


    __device__ Color render_pixel(const Ray& ray, const GpuView& world, int bounces, curandStatePhilox4_32_10_t& state){
        if (bounces <= 0) return Color();

        GpuScatterRecord s_record = GpuScatterRecord{Vec3(), Ray(Vec3(), Vec3())};
        
        Ray curr_ray = ray;

        Color curr_attenuation = Color(1, 1, 1);
        Color result = Color();
        bool is_primary = true;
        for (int i = 0; i < bounces; ++i){
            // Each bounce needs a fresh hit record. Reusing the previous one
            // can make a miss look like the old material was hit again.
            GpuHitRecord record = GpuHitRecord();
            Color direct_lighting = Color();
            bool scattered = false;
            bool hit = world.ray_hit(curr_ray, record);
            if (!hit) {
                result += Color(0.8, 0.8, 0.8) * curr_attenuation;
                break;
            }

            Vec3 hit_pos = curr_ray.ray_at(record.time);
            const GpuMaterial& mat = world.materials[record.mat_index];
            switch (mat.get_type()){
                case GpuMaterialType::EMISSIVE:
                    if (is_primary){
                        const GpuEmissive& light_src = mat.get_material().emissive;
                        result += curr_attenuation * (light_src.albedo * light_src.intensity);
                    }
                    break;
                case GpuMaterialType::DIFFUSE:
                    const Vec3& next_pos = hit_pos.hit_offset(record.normal);
                    direct_lighting = world.calculate_direct_lighting(next_pos, record.normal, state);
                    
                    scattered = mat.scatter(curr_ray, record, s_record, state);
                    is_primary = false;
                    break;
                case GpuMaterialType::METAL: 
                    direct_lighting = Vec3();
                    scattered = mat.scatter(curr_ray, record, s_record, state);
                    if (!scattered){
                        break;
                    }
                    is_primary = true;
                    break;
                case GpuMaterialType::DIELECTRIC:
                    direct_lighting = Vec3();
                    scattered = mat.scatter(curr_ray, record, s_record, state);
                    is_primary = true;
                    break;
            }   

            if (!scattered){
                return result;
            }

            curr_attenuation *= s_record.attentuation;
            result += direct_lighting * curr_attenuation;
            curr_ray = s_record.scattered_ray;
          
        }
        return result;
    }

    
    __global__ void render_kernel(KernelData kernel_data, GpuView scene, rt::Color* accumulated_frame, ::Color* result, int bounces, bool is_new_frame) {
        int i = blockIdx.x * blockDim.x + threadIdx.x;
        if (i >= kernel_data.width * kernel_data.height) {
            return;
        }

        curandStatePhilox4_32_10_t local_state;
        curand_init(1234ULL, i, kernel_data.spp, &local_state);


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
        
       
        
            
        float rx = curand_uniform(&local_state);
        float ry = curand_uniform(&local_state);
        
    
        float pixel_row = row + ry;
        float pixel_col = col + rx;
        Vec3 topleft = kernel_data.position - right * (width/2) * (dx) + up * (height/2) * (dy);
        Vec3 target = topleft + forward * depth + up * (-pixel_row) * dy + right * (pixel_col) * dx;

        Ray ray = Ray(kernel_data.position, target - kernel_data.position);
        ray.direction.normalize();

        rt::Color pixel = render_pixel(ray, scene, bounces, local_state);
      
        pixel.x = g_clamp(pixel.x, 0.0f, 1.0f);
        pixel.y = g_clamp(pixel.y, 0.0f, 1.0f);
        pixel.z = g_clamp(pixel.z, 0.0f, 1.0f);
        
        pixel.x = sqrtf(pixel.x);
        pixel.y = sqrtf(pixel.y);
        pixel.z = sqrtf(pixel.z);
        if (is_new_frame) accumulated_frame[i] = pixel;
        else accumulated_frame[i] += pixel;
        
      
        result[i] = raylib_color_from_Vec3(accumulated_frame[i] * (1.0/kernel_data.spp));
    }

    void CudaRenderer::render_HD(GpuView device_view, KernelData data, ::Color* host_pixels){
        curr_spp = 0;
        cudaMemset(accumulated_sum, 0, sizeof(rt::Color) * width * height);
        data.width = width;
        data.height = height;
        
        
        int threads = 256;
        int total_pixels = width * height;
        int blocks = (total_pixels + threads - 1) / threads;

        while (curr_spp < max_spp){
            curr_spp++;
            data.spp = curr_spp;
            render_kernel<<<blocks, threads>>>(data, device_view, accumulated_sum, device_pixels, max_bounces, false);
        }
        cudaDeviceSynchronize();
        cudaMemcpy(host_pixels, device_pixels, sizeof(::Color) * width * height, cudaMemcpyDeviceToHost);
    }
    void CudaRenderer::render(GpuView device_view, KernelData data, ::Color* host_pixels, bool is_new_frame) {
        if (is_new_frame){
            curr_spp = 0;
        }

        if (curr_spp < max_spp){
            curr_spp++;
        }
        else{
            return;
        }

        data.width = width;
        data.height = height;
        data.spp = curr_spp;
        
        int threads = 256;
        int total_pixels = width * height;
        int blocks = (total_pixels + threads - 1) / threads;

        render_kernel<<<blocks, threads>>>(data, device_view, accumulated_sum, device_pixels, max_bounces, is_new_frame);
        
        cudaDeviceSynchronize();
        cudaError_t error = cudaDeviceSynchronize();
       
        cudaMemcpy(host_pixels, device_pixels, sizeof(::Color) * width * height, cudaMemcpyDeviceToHost);
    }

    void CudaRenderer::shutdown(){
        cudaFree(device_pixels);
        cudaFree(accumulated_sum);
    }

}
