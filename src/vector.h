#pragma once

#include <cmath>
#include <iostream>
#include "cuda_compat.h"

namespace rt{
struct Vec3{
    float x, y, z;

    HD Vec3(){
        x = 0;
        y = 0;
        z = 0;
    }
    HD Vec3(float x, float y, float z): x(x), y(y), z(z)
    {
    }

    HD Vec3& operator+=(const Vec3& other){
        x += other.x;
        y += other.y;
        z += other.z;
        return (*this);
    }
    HD Vec3& operator*=(float t){
        x *= t;
        y *= t;
        z *= t;
        return (*this);
    }
    HD Vec3& operator*=(Vec3 other){
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return (*this);
    }
    HD Vec3 operator+(const Vec3& other) const{
        return Vec3(x + other.x, y + other.y, z + other.z);
    }
    HD Vec3 operator-(const Vec3& other) const{
        return Vec3(x - other.x, y - other.y, z - other.z);
    }
    HD Vec3 operator*(float t) const{
        return Vec3(x * t, y * t, z* t);
    }
    HD Vec3 operator*(Vec3 other) const{
        return Vec3(x * other.x, y * other.y, z * other.z);
    }
    HD Vec3& normalize(){
        float scale = sqrtf(x * x + y * y + z * z);
       
        x /= scale;
        y /= scale;
        z /= scale;
        return (*this);
    }
    
    HD float magnitude() const{
        return sqrtf(x * x + y * y + z * z);
    }
    HD void set(float new_x, float new_y, float new_z){
        x = new_x;
        y = new_y;
        z = new_z;
    }
    HD float dot(const Vec3& other) const{
        return (x * other.x + y * other.y + z * other.z);
    }
    HD Vec3 cross(const Vec3& other) const{
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
    
    HD Vec3 hit_offset(const Vec3& outward_normal) const{
        return (*this) + (outward_normal * 0.001);
    }


};

using Color = Vec3;

inline int rgb_map(float x){
    if (x > 0.9999){
        x = 0.9999;
    }
    if (x < 0.0001){
        x = 0.0001;
    }
    return static_cast<int>((x)  * 255.99);
}
struct Ray{
    Vec3 origin;
    Vec3 direction;

    HD Ray(Vec3 origin, Vec3 direction):
    origin(origin), direction(direction)
    {
    }

    HD Vec3 ray_at(float t) const{
        return origin + (direction * t);
    }

    HD Color ray_base_color() const{
        Vec3 unit_direction = direction;
        float a = 0.5 * (direction.y + 1.0);
        return Color(1.0, 1.0, 1.0) * (1.0 - a) + Color(0.5, 0.7, 1.0) * a;
    }   
    
}; 

}
