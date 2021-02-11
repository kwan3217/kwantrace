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

/** \mainpage
 * # KwanTrace
 * A C++ Ray-tracer
 *
 * It occurred to me that most of my worries over POV-Ray are due to:
 *
 *  * Parsing time
 *  * Memory usage
 *  * Sharing between frames
 *
 * All of these could be eliminated by changing the parser up. What if instead
 * of a custom scene description language, we just used C++? The program would
 * construct the scene, then call the render routine to do the actual rendering.
 *
 *     Shader s();
 *     Cone c(vector(),radius1,radius2);
 *     s.add(c);
 *     // Lots more stuff
 *     s.render();
 *
 * *Then, it could modify the existing scene and render it again.*
 *
 *     c.radius1=clock*0.2;
 *     s.render();
 *
 * The interface isn't quite this clean yet, but it's getting there.
 *
 * The library is broken up into what I believe to be bite-sized, well-decomposed
 * classes, of which these are the most important:
 *
 *   * kwantrace::Ray - the object that describes a ray, with an initial
 *     position and direction
 *   * kwantrace::Renderable - Describes an object that can be rendered. This is further subclassed to:
 *       * kwantrace::Primitive - a single mathematical primitive object, such as a sphere, cone, height field,
 *         etc. (Just because it's primitive, doesn't mean it's easy). Subclasses define individual objects.
 *         This deals with converting back and forth from world coordinates to local coordinates, since in many
 *         cases the definition of an object is much simpler in the right local frame. Subclasses only have to deal
 *         with local coordinates.
 *
 *         Primitive objects can be intersected by a ray, can calculate surface normals, and can determine if a given
 *         point is inside or outside of itself.
 *       * kwantrace::Composite - Container for Renderable objects, intended to be used for Constructive Solid
 *         Geometry (CSG).
 *
 *         Subclasses override the ray intersection function, to call the intersection functions
 *         for their children, and to determine which intersections count and which do not.
 *   * kwantrace::Camera - maps pixels on screen to initial rays in world space
 *   * kwantrace::Light - handles lights, including checking whether a light is visible from a particular
 *         point. Default implementation handles point sources.
 *   * kwantrace::Shader - Calculates the color of a ray, given the intrinsic properties of the object and the
 *         geometry of the intersection point. Subclasses include:
 *       * kwantrace::AmbientShader - Mimics ambient in POV-Ray. This just gives you a fraction of the intrinsic color
 *         and is in a sense the "glow" of an object.
 *       * kwantrace::DiffuseShader - Mimics diffuse in POV-Ray. This gives you diffuse shading with the Lambert shading
 *         model.
 *       * kwantrace::POVRayShader - Adds together all of the POV-Ray shaders.
 *   * kwantrace::Scene - holds the objects, lights, camera, and shader for a scene, and manages rendering the scene
 *     into a pixel buffer. For each pixel in the image, this code:
 *       1. calculates a normalized image point
 *       2. passes it to the camera to get the initial ray
 *       3. Checks the ray against the objects in the scene
 *       4. If there is an intersection, calculates the normal and passes everything to the shader. The shader itself
 *          may spawn a ray as part of its calculation.
 *       5. Writes the returned color to the image buffer
 *
 * See the full class heirarchy for more details.
 *
 * All of these classes have virtual destructors, so any of them can be further subclassed by your code. For instance,
 * if there is a primitive I didn't think of that you want, you can write a subclass of Primitive, fill in the intersect,
 * normal, and inside functions, then add an instance of your new class to the scene just like any other object. You can
 * also add cameras (for handling different camera distortion models), shaders, etc.
 */


#endif //KWANTRACE_KWANTRACE_H
