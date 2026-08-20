# 1. Set the minimum required CMake version
cmake_minimum_required(VERSION 3.15)

# 2. Define the project name and programming language
project(MyProject LANGUAGES CXX)

# 3. Create the executable compilation target
add_executable(my_app)

# 4. Bind the source files to your target
target_sources(my_app PRIVATE src/main.cpp src/Sphere.cpp
    src/Renderer.cpp
    src/Utility.cpp
    src/Material.cpp
    src/Scene.cpp)


# Locate an external library installed on your system (e.g., glfw)
find_package(raylib REQUIRED)

# Link the external library securely to your target
target_link_libraries(my_app PRIVATE raylib)