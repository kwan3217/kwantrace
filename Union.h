//
// Created by jeppesen on 2/4/21.
//

#ifndef KWANTRACE_UNION_H
#define KWANTRACE_UNION_H

#include "Primitive.h"

namespace kwantrace {

  class Union : public CompositePrimitive {
  protected:
    virtual bool intersect_local(const Ray &ray, double &t) override {
      bool result=false;
      t=std::numeric_limits<double>::infinity();
      lastHit=-1;
      int i=0;
      for(auto&& primitive:*this) {
        double this_t;
        if(primitive->intersect(ray,this_t)) {
          result=true;
          if(this_t<t) {
            t=this_t;
            lastHit=i;
          }
        }
        i++;
      }
      return result;
    };
    virtual Eigen::Vector3d normal_local(const Eigen::Vector3d &point) override {
      if(lastHit>0) {
        return (*this)[lastHit]->normal(point);
      } else {
        return Eigen::Vector3d();
      }
    };
    virtual bool inside_local(const Eigen::Vector3d &point) override {
      bool result=false;
      for(auto&& primitive:*this) {
        result|=primitive->inside(point);
      }
      return result;
    };
  };

}
#endif //KWANTRACE_UNION_H
