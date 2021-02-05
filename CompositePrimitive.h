//
// Created by jeppesen on 2/4/21.
//

#ifndef KWANTRACE_COMPOSITEPRIMITIVE_H
#define KWANTRACE_COMPOSITEPRIMITIVE_H

#include <vector>
#include <memory>
#include "Primitive.h"

namespace kwantrace {
  /**
   * Boo! Multiple inheritance! Fortunately, kwantrace::Primitive and std::vector<> have no
   * common ancestors, so at least there is no diamond problem. I wish that I knew how to declare
   * an add member function (or set of them) so that I could contain and delegate a member vector,
   * but I don't so I can't.
   */
  class CompositePrimitive : public Primitive, public std::vector<std::shared_ptr<Primitive>> {
  protected:
    virtual DirectionVector normal_local(const PositionVector &point, std::vector<int> &indexes, int level= -1) const override {
      if(level<0) {
        level=indexes.size()-1;
      }
      if(indexes[level]>0) {
        return (*this)[indexes[level]]->normal(point,indexes,level-1);
      } else {
        return DirectionVector();
      }
    };
    /**Return appropriate pigment. This is the pigment of the correct child component, if it has one,
     * else pigment of this object. This lets childrens' pigments override parents. For example, suppose you have a
     * composite object with three spheres. Two of them have no pigment, while the third one is blue. The composite
     * is red. If we are checking a point on the blue sphere, you get blue, otherwise you get red.
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
        childHasPigment=(*this)[indexes[level]]->eval_pigment(v,pigment,indexes,level-1);
      }
      if(childHasPigment) {
        return true;
      } else {
        return Primitive::eval_pigment(v,pigment,indexes,level);
      }
    }
  public:
    auto lastIndex() {return size()-1;}
    Primitive& last() {return *(back());}
    virtual void prepareRender() override {
      Primitive::prepareRender();
      for (auto&& primitive:*this) {
        primitive->prepareRender();
      }
    }
  };

}

#endif //KWANTRACE_COMPOSITEPRIMITIVE_H
