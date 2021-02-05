//
// Created by jeppesen on 2/4/21.
//

#ifndef KWANTRACE_FIELD_H
#define KWANTRACE_FIELD_H

#include <eigen3/Eigen/Dense>

namespace kwantrace {
  template<int N, typename T=double>
  class Field {
  public:
    virtual Eigen::Matrix<T,N,1> operator()(Eigen::Vector3d r)=0;
    virtual Eigen::Matrix<T,N,1> operator()(double x, double y, double z)=0;
  };
}

#endif //KWANTRACE_FIELD_H
