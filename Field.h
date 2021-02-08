//
// Created by jeppesen on 2/4/21.
//

#ifndef KWANTRACE_FIELD_H
#define KWANTRACE_FIELD_H

namespace kwantrace {
  template<int N, typename T=double>
  class Field:public Transformable {
  private:
    typedef Eigen::Matrix<T,N,1> OutVector;
  protected:
    virtual OutVector field_local(const Position& r) const =0;
  public:
    virtual OutVector operator()(const Position& r) const {return field_local(Mw2b * r);};
    virtual ~Field() {};
    Eigen::Matrix<T,N,1> operator()(double x, double y, double z) {return *this(Position(x, y, z));};
  };

  typedef Field<5,double> ColorField;
  class ConstantColor: public ColorField {
  private:
    ObjectColor value;
    virtual ObjectColor field_local(const Position& r) const override {return value;}
  public:
    ConstantColor(ObjectColor Lvalue):value(Lvalue) {};
    ConstantColor(double r=0, double g=0, double b=0, double f=0, double t=0) {value<< r,g,b,f,t;};
  };
}

#endif //KWANTRACE_FIELD_H
