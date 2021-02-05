//
// Created by jeppesen on 2/4/21.
//

#ifndef KWANTRACE_COMMON_H
#define KWANTRACE_COMMON_H

#include <eigen3/Eigen/Dense>
#include <numbers>
#include <cmath>

namespace kwantrace {
  inline double deg2rad(double deg) { return deg * std::numbers::pi / 180.0; }

  inline double rad2deg(double rad) { return rad * 180.0 / std::numbers::pi; }

  //Use these sparingly -- prefer radians as native angle unit when possible,
  //but prefer these over ad-hoc radian/degree conversion in the code. As usual,
  //you the user use functions, and let the compiler deal with inlining.
  //These are mostly included to make scene definition language more POV-Ray like.
  inline double sind(double angle) {return std::sin(deg2rad(angle));}
  inline double cosd(double angle) {return std::cos(deg2rad(angle));}
  inline double tand(double angle) {return std::tan(deg2rad(angle));}
  inline double asind(double arg) {return rad2deg(std::asin(arg));}
  inline double acosd(double arg) {return rad2deg(std::acos(arg));}
  inline double atand(double arg) {return rad2deg(std::atan(arg));}
  inline double atan2d(double y, double x) {return rad2deg(std::atan2(y,x));}

  template<int N, typename T> class Field;
  typedef Field<5,double> ColorField;
  typedef Eigen::Matrix<double,5,1> ObjectColor;
  typedef Eigen::Vector3d RayColor;
  template<int N>
  class  PDVector: public Eigen::Vector3d {
  public:
    PDVector() {}
    PDVector(Eigen::Vector3d matrix): Eigen::Vector3d(matrix) {}
    PDVector(double x, double y, double z):Eigen::Vector3d(x,y,z) {}
  };
  typedef PDVector<1> PositionVector;
  typedef PDVector<0> DirectionVector;
  inline Eigen::Vector4d extend(PositionVector v, double w) {
    Eigen::Vector4d result;
    result<<v,w;
    return result;
  }
  inline Eigen::Vector3d deextend(Eigen::Vector4d v) {
    return v.head<3>();
  }

  template<int N>
  inline PDVector<N> operator*(const Eigen::Matrix4d& M, const PDVector<N>& v) {
    return deextend(M * extend(v,N));
  }

}
#endif //KWANTRACE_COMMON_H
