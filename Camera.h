/* KwanTrace - C++ Ray Tracing Library
Copyright (C) 2021 by kwan3217

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

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
    virtual Ray projectLocal(double x, double y) const = 0;
  public:
    /** Create a ray in world space. This is done by
     * having a subclass create a ray in local space, then using
     * \f$[\mathbf{M}_{b2r}]\f$ to transform the ray to world space.
     *
     * @param x horizontal camera plane coordinate, from -0.5 on the left to 0.5 on the right
     * @param y vertical camera plane coordinate, from -0.5 on top to 0.5 on bottom.
     * @return Ray in world coordinates
     */
    Ray project(double x, double y) const {
      return Mb2w * projectLocal(x, y);
    }
  };
}

#include "PerspectiveCamera.h"

#endif //KWANTRACE_CAMERA_H
