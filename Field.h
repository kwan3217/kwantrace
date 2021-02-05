//
// Created by jeppesen on 2/4/21.
//

#ifndef KWANTRACE_FIELD_H
#define KWANTRACE_FIELD_H

#include <eigen3/Eigen/Dense>
#include "common.h"

namespace kwantrace {
  template<int N, typename T=double>
  class Field {
  private:
    typedef Eigen::Matrix<T,N,1> OutVector;
  protected:
    virtual OutVector field_local(const PositionVector& r)=0;
  public:
    //! Transformation chain
    TransformChain transformChain;
    void prepareRender() {
      transformChain.prepareRender();
    }
    virtual OutVector operator()(const PositionVector& r) {return field_local(transformChain.Mw2b*r);};
    virtual ~Field() {};
    Eigen::Matrix<T,N,1> operator()(double x, double y, double z) {return *this(PositionVector(x, y, z));};
  };

  class ConstantColor: public ColorField {
  private:
    ObjectColor value;
    virtual ObjectColor field_local(const PositionVector& r) override {return value;}
  public:
    ConstantColor(ObjectColor Lvalue):value(Lvalue) {};
    ConstantColor(double r=0, double g=0, double b=0, double f=0, double t=0) {value<< r,g,b,f,t;};
  };
}

#endif //KWANTRACE_FIELD_H
