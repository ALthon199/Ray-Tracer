#pragma once

#include "GpuScene.h"
#include "cuda_kernels.h"
namespace rt{

class CudaRenderer {

    public:
        void initialize(const GpuScene* scene, int width, int height, int spp);
        void render(KernelData data, ::Color* host_pixels);    
        void shutdown();      
    
    private:
        
        GpuScene world;
        ::Color* device_pixels = nullptr;
        int width;
        int height;
        int spp;

};
}