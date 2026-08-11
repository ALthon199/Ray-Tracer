#include <random>
#include "Utility.h"

float random_num(float min, float max){
 
    std::random_device rd;

    
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> distr(-0.5, 0.5);

  
   return distr(gen);
}
