#pragma once

#include "GpuScene.h"
#include "cuda/cuda_kernels.h"
namespace rt{

class CudaRenderer {

    public:
        void initialize(int width, int height, int spp);
        void render(GpuView device_view, KernelData data, ::Color* host_pixels);    
        void shutdown();      
    
    private:
   
        ::Color* device_pixels = nullptr;
        int width;
        int height;
        int spp;

};
}