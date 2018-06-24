cmake_minimum_required(VERSION 3.10)

set(project_name "RayTracing")

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${project_name})

project(${project_name})

set(CMAKE_CXX_FLAGS "-std=c++11")

file(GLOB src src/*.cpp src/*.h)

add_executable(${project_name} ${src})