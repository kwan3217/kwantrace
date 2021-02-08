//
// Created by jeppesen on 2/7/21.
//

#ifndef KWANTRACE_COMPOSITE_H
#define KWANTRACE_COMPOSITE_H

class Composite : public kwantrace::Renderable {
protected:
  kwantrace::RenderableList children;
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
  virtual bool evalPigment(const PDVector<1> & v, kwantrace::ObjectColor& pigment, const kwantrace::Primitive* finalObject) const override {
    for(auto&& child:children) {
      if(child->contains(finalObject)) {
        if(child->evalPigment(v,pigment,finalObject)) {
          return true;
        }
        break;
      }
    }
    return Renderable::evalPigment(v, pigment, finalObject);
  }
public:
  auto lastIndex() {return children.size()-1;}
  Renderable& last() {return *(children.back());}
  virtual bool contains(const kwantrace::Primitive* finalObject) const override{
    for (auto&& child:children) {
      if(child->contains(finalObject)) return true;
    }
    return false;
  }
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
  virtual std::shared_ptr<Transform < double, 3, Affine>> addTransform(std::shared_ptr<Transform < double, 3, Affine>> transform) override {
    std::shared_ptr<Transform < double, 3, Affine>> result= addTransform(transform);
    for(auto&& child:children) {
      child->addTransform(transform);
    }
    return result;
  }
};

class Union : public Composite {
protected:
  virtual const kwantrace::Primitive* intersect(const kwantrace::Ray &ray, double& t) const override {
    const kwantrace::Primitive* result;
    t=std::numeric_limits<double>::infinity();
    for(auto&& child:children) {
      double this_t;
      const kwantrace::Primitive* this_result=child->intersect(ray, this_t);
      if(result && this_t<t) {
        t=this_t;
        result=this_result;
      }
    }
    return result;
  };
  virtual bool inside(const PDVector<1> &point) const override {
    bool result=false;
    for(auto&& child:children) {
      result|=child->inside(point);
    }
    return result;
  };
};

#include "Renderable.h"

#endif //KWANTRACE_COMPOSITE_H
