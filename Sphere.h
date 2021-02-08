//
// Created by jeppesen on 2/3/21.
//

#ifndef KWANTRACE_SPHERE_H
#define KWANTRACE_SPHERE_H

namespace kwantrace {
  class Sphere : public Primitive {
  public:
    //! Determine the intersection of a rayLocal and a sphere
    /**
     *
     * @param rayLocal Ray in object coordinates
     * @param t [out] Parameter of rayLocal intersection. Will always be positive, and if there are two intersections, will
     *                always be the nearer of the two that is in front of the camera.
     * @return true if rayLocal intersects sphere, false otherwise
     *
     * Derivation on https://omoikane.kwansystems.org/wiki/index.php/Geolocation#Projection_onto_a_spherical_cloud_deck
     */
    virtual bool intersectLocal(const Ray &rayLocal, double &t) const override {
      double a = rayLocal.v.dot(rayLocal.v);
      double b = 2 * rayLocal.r0.dot(rayLocal.v);
      double c = rayLocal.r0.dot(rayLocal.r0) - 1;
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

    virtual Direction normalLocal(const Position &pointLocal) const override {
      return Direction(pointLocal / pointLocal.norm());
    }

    virtual bool insideLocal(const Position &pointLocal) const override {
      return pointLocal.norm() < 1;
    }
    /*
    static Eigen::Vector2d uv_local(const Position &point) {
      double lon = atan2(point.y(), point.x());
      if (lon < 0) lon += EIGEN_PI;
      double lat = asin(point.z() / point.norm());
      return Eigen::Vector2d(lon / (2 * EIGEN_PI), (lat / EIGEN_PI) + 0.5);
    }
     */
  };
}
#endif //KWANTRACE_SPHERE_H
