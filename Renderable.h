//
// Created by jeppesen on 2/3/21.
//

#ifndef KWANTRACE_RENDERABLE_H
#define KWANTRACE_RENDERABLE_H

#include <memory>
#include "Transformable.h"
#include "Ray.h"
#include "Field.h"

namespace kwantrace {
  class Primitive;
  /**
   * Superclass for Primitive and Composite. This is able to be intersected and has an inside, but does not have a normal.
   * It has a pigment since it is needed both for Primitive, and for Composite as the default pigment.
   */
  class Renderable:public Transformable {
  protected:
    std::shared_ptr<ColorField> pigment;
    Observer<Renderable> parent=nullptr; ///< Used to find parent object to inherit default properties from
  public:
    virtual void setParent(Observer<Renderable> Lparent) {parent=Lparent;}
    /** Intersect a ray with this object, in world space.
     * @param[in]  ray      ray in world space
     * @param[out] t        Ray parameter of intersection
     * @return              Pointer to Renderable if ray intersects surface, nullptr if not.
     *                         Output parameter t is unspecified if function returns false
     */
    virtual Observer<Primitive> intersect(const Ray &ray, double& t) const=0;
    virtual bool inside(const Position &r) const=0;
    void setPigment(std::shared_ptr<ColorField> Lpigment) {
      pigment = Lpigment;
    }
    virtual bool evalPigment(const Position& r, ObjectColor& color) const {
      if(pigment) {
        color=(*pigment)(r);
        return true;
      } else if(parent) {
        return parent->evalPigment(r,color);
      } else {
        return false;
      }
    }
    virtual std::shared_ptr<Eigen::Affine3d> addTransform(std::shared_ptr<Eigen::Affine3d> transform) override {
      std::shared_ptr<Eigen::Affine3d> result=Transformable::addTransform(transform);
      if(pigment) {
        pigment->addTransform(transform);
      }
      return result;
    }
    /**Prepare an object for rendering. This must be called
     * between any change to the object and rendering the object
     */
    virtual void prepareRender() override {
      Transformable::prepareRender();
      if(pigment) {
        pigment->prepareRender();
      }
    }
  };
  typedef std::vector<std::shared_ptr<Renderable>> RenderableList;

  /** Primitive object -- IE one that directly has geometry itself, rather than being a composite of other
   * renderables. Subclasses of this only need to deal with local coordinates.
   */
  class Primitive:public Renderable {
  private:
    /** Intersect a ray with this object, in object local space.
     * @param[in]  rayLocal ray in local object space
     * @param[out] t        Position of intersection
     * @return              True if object is intersected by this ray. Output parameter r is unspecified if function returns false
     */
    virtual bool intersectLocal(const Ray &rayLocal, double& t) const=0;
    /** Generate the normal vector to an object at a point.
     *
     * @param[in]  rLocal point on surface of object, already transformed into local object space
     * @return     Normal to surface.
     *
     * It is unspecified behavior to call this on a point which is not on the surface
     * of an object -- the function may return any value at all. That being said, it is
     * better to be forgiving in the face of floating point limited precision.
     *
     * Don't worry about returning a unit-length normal, that is done upstream. You can't
     * even do it if you wanted to, since it needs to be unit-length in world coordinates.
     */
    virtual Direction normalLocal(const Position &rLocal) const = 0;
    /**Check if a point is inside the object, in object local space
     *
     * @param[in] rLocal Point in object coordinates
     * @return True if point is inside object, false if not
     */
    virtual bool insideLocal(const Position &rLocal) const = 0;
  public:
    bool inside_out=false;
    virtual ~Primitive() {};
    virtual Observer<Primitive> intersect(const Ray &ray, double& t) const override {
      if (intersectLocal(Mw2b * ray, t)) {
        return this;
      } else {
        return nullptr;
      }
    };

    virtual Direction normal(const Position &r) const {
      return (Direction) ((inside_out ? -1 : 1) * (Mb2wN * normalLocal(Mw2b * r)).normalized());
    }

    virtual bool inside(const Position &r) const override {
      return inside_out ^ insideLocal(Mw2b * r);
    }
  };

}

#include "Sphere.h"

#endif //KWANTRACE_RENDERABLE_H
