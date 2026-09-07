#include "Material.h"
#include "Vector.h"
#include "Hittable.h"
namespace rt {

class Triangle : public Hittable {
    public:
        Vec3 v1, v2, v3;

        Triangle(Vec3 v1, Vec3 v2, Vec3 v3, std::shared_ptr<Material> material) : 
        v1(v1), v2(v2), v3(v3), Hittable(material)
        {}

    private:
        void hit(const Ray& ray, HitRecord& record) const override;
        Vec3 sample_point() const override;

        
        
           
};

}
