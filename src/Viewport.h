#pragma once

namespace rt{

class Viewport{
    
    public:
        Viewport(float height, int WINDOW_WIDTH, int WINDOW_HEIGHT){
            float aspect_ratio = static_cast<float> (WINDOW_WIDTH) / WINDOW_HEIGHT;
            viewport_height = height;
            viewport_width = aspect_ratio * viewport_height;
            viewport_depth = 1;
            viewport_dx = viewport_width/WINDOW_WIDTH;
            viewport_dy = viewport_height/WINDOW_HEIGHT;
        }

        float get_viewport_dx() const{
            return viewport_dx;
        }

        float get_viewport_dy() const{
            return viewport_dy;
        }

        float get_viewport_width() const{
            return viewport_width;
        }

        float get_viewport_height() const{
            return viewport_height;
        }

        float get_viewport_depth() const{
            return viewport_depth;
        }


    private:
        float viewport_height;
        float viewport_width;
        float viewport_depth;
        float viewport_dx;
        float viewport_dy;

};
}