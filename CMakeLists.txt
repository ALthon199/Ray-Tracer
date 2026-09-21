cmake_minimum_required(VERSION 3.15)

if(CMAKE_CUDA_COMPILER)
    project(MyProject LANGUAGES CXX CUDA)
else()
    project(MyProject LANGUAGES CXX)
endif()

add_executable(my_app)
target_compile_features(my_app PRIVATE cxx_std_17)

target_sources(my_app PRIVATE
    src/main.cpp
    src/Sphere.cpp
    src/Renderer.cpp
    src/Utility.cpp
    src/Material.cpp
    src/Scene.cpp
    src/Triangle.cpp
    src/Mesh.cpp
)

find_package(raylib REQUIRED)
target_link_libraries(my_app PRIVATE raylib)

if(CMAKE_CUDA_COMPILER)
    find_package(CUDAToolkit REQUIRED)

    target_sources(my_app PRIVATE
        src/cuda_kernels.cu
    )

    target_link_libraries(my_app PRIVATE CUDA::cudart)

    set_target_properties(my_app PROPERTIES
        CUDA_ARCHITECTURES 75
        CUDA_SEPARABLE_COMPILATION OFF
    )

    target_compile_definitions(my_app PRIVATE HAS_CUDA)

    find_file(CUDART_DLL cudart64_13.dll
        PATHS "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v13.4/bin/x64"
    )
    if(CUDART_DLL)
        add_custom_command(TARGET my_app POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CUDART_DLL}
            $<TARGET_FILE_DIR:my_app>
        )
    endif()
endif()