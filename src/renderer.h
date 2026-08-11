#pragma once

#include "Hittable.h"
#include "Camera.h"
#include "Vector.h"
#include "ImageBuffer.h"
#include "Viewport.h"
namespace rt {

class Renderer {
    public:
        Renderer(int r_ssp)
            : ssp(r_ssp)
        {
        }
        void render_frame(const HittableList& hit_list, const Camera& camera, const Viewport& viewport, ImageBuffer& pixels) const;
        
    private:
        int ssp;
        
              
};


}



