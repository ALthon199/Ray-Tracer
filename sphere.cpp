#include "sphere.h"



Sphere::Sphere(Vec3 position, Vec3 color, float radius):
    position(position), color(color), radius(radius)
{

}

// Returns the time t >= 0 of where the ray hits, returns -1 if doesnt hit
float Sphere:: sphere_hit(const Ray& ray) const{

    float a = ray.direction.dot(ray.direction);

    float b = 2 * ray.direction.dot(ray.origin - position); 

    float c = (ray.origin - position).dot(ray.origin - position) - radius * radius;
    
    float det = b * b - 4 * a * c;

    if (det < 0){
        return -1;
    }


    float t1 = (-b + std::sqrt(det)) / (2 * a);
    float t2 = (-b - std::sqrt(det)) / (2 * a);
    return t1 < t2 ? t1 : t2;

}

Vec3 Sphere:: get_origin() const{
    return position;
}
Vec3 Sphere::sphere_color() const{
    return color;
}

