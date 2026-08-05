#pragma once

#include "hittable.h"
#include "camera.h"
#include "vector.h"
namespace rt {

class Renderer {
    public:
        Renderer(int r_ssp)
            : ssp(r_ssp)
        {
        }
        Color pixel_color(const Ray& ray, const HittableList& hit_list, const Camera& camera);
        
    private:
        int ssp;
        
              
};


}



