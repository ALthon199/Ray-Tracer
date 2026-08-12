#include <random>
#include "Utility.h"
#include "Vector.h"

rt::Vec3 random_unit_vec(){
    return rt::Vec3(random_num(0.0f, 0.1f), random_num(0.0f, 0.1f), random_num(0.0f, 0.1f)).normalize();
}

float random_num(float min, float max){
 
    std::random_device rd;

    
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> distr(-0.5, 0.5);

  
   return distr(gen);
}
