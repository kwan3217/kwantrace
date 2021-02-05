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
    /** Intersect a ray with this object, in object local space.
     * @param[in]  ray      ray, already transformed into local object space
     * @param[out] t        ray parameter of intersection
     * @param      indexes  used for composite objects
     * @return              true if ray intersects surface, false if not. Output parameter t is unspecified
     *                         if function returns false
     */
    virtual bool intersect_local(const Ray &ray, double &t, std::vector<int> &indexes) const=0;
    /** Generate the normal vector to an object at a point.
     *
     * @param[in]  point    point on surface of object, already transformed into local object space
     * @param[in]  indexes  used for composite objects
     * @param[in]  level    used for composite objects
     * @return Normal to surface
     *
     * It is unspecified behavior to call this on a point which is not on the surface
     * of an object -- the function may return any value at all. That being said, it is
     * better to be forgiving in the face of floating point limited precision.
     */
    virtual Eigen::Vector3d normal_local(const Eigen::Vector3d &point, std::vector<int> &indexes, int level=-1) = 0;
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
    std::shared_ptr<ColorField> pigment;
    bool inside_out=false;
    virtual ~Primitive() {};
    /**Prepare an object for rendering. This must be called
     * between any change to the object and rendering the object
     */
    virtual void prepareRender() {
      transformChain.prepareRender();
      if(pigment) {
        pigment->prepareRender();
      }
    }
    /**
     * Intersect a ray with an object
     * @param[in]  ray      Ray to intersect with object, in world space
     * @param[out] t        ray parameter of intersection
     * @param[out] indexes  used for composite objects
     * @return
     */
    bool intersect(const Ray &ray, double &t, std::vector<int> &indexes) const {
      return intersect_local(transformChain.Mw2b * ray, t, indexes);
    };

    Eigen::Vector3d normal(const Eigen::Vector3d &point, std::vector<int> &indexes, int level=-1) {
      return (inside_out?-1:1)*mul0(transformChain.Mb2wN, normal_local(mul1(transformChain.Mw2b, point), indexes, level));
    }

    bool inside(const Eigen::Vector3d &point) {
      return inside_out^inside_local(mul1(transformChain.Mw2b, point));
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
