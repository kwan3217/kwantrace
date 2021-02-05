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
    PositionVector r0;
    //! Ray direction
    DirectionVector v;

    Ray(PositionVector Lr0, DirectionVector Lv) : r0(Lr0), v(Lv) {};

    Ray(double x0, double y0, double z0, double vx, double vy, double vz) : r0(PositionVector(x0, y0, z0)),
                                                                            v(DirectionVector(vx, vy, vz)) {}

    Ray() : r0(PositionVector(0, 0, 0)), v(DirectionVector(0, 0, 0)) {}
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
      r0 = M*r0; //Transform the initial point such that this vector *is* subject to translation
      v = M*v; //Transform the direction such that this vector *is not* subject to translation
      return *this;
    }
    //! Evaluate the ray
    /**
     *
     * @param t Parameter to evaluate the ray at
     * @return Point on ray at given parameter
     */
    PositionVector operator()(double t) {
      return static_cast<Eigen::Vector3d>(r0 + v * t);
    }
  };

  inline Ray operator*(const Eigen::Matrix4d &M, Ray ray) {
    ray *= M;
    return ray;
  }
}
#endif //KWANTRACE_RAY_H
