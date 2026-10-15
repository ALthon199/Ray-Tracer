#pragma once

#include <raylib.h>
#include "GpuScene.h"
#include "cuda/cuda_kernels.h"
namespace rt{

class CudaRenderer {

    public:
        void initialize(int width, int height, int spp, int bounces);
        void render_HD(GpuView device_view, KernelData data, ::Color* host_pixels);   
        void render(GpuView device_view, KernelData data, ::Color* host_pixels, bool new_frame);    
        void shutdown();      
    
    private:
        rt::Color* accumulated_sum = nullptr;
        ::Color* device_pixels = nullptr;
        int width;
        int height;
        int max_spp;
        int curr_spp = 0;
        int max_bounces;
        

};
}
