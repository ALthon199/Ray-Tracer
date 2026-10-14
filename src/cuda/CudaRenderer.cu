#include "cuda/CudaRenderer.h"
#include "cuda/GpuScene.h"
#include "cuda/GpuMaterial.h"
#include "cuda/cuda_kernels.h"
#include "cuda/GpuUtility.h"
#include <curand_kernel.h>

namespace rt {

    void CudaRenderer::initialize(int width, int height, int spp, int bounces){
        this->width = width;
        this->height = height;
        this->spp = spp;
        this->max_bounces = bounces;
        cudaMalloc(&device_pixels, sizeof(::Color) * width * height);
    }


    __device__ Color render_pixel(const Ray& ray, const GpuView& world, int bounces, curandStatePhilox4_32_10_t& state){
        if (bounces <= 0) return Color();

        GpuHitRecord record = GpuHitRecord();
        GpuScatterRecord s_record = GpuScatterRecord{Vec3(), Ray(Vec3(), Vec3())};
        
        
      

        Ray curr_ray = ray;

        Color curr_attenuation = Color(1, 1, 1);
        Color result = Color();
        bool is_primary = true;
        for (int i = 0; i < bounces; ++i){
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


    __global__ void render_kernel(KernelData kernel_data, GpuView scene, ::Color* result, int bounces) {
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

            pixel += render_pixel(ray, scene, bounces, local_state);
        }
        
        Vec3 final_color = pixel * (1.0f / kernel_data.spp);

        final_color.x = g_clamp(final_color.x, 0.0f, 1.0f);
        final_color.y = g_clamp(final_color.y, 0.0f, 1.0f);
        final_color.z = g_clamp(final_color.z, 0.0f, 1.0f);

        result[i] = raylib_color_from_Vec3(final_color);
    }


    void CudaRenderer::render(GpuView device_view, KernelData data, ::Color* host_pixels) {
        data.width = width;
        data.height = height;
        data.spp = spp;
        int threads = 256;
        int total_pixels = width * height;
        int blocks = (total_pixels + threads - 1) / threads;

        render_kernel<<<blocks, threads>>>(data, device_view, device_pixels, max_bounces);
        
        cudaDeviceSynchronize();
        cudaError_t error = cudaDeviceSynchronize();
       
        cudaMemcpy(host_pixels, device_pixels, sizeof(::Color) * width * height, cudaMemcpyDeviceToHost);
    }

    void CudaRenderer::shutdown(){
        cudaFree(device_pixels);
    }

}