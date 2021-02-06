//
// Created by jeppesen on 2/3/21.
//

#ifndef KWANTRACE_RENDERABLE_H
#define KWANTRACE_RENDERABLE_H

#include <memory>
#include "TransformChain.h"
#include "Ray.h"
#include "Field.h"

namespace kwantrace {
  class Renderable:public Transformable {
  protected:
    std::shared_ptr<ColorField> pigment;
  public:
    /**
     * Intersect a ray with an object
     * @param[in]  ray      Ray to intersect with object, in world space
     * @param[out] t        ray parameter of intersection
     * @param[out] indexes  used for composite objects
     * @return
     */
    virtual bool intersect(const Ray &ray, double &t, std::vector<int> &indexes) const=0;
    virtual DirectionVector normal(const PositionVector &point, std::vector<int> &indexes, int level=-1) const=0;
    virtual bool inside(const PositionVector &point) const=0;
    void set_pigment(std::shared_ptr<ColorField> Lpigment) {
      pigment = Lpigment;
    }
    virtual bool eval_pigment(const PositionVector& v, ObjectColor& color, std::vector<int> &indexes, int level=-1) const {
      if(pigment) {
        color=(*pigment)(v);
        return true;
      }
      return false;
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

  class Primitive:public Renderable {
  private:
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
    virtual DirectionVector normal_local(const PositionVector &point, std::vector<int> &indexes, int level= -1) const = 0;
    //!Check if a point is inside the object, in object local space
    /**
     *
     * @param[in] point Point in object coordinates
     * @return True if point is inside object, false if not
     */
    virtual bool inside_local(const PositionVector &point) const = 0;
  public:
    bool inside_out=false;
    virtual ~Primitive() {};
    /**
     * Intersect a ray with an object
     * @param[in]  ray      Ray to intersect with object, in world space
     * @param[out] t        ray parameter of intersection
     * @param[out] indexes  used for composite objects
     * @return
     */
    virtual bool intersect(const Ray &ray, double &t, std::vector<int> &indexes) const override {
      return intersect_local(transformChain.Mw2b * ray, t, indexes);
    };

    virtual DirectionVector normal(const PositionVector &point, std::vector<int> &indexes, int level=-1) const override {
      return (DirectionVector) ((inside_out ? -1 : 1) *
                                (transformChain.Mb2wN * normal_local(transformChain.Mw2b * point, indexes, level)));
    }

    virtual bool inside(const PositionVector &point) const override {
      return inside_out^inside_local(transformChain.Mw2b*point);
    }
  };

  class Composite : public Renderable {
  protected:
    std::vector<std::shared_ptr<Renderable>> children;
    virtual DirectionVector normal(const PositionVector &point, std::vector<int> &indexes, int level= -1) const override {
      if(level<0) {
        level=indexes.size()-1;
      }
      if(indexes[level]>0) {
        return children[indexes[level]]->normal(point,indexes,level-1);
      } else {
        return DirectionVector();
      }
    };
    /**Return appropriate pigment. This is the pigment of the correct child component, if it has one,
     * else pigment of this object. This lets childrens' pigments override parents. For example, suppose you have a
     * composite object with three children. Two of them have no pigment, while the third one is blue. The composite
     * is red. If we are checking a point on the blue child, you get blue, otherwise you get red.
     *
     * @param[in] v
     * @param[out] pigment
     * @param[in] indexes
     * @param[in] level
     * @return
     */
    virtual bool eval_pigment(const PositionVector& v, ObjectColor& pigment, std::vector<int> &indexes, int level=-1) const override {
      if(level<0) {
        level = indexes.size() - 1;
      }
      bool childHasPigment=false;
      if(indexes[level]>=0) {
        childHasPigment=children[indexes[level]]->eval_pigment(v,pigment,indexes,level-1);
      }
      if(childHasPigment) {
        return true;
      } else {
        return Renderable::eval_pigment(v,pigment,indexes,level);
      }
    }
  public:
    auto lastIndex() {return children.size()-1;}
    Renderable& last() {return *(children.back());}
    virtual void prepareRender() override {
      Renderable::prepareRender();
      for (auto&& child:children) {
        child->prepareRender();
      }
    }
    std::shared_ptr<Renderable> addChild(std::shared_ptr<Renderable> child) {
      children.push_back(child);
      return child;
    }
    virtual std::shared_ptr<Eigen::Affine3d> addTransform(std::shared_ptr<Eigen::Affine3d> transform) override {
      std::shared_ptr<Eigen::Affine3d> result=Renderable::addTransform(transform);
      for(auto&& child:children) {
        child->addTransform(transform);
      }
      return result;
    }
  };

  class Union : public Composite {
  protected:
    virtual bool intersect(const Ray &ray, double &t, std::vector<int> &indexes) const override {
      bool result=false;
      t=std::numeric_limits<double>::infinity();
      int i=0;
      int index=-1;
      for(auto&& child:children) {
        double this_t;
        if(child->intersect(ray, this_t, indexes)) {
          result=true;
          if(this_t<t) {
            t=this_t;
            index=i;
          }
        }
        i++;
      }
      //Note that if the sub-object which is hit is also composite, then it will itself have already pushed
      //onto this vector. So, the vector ends up indexed from inside to out.
      indexes.push_back(index);
      return result;
    };
    virtual bool inside(const PositionVector &point) const override {
      bool result=false;
      for(auto&& child:children) {
        result|=child->inside(point);
      }
      return result;
    };
  };

}

#include "Sphere.h"

#endif //KWANTRACE_RENDERABLE_H
