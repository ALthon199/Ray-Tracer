#include "cuda/GpuMesh.h"
#include "cuda/GpuPrimitives.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
namespace rt {
    HostMesh::HostMesh(std::string file_name){
        // Takes in a obj file and returns the as a host mesh

        std::vector<Vec3> vertices;
        std::ifstream file(file_name);

        if (!file.is_open()){
            std::cerr << "No file exists";
            return;
        }

        std::string line;

        while(std::getline(file, line)){
            std::stringstream ss(line);
            std::string prefix;
            ss >> prefix;
            
            
            if (prefix == "v") {
                Vec3 v;
                ss >> v.x >> v.y >> v.z;
                vertices.push_back(v);
            }

            else if (prefix == "f") {
                
                std::vector<int> indices;
                std::string token;
                while (ss >> token) {
                    int first_num = std::stoi(token);
                    indices.push_back(first_num);
                }
                
                for (int i = 0; i < indices.size() - 2; i++){
                    GpuTriangle tri;
                    
                    tri.v1 = vertices[indices[0] - 1];
                    tri.v2 = vertices[indices[i+1] - 1];
                    tri.v3 = vertices[indices[i+2] - 1];
                    faces.push_back(tri);
                    materials.push_back(nullptr);
                }
            }
        }
        
    }
}