#pragma once
#include "Vector.h"

namespace rt {

struct HitRecord;

struct ScatterRecord {
    Color attentuation;
    Ray scattered_ray;
};

enum class MaterialType {
    DIFFUSE,
    METAL,
    DIELECTRIC,
    EMISSIVE
};

class Material {
    public:
        virtual ~Material() = default;
        virtual bool scatter(const Ray& ray, const HitRecord& record, ScatterRecord& s_record) const = 0;
        virtual MaterialType get_type() const = 0;
        virtual Color emitted() const {
            return Color(0, 0, 0);
        }
};

class Diffuse : public Material {
    public:
        Diffuse(const Vec3& albedo) : albedo(albedo){  
        }
        bool scatter(const Ray& ray, const HitRecord& record, ScatterRecord& s_record) const override;
        MaterialType get_type() const override;
       

    private:
        Vec3 albedo;
};

class Metal : public Material {
    public:
        Metal(const Vec3& albedo, float fuzz) : albedo(albedo), fuzz(fuzz){  
        }
        bool scatter(const Ray& ray, const HitRecord& record, ScatterRecord& s_record) const override;
        MaterialType get_type() const override;
      

    private:
        Vec3 albedo;
        float fuzz;
};

class Dielectric : public Material {
    public:
        Dielectric(float refraction_index) : refraction_index(refraction_index){  
        }
        bool scatter(const Ray& ray, const HitRecord& record, ScatterRecord& s_record) const override;
        MaterialType get_type() const override;
       

    private:
        float refraction_index;
};

class Emissive : public Material {
    public:
        Emissive(const Vec3& albedo, float intensity) : albedo(albedo), intensity(intensity){  
        }
        bool scatter(const Ray& ray, const HitRecord& record, ScatterRecord& s_record) const override;
        Color emitted() const override;
        MaterialType get_type() const override;
    
    private:
        Vec3 albedo;
        float intensity;
};


}