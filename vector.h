#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>
#include <iostream>

namespace rt{
struct Vec3{
    float x, y, z;

    Vec3(){
        x = 0;
        y = 0;
        z = 0;
    }
    Vec3(float x, float y, float z): x(x), y(y), z(z)
    {
    }

    Vec3& operator+=(const Vec3& other){
        x += other.x;
        y += other.y;
        z += other.z;
        return (*this);
    }
    Vec3 operator+(const Vec3& other) const{
        return Vec3(x + other.x, y + other.y, z + other.z);
    }
    Vec3 operator-(const Vec3& other) const{
        return Vec3(x - other.x, y - other.y, z - other.z);
    }
    Vec3 operator*(float t){
        return Vec3(x * t, y * t, z* t);
    }
    Vec3& normalize(){
        float scale = std::sqrt(x * x + y * y + z * z);
        if (scale == 0){
            std::cout << "Vector of length 0" << std::endl;
            return (*this);
        }
        x /= scale;
        y /= scale;
        z /= scale;
        return (*this);
    }
    

    void set(float new_x, float new_y, float new_z){
        x = new_x;
        y = new_y;
        z = new_z;
    }
    float dot(const Vec3& other) const{
        return (x * other.x + y * other.y + z * other.z);
    }
    Vec3 cross(const Vec3& other) const{
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
   


};

using color = Vec3;

inline int rgb_map(float x){
    return static_cast<int>((x + 1) / 2 * 255);
}
struct Ray{
    Vec3 origin;
    Vec3 direction;

    Ray(Vec3 origin, Vec3 direction):
    origin(origin), direction(direction)
    {

    }
    Vec3 ray_at(float t) const{
        return origin + direction;
    }
    color ray_color() const{
        Vec3 unit_direction = direction;
        float a = 0.5 * (direction.y+ 1.0);
        return color(1.0, 1.0, 1.0) * (1.0 - a) + color(0.5, 0.7, 1.0) * a;
    }   
    
}; 

}
#endif