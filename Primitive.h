//
// Created by jeppesen on 2/3/21.
//

#ifndef KWANTRACE_PRIMITIVE_H
#define KWANTRACE_PRIMITIVE_H

#include <memory>
#include "TransformChain.h"
#include "Ray.h"
#include "Field.h"

namespace kwantrace {
  class Primitive {
  protected:
    //! Intersect a ray with this object, in object local space
    /**
     *
     * @param[in] ray ray, already transformed into local object space
     * @param[out] t ray parameter of intersection
     * @return true if ray intersects surface, false if not. Output parameter t is only defined
     *   if function returns true
     */
    virtual bool intersect_local(const Ray &ray, double &t) = 0;
    //! Generate the normal vector to an object at a point.
    /**
     *
     * @param[in] point point on surface of object
     * @return Normal to surface
     *
     * It is undefined behavior to call this on a point which is not on the surface
     * of an object -- the function may return any value at all. That being said, it is
     * better to be forgiving in the face of floating point limited precision.
     */
    virtual Eigen::Vector3d normal_local(const Eigen::Vector3d &point) = 0;
    //!Check if a point is inside the object, in object local space
    /**
     *
     * @param[in] point Point in object coordinates
     * @return True if point is inside object, false if not
     */
    virtual bool inside_local(const Eigen::Vector3d &point) = 0;
  public:
    //! Transformation chain
    TransformChain transformChain;
    std::unique_ptr<Field<5>> Pigment;
    virtual ~Primitive() {};
    virtual void prepareRender() {
      transformChain.prepareRender();
    }
    //! Prepare an object for rendering.
    /**
     * This must be called between any change to the object and rendering the object
     */
    bool intersect(const Ray &ray, double &t) {
      return intersect_local(transformChain.M * ray, t);
    };

    Eigen::Vector3d normal(const Eigen::Vector3d &point) {
      return mul0(transformChain.MinvT, normal_local(mul1(transformChain.M, point)));
    }

    bool inside(const Eigen::Vector3d &point) {
      return inside_local(mul1(transformChain.M, point));
    }

    int translate(const Eigen::Vector3d &point) { return transformChain.translate(point); }

    int translate(double x, double y, double z) { return transformChain.translate(x, y, z); }

    int rotate(const Eigen::Vector3d &point) { return transformChain.rotate(point); }

    int rotate(double x, double y, double z) { return transformChain.rotate(x, y, z); }

    int scale(const Eigen::Vector3d &point) { return transformChain.scale(point); }

    int scale(double x, double y, double z) { return transformChain.scale(x, y, z); }
  };
}

#include "Sphere.h"
#include "CompositePrimitive.h"
#include "Union.h"

#endif //KWANTRACE_PRIMITIVE_H
