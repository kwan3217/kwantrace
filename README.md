# KwanTrace
A C++ Ray-tracer

It occurred to me that most of my worries over POV-Ray are due to:

 * Parsing time
 * Memory usage
 * Sharing between frames

All of these could be eliminated by changing the parser up. What if instead
of a custom scene description language, we just used C++? The program would
construct the scene, then call the render routine to do the actual rendering.

     Shader s();
     Cone c(vector(),radius1,radius2);
     s.add(c);
     // Lots more stuff
     s.render();

*Then, it could modify the existing scene and render it again.*

     c.radius1=clock*0.2;
     s.render();

The interface isn't quite this clean yet, but it's getting there.

A program which uses this library would then be structured something like this:

    #include <kwantrace.h>
    using std;
    using kwantrace;
    
    int main() {
      auto scene=Scene();
      auto light=scene->add(make_shared<Light>(/* light params */);
      auto sphere1=scene->add(make_shared<Sphere>());
      auto sphere1trans=sphere1->translate(5,0,0);
      //...other stuff to decorate sphere1
      auto sphere2=scene->add(make_shared<Sphere>());
      sphere2->translate(-5,0,0);
      //...other stuff to decorate sphere2
      //...other scene objects. You only need to keep handles
      //for things you will be using later
      scene->add(make_shared<PerspectiveCamera>(/* camera params */));

      int width=1920;
      int height=1080;
      int frames=100;
      for(int frame=0;frame<frames;frame++) {
        //move sphere1
        sphere1trans->setX(5+frame*0.1);
        //...other scene modifications

        auto pixmap=scene->render(width,height); //this does ALL the hard work

        //...scene is now rendered in pixmap. The
        //following code writes a portable pixel map (PPM)
        char oufn[20];
        sprintf(oufn,"image%02d.ppm");
        FILE* ouf=fopen(oufn,"wb");
        fprintf(ouf,"P6 %d %d 256",width,height);
        fwrite(pixmap->get(),width*3,height,ouf);
        fclose(ouf);
      }
    }

Our scene description therefore combines all the power and expressiveness
of C++ with all the headaches and gotchas of C++.

## Extension

The library is intended to be extended. It is easy to add new primitives,
new texture maps, new lights, new camera projections, etc. You can even
add new scene renderers which do things in parallel threads, do pixel
supersampling, etc. All of these will be first-class classes, of equal
rank and performance to anything "built-in". In fact, all concrete features
of this library are implemented as extensions themselves.

See the header file for each class for details of how to extend.

## Library Structure

The library is broken up into what I believe to be bite-sized, well-decomposed
classes, of which these are the most important:

   * kwantrace::Ray - the object that describes a ray, with an initial
     position and direction
   * kwantrace::Renderable - Describes an object that can be rendered. This is further subclassed to:
       * kwantrace::Primitive - a single mathematical primitive object, such as a sphere, cone, height field,
         etc. (Just because it's primitive, doesn't mean it's easy). Subclasses define individual objects.
         This deals with converting back and forth from world coordinates to local coordinates, since in many
         cases the definition of an object is much simpler in the right local frame. Subclasses only have to deal
         with local coordinates.

         Primitive objects can be intersected by a ray, can calculate surface normals, and can determine if a given
         point is inside or outside of itself.
       * kwantrace::Composite - Container for Renderable objects, intended to be used for Constructive Solid
         Geometry (CSG).

         Subclasses override the ray intersection function, to call the intersection functions
         for their children, and to determine which intersections count and which do not.
   * kwantrace::Camera - maps pixels on screen to initial rays in world space
   * kwantrace::Light - handles lights, including checking whether a light is visible from a particular
         point. Default implementation handles point sources.
   * kwantrace::Shader - Calculates the color of a ray, given the intrinsic properties of the object and the
         geometry of the intersection point. Subclasses include:
       * kwantrace::AmbientShader - Mimics ambient in POV-Ray. This just gives you a fraction of the intrinsic color
         and is in a sense the "glow" of an object.
       * kwantrace::DiffuseShader - Mimics diffuse in POV-Ray. This gives you diffuse shading with the Lambert shading
         model.
       * kwantrace::POVRayShader - Adds together all of the POV-Ray shaders.
   * kwantrace::Scene - holds the objects, lights, camera, and shader for a scene, and manages rendering the scene
     into a pixel buffer. For each pixel in the image, this code:
       1. calculates a normalized image point
       2. passes it to the camera to get the initial ray
       3. Checks the ray against the objects in the scene
       4. If there is an intersection, calculates the normal and passes everything to the shader. The shader itself
          may spawn a ray as part of its calculation.
       5. Writes the returned color to the image buffer

See the full class heirarchy for more details.

All of these classes have virtual destructors, so any of them can be further subclassed by your code. For instance,
if there is a primitive I didn't think of that you want, you can write a subclass of Primitive, fill in the intersect,
normal, and inside functions, then add an instance of your new class to the scene just like any other object. You can
also add cameras (for handling different camera distortion models), shaders, etc.

## Ray tracing reference
This class is in a sense intended to be an executable textbook on ray tracing.
Ray tracing itself includes a lot of aspects of what might be called practical
computational geometry. So, even if you never render a single reflective
sphere over a checkered plane, it might be useful to know this stuff.

It's currently light on vector and linear algebra stuff, but that is 
implemented in Eigen and covered much more thoroughly than I ever could
in other sources. To start, check out the 3blue1brown [linear algebra
series](https://www.youtube.com/playlist?list=PLZHQObOWTQDPD3MizzM2xVFitgF8hE_ab).
Think of this as the part of practical computational geometry that isn't linear
algebra, but uses linear algebra (and assumes you know it).

This document might not have everything that is known about ray tracing, but it will
contain everything *I* know about it. Some of the method documentation comments
are practically essays in themselves.

* Parametric equations and rays: kwantrace::Ray
* Intersections with implicit surfaces: kwantrace::Primitive::intersectLocal()
* Transforming normal vectors from body to world coordinates: kwantrace::Primitive::normal()
* Transformation of an object to point it at a target: kwantrace::PointToward::matrix()
* Coordinate frame handedness: kwantrace::RotateScalar
* Lambertian reflectance: kwantrace::DiffuseShader
* Point-light shadows: kwantrace::DiffuseShader::shade() and kwantrace::Light
* Translation matrices and homogeneous coordinates: kwantrace::PDVector
## C++ patterns
We use:
* Distinct scene construction and rendering phases. Again keeping in mind the
  animation loop, we construct a scene, call kwanpov::Scene::render(), then
  when that returns, we modify the scene and call render() again. Inside
  of render(), the intent is to use read-only access to the scene, and
  write-only access to the pixel buffer. This breakdown is intended
  to allow multi-threaded rendering. Many classes have a prepareRender()
  method which needs to be called in the beginning of the rendering process
  to do things like turn Transformation objects into matrices, set
  pointers to parent objects, etc. All of these are called automatically
  by kwantrace::Scene::render(). 
* Containers, particularly `std::vector<shared_ptr<>>`. These are usually
   kept private or protected, so that we are forced to use accessor methods.
* C++ smart pointers, particularly `shared_ptr`. This allows us in 
   principle to construct
   an object once, then attach it as a child to two different parents. The
   two parents would have pointers to the one real object, so any change
   to the child will show up with both parents.
* Observing an object is implemented with raw pointers. You can look at
   and change an object through an observer, but someone else always owns
   it and is responsible for cleaning it up. This is wrapped in the 
   kwantrace::Observer<> template `using` definition.
* Most add-type accessor methods take a `shared_ptr`, add it to the list in
   question, then return the passed-in pointer as-is. This allows us to do
   things like this:
   
        Scene scene;
        auto sphere1=scene.add(std::make_shared<Sphere>());
        sphere1->addPigment(...);

   The scene is in a sense the only mandatory owner, but this code has a shared
   copy of the pointer keeping the object alive as well. If we don't need
   to reference the child object any more, we don't have to catch the returned
   pointer copy.
     
## License
All source code in this project is written by me, kwan3217. The GNU
General Public Licence v2.0 applies to it. Some of the images in
the image/ folder were created by others, and are used by me under
the appropriate license as detailed in image/README.md . Images
that I *did* create are licensed with the GNU GPL 2.0 as well.
