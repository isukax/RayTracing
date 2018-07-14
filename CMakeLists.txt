cmake_minimum_required(VERSION 3.10)

set(project_name "RayTracing")

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${project_name})

project(${project_name})

find_package( OpenMP )
if (OPENMP_FOUND) 
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB_RECURSE src src/*.cpp src/*.h)
#file(GLOB_RECURSE external external/*.h)
include_directories(external)
include_directories(external/assimp/include)

link_directories(external/assimp/lib/)

add_executable(${project_name} ${src})
