#include <raylib.h>
#include <cuda_runtime.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "cuda_kernels.h"
#include "Camera.h"
#include "Vector.h"
#include "Viewport.h"

namespace rt{
__global__ void calculate_pixel_idx(::Color* result, KernelData kernel_data, int width, int height) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int col = i % width;
    int row = i / height;

    Vec3& right = kernel_data.right_vector;
    Vec3& up = kernel_data.up_vector;
    Vec3& forward = kernel_data.forward_vector;

    float dx = kernel_data.viewport_dx;
    float dy = kernel_data.viewport_dy;
    float depth = kernel_data.viewport_dx;
    Vec3 topleft = kernel_data.position - right * (width/2) * (dx) + up * (height/2) * (dy);
    Vec3 target = topleft + forward * depth + up * (-row) * dy + right * (col) * dx;

    Ray ray = Ray(kernel_data.position, target - kernel_data.position);
    ray.direction.normalize();

    Color color = ray.ray_base_color();
    result[i] = ::Color{(unsigned char)(color.x * 255), (unsigned char)(color.y * 255), (unsigned char)(color.z * 255), 255};
}




void render_pixels(int width, int height, ::Color* pixels, Camera camera, Viewport viewport) {


    // --- GPU timing ---
    ::Color* d_result;
    cudaMalloc(&d_result, width * height * sizeof(::Color));

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

    cudaEventRecord(start);
    calculate_pixel_idx<<<width, height>>>(d_result, kernel_data, width, height);
    cudaEventRecord(stop);
    cudaEventSynchronize(stop);




    cudaMemcpy(pixels, d_result, width * height * sizeof(::Color), cudaMemcpyDeviceToHost);

    

    // verify results match
  
  
 

    cudaFree(d_result);
    cudaEventDestroy(start);
    cudaEventDestroy(stop);


}
}