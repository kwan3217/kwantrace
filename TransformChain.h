//
// Created by jeppesen on 2/3/21.
//

#ifndef KWANTRACE_TRANSFORMCHAIN_H
#define KWANTRACE_TRANSFORMCHAIN_H

#include <vector>
#include <memory>
#include <eigen3/Eigen/Dense>
#include "common.h"

namespace kwantrace {
  class TransformChain : public std::vector<std::unique_ptr<Eigen::Affine3d>> {
  private:
  public:
    Eigen::Matrix4d M;
    Eigen::Matrix4d Minv;
    Eigen::Matrix4d MinvT;

    Eigen::Matrix4d combine() {
      Eigen::Affine3d result{Eigen::Affine3d::Identity()};
      for (auto&& trans:*this) {
        result = (*trans) * result;
      }
      return result.matrix();
    }

    virtual void prepareRender() {
      M = combine();
      Minv = M.inverse();
      MinvT = Minv.transpose();
    }

    //! POV-Ray like translation operation
    /**
     *
     * @param[in] point Vector to move the object. This is in the physical sense -- an object which was at the origin
     *   will be at point after this operation
     */
    int translate(const Eigen::Vector3d &point) {
      push_back(std::make_unique<Eigen::Affine3d>(Eigen::Translation3d(-point)));
      return size() - 1;
    }

    int translate(double x, double y, double z) {
      return translate(Eigen::Vector3d(x, y, z));
    }
    //! POV-Ray like rotation operation
    /**
     *
     * @param point rotation vector. Each component represents a rotation around the corresponding axis, applied in
     *    x,y,z order. Each component is the magnitude of the rotation in degrees. Note that since Kwantrace is
     *    right-handed, a positive component means a physical right-handed rotation. So for instance, if an object
     *    was pointed down the
     */
    int rotate(const Eigen::Vector3d &point) {
      int result = -1;
      if (point.x() != 0) {
        push_back(std::make_unique<Eigen::Affine3d>(Eigen::AngleAxis(deg2rad(point.x()), Eigen::Vector3d(1, 0, 0))));
        result = size() - 1;
      }
      if (point.y() != 0) {
        push_back(std::make_unique<Eigen::Affine3d>(Eigen::AngleAxis(deg2rad(point.y()), Eigen::Vector3d(0, 1, 0))));
        result = size() - 1;
      }
      if (point.z() != 0) {
        push_back(std::make_unique<Eigen::Affine3d>(Eigen::AngleAxis(deg2rad(point.z()), Eigen::Vector3d(0, 0, 1))));
        result = size() - 1;
      }
      return result;
    }

    int rotate(double x, double y, double z) {
      return rotate(Eigen::Vector3d(x, y, z));
    }

    int scale(const Eigen::Vector3d &point) {
      push_back(std::make_unique<Eigen::Affine3d>(Eigen::Scaling(point.x(), point.y(), point.z())));
      return size() - 1;
    }

    int scale(double x, double y, double z) {
      return scale(Eigen::Vector3d(x == 0 ? 1 : x, y == 0 ? 1 : y, z == 0 ? 1 : z));
    }

    //! Point-toward
    static Eigen::Affine3d
    point_toward(const Eigen::Vector3d &p_b, const Eigen::Vector3d &p_r, const Eigen::Vector3d &t_b,
                 const Eigen::Vector3d &t_r) {

    }

    //! Location-look_at
    static Eigen::Affine3d location_lookat(const Eigen::Vector3d &location, const Eigen::Vector3d &look_at,
                                           const Eigen::Vector3d &sky = Eigen::Vector3d(0, 0, 1)) {
      //Eigen::Affine3d result=point_toward(Eigen)
    }
  };
}

#endif //KWANTRACE_TRANSFORMCHAIN_H
