#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <string>
#include "Sphere.h"
#include "Vector.h"
#include "Hittable.h"
#include "Camera.h"
#include "Renderer.h"
#include <raylib.h>
// File to simply to help with outputting ppm files


inline void output_ppm(const std::string& out_file, const std::vector<Color>& pixels, int WINDOW_WIDTH, int WINDOW_HEIGHT){
    std::ofstream outFile(out_file);
    if (!outFile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
    }
  
    outFile << "P3" << " " << WINDOW_WIDTH << " " << WINDOW_HEIGHT << " " << 255 << "\n";
    for (int y = 0; y < WINDOW_HEIGHT; y++){
        for (int x = 0; x < WINDOW_WIDTH; x++){
            Color pixel = pixels[y * WINDOW_WIDTH + x];    
            outFile << (int) (pixel.r) << " " << (int) (pixel.g) << " " << (int) (pixel.b) << "\n";
        }
    }

}
