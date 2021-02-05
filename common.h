//
// Created by jeppesen on 2/4/21.
//

#ifndef KWANTRACE_COMMON_H
#define KWANTRACE_COMMON_H

#include <eigen3/Eigen/Dense>

namespace kwantrace {
  constexpr static const double pi = 3.1415926535897932;

  inline double deg2rad(double deg) { return deg * pi / 180.0; }

  inline double rad2deg(double rad) { return rad * 180.0 / pi; }

  inline Eigen::Vector3d mul0(Eigen::Matrix4d M, Eigen::Vector3d v) {
    Eigen::Vector4d vv;
    vv << v, 0;
    return (M * vv).head<3>();
  }

  inline Eigen::Vector3d mul1(Eigen::Matrix4d M, Eigen::Vector3d v) {
    Eigen::Vector4d vv;
    vv << v, 1;
    return (M * vv).head<3>();
  }


}
#endif //KWANTRACE_COMMON_H
