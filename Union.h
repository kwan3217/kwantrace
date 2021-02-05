//
// Created by jeppesen on 2/4/21.
//

#ifndef KWANTRACE_UNION_H
#define KWANTRACE_UNION_H

#include "Primitive.h"

namespace kwantrace {

  class Union : public CompositePrimitive {
  protected:
    virtual bool intersect_local(const Ray &ray, double &t, vector<int> &indexes) const override {
      bool result=false;
      t=std::numeric_limits<double>::infinity();
      int i=0;
      int index=-1;
      for(auto&& primitive:*this) {
        double this_t;
        if(primitive->intersect(ray, this_t, indexes)) {
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
    virtual Eigen::Vector3d normal_local(const Eigen::Vector3d &point, std::vector<int> &indexes, int level=-1) override {
      if(level<0) {
        level=indexes.size()-1;
      }
      if(indexes[level]>0) {
        return (*this)[indexes[level]]->normal(point,indexes,level-1);
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
