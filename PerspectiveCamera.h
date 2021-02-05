//
// Created by jeppesen on 2/4/21.
//

#ifndef KWANTRACE_PERSPECTIVECAMERA_H
#define KWANTRACE_PERSPECTIVECAMERA_H

#include "common.h"

namespace kwantrace {
  template<typename pixtype=uint8_t>
  class PerspectiveCamera : public Camera<pixtype> {
  public:
    Eigen::Vector3d direction;
    Eigen::Vector3d right;
    Eigen::Vector3d up;

    PerspectiveCamera() : direction(Eigen::Vector3d(1, 0, 0)), right(Eigen::Vector3d(0, 1, 0)),
                          up(Eigen::Vector3d(0, 0, 1)) {};

    PerspectiveCamera(double width, double height) : direction(Eigen::Vector3d(1, 0, 0)),
                                                     right(Eigen::Vector3d(0, width / height, 0)),
                                                     up(Eigen::Vector3d(0, 0, 1)) {};

    PerspectiveCamera(double width, double height, double angle) : direction(
            Eigen::Vector3d(0.5 * (width / height) / tan(deg2rad(angle) / 2), 0, 0)),
                                                                   right(Eigen::Vector3d(0, width / height, 0)),
                                                                   up(Eigen::Vector3d(0, 0, 1)) {};

    PerspectiveCamera(Eigen::Vector3d &Ldirection, Eigen::Vector3d &Lright, Eigen::Vector3d &Lup) : direction(
            Ldirection), right(Lright), up(Lup) {};
  protected:
    virtual Ray project_local(double x, double y) {
      Ray result;
      result.v = direction + right * x + up * y;
      return result;
    }
  };
}

#endif //KWANTRACE_PERSPECTIVECAMERA_H
