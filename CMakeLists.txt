cmake_minimum_required(VERSION 3.16)
project(kwantrace)

set(CMAKE_CXX_STANDARD 20)

add_executable(kwantrace main.cpp Primitive.h Sphere.h Ray.h TransformChain.h kwantrace.h Camera.h PerspectiveCamera.h common.h CompositePrimitive.h Union.h Field.h)