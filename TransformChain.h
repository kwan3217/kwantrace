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
  class TransformChain : public std::vector<std::shared_ptr<Eigen::Affine3d>> {
  private:
  public:
    Eigen::Matrix4d Mb2w;
    Eigen::Matrix4d Mw2b;
    Eigen::Matrix4d Mb2wN;

    Eigen::Matrix4d combine() {
      Eigen::Affine3d result{Eigen::Affine3d::Identity()};
      for (auto&& trans:*this) {
        result = (*trans) * result;
      }
      return result.matrix();
    }

    virtual void prepareRender() {
      Mb2w = combine();
      Mw2b = Mb2w.inverse();
      Mb2wN = Mw2b.transpose();
    }

  };

  class Transformable {
  protected:
    TransformChain transformChain;
  public:
    virtual void prepareRender() {
      transformChain.prepareRender();
    }
    virtual std::shared_ptr<Eigen::Affine3d> addTransform(std::shared_ptr<Eigen::Affine3d> transform) {
      transformChain.push_back(transform);
      return transform;
    }
    //! POV-Ray like translation operation
    /**
     *
     * @param[in] point Vector to move the object. This is in the physical sense -- an object which was at the origin
     *   will be at point after this operation
     */
    std::shared_ptr<Eigen::Affine3d> translate(const PositionVector &point) {
      return addTransform(std::make_unique<Eigen::Affine3d>(Eigen::Translation3d(point)));
    }

    std::shared_ptr<Eigen::Affine3d> translate(double x, double y, double z) {
      return translate(PositionVector(x, y, z));
    }

    //! POV-Ray like rotation operation
    /**
     *
     * @param point rotation vector. Each component represents a rotation around the corresponding axis, applied in
     *    x,y,z order. Each component is the magnitude of the rotation in degrees. Note that since Kwantrace is
     *    right-handed, a positive component means a physical right-handed rotation. So for instance, if an object
     *    was pointed down the
     */
    std::shared_ptr<Eigen::Affine3d> rotate(const Eigen::Vector3d &point) {
      std::shared_ptr<Eigen::Affine3d> result;
      if (point.x() != 0) {
        result=addTransform(std::make_unique<Eigen::Affine3d>(Eigen::AngleAxis(deg2rad(point.x()), PositionVector(1, 0, 0))));
      }
      if (point.y() != 0) {
        result=addTransform(std::make_unique<Eigen::Affine3d>(Eigen::AngleAxis(deg2rad(point.y()), PositionVector(0, 1, 0))));
      }
      if (point.z() != 0) {
        result=addTransform(std::make_unique<Eigen::Affine3d>(Eigen::AngleAxis(deg2rad(point.z()), PositionVector(0, 0, 1))));
      }
      return result;
    }

    std::shared_ptr<Eigen::Affine3d> rotate(double x, double y, double z) {
      return rotate(Eigen::Vector3d(x, y, z));
    }

    std::shared_ptr<Eigen::Affine3d> scale(const Eigen::Vector3d &point) {
      return addTransform(std::make_unique<Eigen::Affine3d>(Eigen::Scaling(point.x(), point.y(), point.z())));
    }

    std::shared_ptr<Eigen::Affine3d> scale(double x, double y, double z) {
      return scale(Eigen::Vector3d(x == 0 ? 1 : x, y == 0 ? 1 : y, z == 0 ? 1 : z));
    }

    /*
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
     */
  };
}

#endif //KWANTRACE_TRANSFORMCHAIN_H
