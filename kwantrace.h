//
// Created by jeppesen on 2/3/21.
//

#ifndef KWANTRACE_KWANTRACE_H
#define KWANTRACE_KWANTRACE_H

//System headers that are used in many places
#include <memory> //for smart pointers
#include <vector> //for collections

//Eigen library, used in many places
#include <eigen3/Eigen/Dense> //for matrices and vectors

//KwanTrace library, ordered from lower-level to higher-level.
#include "common.h"
#include "Transformation.h"
#include "Ray.h"
#include "Renderable.h"
#include "Composite.h"
#include "Light.h"
#include "Shader.h"
#include "Camera.h"
#include "Scene.h"

#endif //KWANTRACE_KWANTRACE_H
