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
    typedef Eigen::Vector3d InVector;
    typedef Eigen::Matrix<T,N,1> OutVector;
  protected:
    virtual OutVector field_local(const InVector& r)=0;
  public:
    //! Transformation chain
    TransformChain transformChain;
    void prepareRender() {
      transformChain.prepareRender();
    }
    virtual OutVector operator()(const InVector& r) {return field_local(mul1(transformChain.Mw2b, r));};
    virtual ~Field() {};
    Eigen::Matrix<T,N,1> operator()(double x, double y, double z) {return *this(InVector(x,y,z));};
  };
  typedef Field<5,double> ColorField;

  class ConstantColor: public ColorField {
  private:
    typedef Eigen::Vector3d InVector;
    typedef Eigen::Matrix<double,5,1> OutVector;
    OutVector value;
    virtual OutVector field_local(const InVector& r) override {return value;}
  public:
    ConstantColor(OutVector Lvalue):value(Lvalue) {};
    ConstantColor(double r, double g, double b, double f, double t) {value<< r,g,b,f,t;};
  };
}

#endif //KWANTRACE_FIELD_H
