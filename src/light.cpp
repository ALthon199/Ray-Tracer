#include "Light.h"
#include "Vector.h"


namespace rt{
    
Light::Light(Vec3 position, Vec3 color, float radius):
    position(position), color(color), radius(radius)
{

}

// Returns the time t >= 0 of where the ray hits, returns -1 if doesnt hit
void Light::hit(const Ray& ray, HitRecord& record) const{
   
    float a = ray.direction.dot(ray.direction);
    float b = 2 * ray.direction.dot(ray.origin - position); 
    float c = (ray.origin - position).dot(ray.origin - position) - radius * radius;
    float det = b * b - 4 * a * c;

    // No hits
    if (det < 0.0 ){
        return;
    }

    float t1 = (-b + std::sqrt(det)) / (2 * a);
    float t2 = (-b - std::sqrt(det)) / (2 * a);

    if (t1 < 0 && t2 < 0){
        return;
    }

    float t;
    if (t1 > 0 && t2 < 0){
        t = t1;
    }
    else if (t1 < 0 && t2 > 0){
        t = t2;
    }
    else{
        t = t1 > t2 ? t2 : t1;
    }
   
    Vec3 P = ray.ray_at(t);
    Vec3 normal = P - position;
    Vec3 unit = normal.normalize();
    
    if (record.time == -1 || t < record.time){
      
        record.hit_light = true;
        record.time = t;
        record.normal = unit;
        record.color = get_light_color();
    }
}

Vec3 Light::get_origin() const{
    return position;
}

Vec3 Light::get_light_color() const{
    return color;
}

}
