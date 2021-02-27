cmake_minimum_required(VERSION 3.13)
project(kwantrace)

set(CMAKE_CXX_STANDARD 20)

add_executable(kwantrace main.cpp Renderable.h Sphere.h Ray.h Transformable.h kwantrace.h Camera.h PerspectiveCamera.h common.h Field.h Light.h Shader.h Scene.h Transformation.h)

target_precompile_headers(kwantrace PUBLIC pch.h)
