#include <random>
#include <iostream>
#include "Utility.h"
#include "Vector.h"


rt::Vec3 random_unit_vec(){
    return rt::Vec3(random_num(-1.0f, 1.0f), random_num(-1.0f, 1.0f), random_num(-1.0f, 1.0f)).normalize();
}

float random_num(float min, float max){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distr(min, max);
    return distr(gen);
}


float clamp(float val, float min, float max){
    if (min > max){
        std::cerr << "Clamp min is greater than Clamp max";
    }
    if (val < min){
        return min;
    }
    if (val > max){
        return max;
    }

    return val;
}

void color_clamp(rt::Color& color, float min, float max){
    color.x = clamp(color.x, 0.0f, 1.0f);
    color.y = clamp(color.y, 0.0f, 1.0f);
    color.z = clamp(color.z, 0.0f, 1.0f);
}