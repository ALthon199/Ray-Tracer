#include "vector.h"
#include <raylib.h>
#include <cmath>

namespace rt{
    class Camera {
        public:
            Camera(){
                initial_vector = Vec3(0, 0, -1);
                forward_vector = initial_vector;
                position = Vec3();
                pitch = 0;
                yaw = -PI/2;
            }
            Vec3 get_position() const;
            Vec3 get_forward_vector() const;
            void update_pitch_yaw(float dpitch, float dyaw);
            void update_pos(float dx, float dy, float dz);
        
        private:
            Vec3 initial_vector;
            Vec3 forward_vector;
            Vec3 position;
            float pitch;
            float yaw;
    };

    inline Vec3 Camera::get_position() const{
        return position;
    }
    
  
    inline Vec3 Camera::get_forward_vector() const{
        return forward_vector;
    }

    inline void Camera::update_pitch_yaw(float dpitch, float dyaw){
        pitch += dpitch;
        float max_pitch = 89.0f * (3.14159265f / 180.0f);
        pitch = std::clamp(pitch, -max_pitch, max_pitch);

        yaw += dyaw;
        yaw = fmodf(yaw, 2.0f * 3.14159265f); 

        float new_x = cos(pitch) * cos(yaw);
        float new_y = sin(pitch);
        float new_z = cos(pitch) * sin(yaw);

        forward_vector = Vec3(new_x, new_y, new_z).normalize();
    }

    inline void Camera::update_pos(float dx, float dy, float dz){
        position.x += dx;
        position.y += dy;
        position.z += dz;
    }
}

