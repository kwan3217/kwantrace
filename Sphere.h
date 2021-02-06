//
// Created by jeppesen on 2/3/21.
//

#ifndef KWANTRACE_SPHERE_H
#define KWANTRACE_SPHERE_H

#include "Renderable.h"

namespace kwantrace {
  class Sphere : public Primitive {
  public:
    //! Determine the intersection of a ray and a sphere
    /**
     *
     * @param ray Ray in object coordinates
     * @param t [out] Parameter of ray intersection. Will always be positive, and if there are two intersections, will
     *                always be the nearer of the two that is in front of the camera.
     * @return true if ray intersects sphere, false otherwise
     *
     * Derivation on https://omoikane.kwansystems.org/wiki/index.php/Geolocation#Projection_onto_a_spherical_cloud_deck
     */
    virtual bool intersect_local(const Ray &ray, double &t, std::vector<int> &unused) const override {
      double a = ray.v.dot(ray.v);
      double b = 2 * ray.r0.dot(ray.v);
      double c = ray.r0.dot(ray.r0) - 1;
      double d = b * b - 4 * a * c;
      if (d < 0) return false;
      double q = -(b + (b > 0 ? 1 : -1) * sqrt(d)) / 2;
      double t1 = q / a;
      double t2 = c / q;
      if (t1 < 0) {
        t = t2;
        return t > 0;
      }
      if (t2 < 0) {
        t = t1;
        return t > 0;
      }

      t = (t1 < t2) ? t1 : t2;
      return true;
    }

    virtual DirectionVector normal_local(const PositionVector &point, std::vector<int> &indexes, int level) const override {
      return Eigen::Vector3d(point / point.norm());
    }

    virtual bool inside_local(const PositionVector &point) const override {
      return point.norm() < 1;
    }

    static Eigen::Vector2d uv_local(const PositionVector &point) {
      double lon = atan2(point.y(), point.x());
      if (lon < 0) lon += EIGEN_PI;
      double lat = asin(point.z() / point.norm());
      return Eigen::Vector2d(lon / (2 * EIGEN_PI), (lat / EIGEN_PI) + 0.5);
    }
  };
}
#endif //KWANTRACE_SPHERE_H
