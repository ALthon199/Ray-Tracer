#pragma once

#include "Hittable.h"
#include "Scene.h"
#include "Camera.h"
#include "Vector.h"
#include "ImageBuffer.h"
#include "Viewport.h"
namespace rt {

class Renderer {
    public:
        Renderer(int r_ssp, int r_max_bounces)
            : ssp(r_ssp), max_bounces(r_max_bounces)
        {
        }
        Color calculate_color(const Ray& ray, const Scene& world, int bounces) const;
        void render_frame(const Scene& world, const Camera& camera, const Viewport& viewport, ImageBuffer& pixels) const;
        
    private:
        int ssp;
        int max_bounces;
        
              
};


}



