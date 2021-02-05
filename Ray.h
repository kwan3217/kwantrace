//
// Created by jeppesen on 2/3/21.
//

#ifndef KWANTRACE_RAY_H
#define KWANTRACE_RAY_H

#include <eigen3/Eigen/Dense>

namespace kwantrace {
  class Ray {
  public:
    //! Ray initial point
    Eigen::Vector3d r0;
    //! Ray direction
    Eigen::Vector3d v;

    Ray(Eigen::Vector3d Lr0, Eigen::Vector3d Lv) : r0(Lr0), v(Lv) {};

    Ray(double x0, double y0, double z0, double vx, double vy, double vz) : r0(Eigen::Vector3d(x0, y0, z0)),
                                                                            v(Eigen::Vector3d(vx, vy, vz)) {}

    Ray() : r0(Eigen::Vector3d(0, 0, 0)), v(Eigen::Vector3d(0, 0, 0)) {}
    //! Transform this ray with a matrix
    /**
     *
     * @param M Matrix
     * @return Reference to this ray
     *
     * Note that the following two expressions are equivalent:
     *   r*=Mb2w;
     *   r=Mb2w*r;
     */
    Ray &operator*=(const Eigen::Matrix4d &M) {
      r0 = mul1(M, r0); //Transform the initial point such that this vector *is* subject to translation
      v = mul0(M, v); //Transform the direction such that this vector *is not* subject to translation
      return *this;
    }
    //! Evaluate the ray
    /**
     *
     * @param t Parameter to evaluate the ray at
     * @return Point on ray at given parameter
     */
    Eigen::Vector3d operator()(double t) {
      return r0 + v * t;
    }
  };

  inline Ray operator*(const Eigen::Matrix4d &M, Ray ray) {
    ray *= M;
    return ray;
  }
}
#endif //KWANTRACE_RAY_H
