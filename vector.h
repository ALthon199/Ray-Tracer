#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>
#include <iostream>

struct Vec3{
    float x, y, z;

    Vec3(float x, float y, float z): 
    x(x), y(y), z(z)
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

    float dot(const Vec3& other) const{
        return (x * other.x + y * other.y + z * other.z);
    }

   


};

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
}; 


#endif