//
// Created by jeppesen on 2/4/21.
//

#ifndef KWANTRACE_CAMERA_H
#define KWANTRACE_CAMERA_H
#include <memory>
#include <limits>
#include "Ray.h"
#include "Transformable.h"

namespace kwantrace {
  /** Abstract camera class. This class manages mapping from pixel space to normalized camera coordinate space. Subclasses
   * will manage creating rays from camera coordinate space.
   */

  class Camera: public Transformable {
  protected:
    /** Create a ray corresponding to the given point in camera coordinate space, with two dimensions that run from
     *  -0.5 to 0.5
     * @param[in] x Horizontal camera coordinate
     * @param[in] y Vertical camera coordinate
     * @return Ray in camera space
     */
    virtual Ray project_local(double x, double y) const = 0;
  public:
    Ray project(double x, double y) const {
      return Mb2w * project_local(x, y);
    }
  };
}

#include "PerspectiveCamera.h"

#endif //KWANTRACE_CAMERA_H
