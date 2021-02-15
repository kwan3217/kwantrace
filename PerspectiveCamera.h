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

#ifndef KWANTRACE_PERSPECTIVECAMERA_H
#define KWANTRACE_PERSPECTIVECAMERA_H

namespace kwantrace {
  /**
   * Perspective camera, a pinhole camera with negative focal length.
   *
   * \image html camera_plane.png
   *
   * First, a bit about the POV-Ray camera model. To generate
   * a ray, the origin is taken as the initial point \f$\vec{r}_0\f$. Then, the
   * direction \f$\vec{v}\f$ is the sum of:
   *    * 100% of the camera direction vector \f$\vec{c}_d\f$
   *    * A fraction of the right vector determined by the x image plane coordinate \f$x\vec{c}_r\f$
   *    * A fraction of the up vector determined by the y image plane coordinate \f$y\vec{c}_u\f$
   *
   * Sweeping over all x and y coordinates from -0.5 to 0.5 will hit every point
   * in a quadrilateral on a plane. The direction vector \f$\vec{c}_d\f$ touches
   * this plane when \f$x=0\f$ and \f$y=0\f$. If the up and right vectors are
   * perpendicular to the direction vector and each other (the usual case) then
   * the quadrilateral is a rectangle and the direction vector is perpendicular
   * to it.
   *
   * It is natural to consider up and right to be the positive directions, and
   * for right to be associated with the conventional horizontal axis x, up
   * with the vertical axis y, and direction with the into-the-screen axis z.
   *
   * Unfortunately all of those constraints describe a left-handed set of vectors.
   * I now know why the default POV-Ray camera is left-handed. I want a right-handed
   * coordinate frame, because the world is right-handed. Not just in KwanTrace,
   * but most external programs I work with are right-handed.
   *
   * So, we have to give up at least one of the natural conventions above. It seems
   * to me that the most sensible one is to consider the +y camera axis to point
   * *down*. It's the smallest change that works, and it makes the program match
   * the traditional top-row-first orientation of most image file formats. Therefore,
   * KwanTrace uses this:
   *
   * \image html kwan_camera_plane.png
   *
   * Note that if you think of x and y being world horizontal axes and z as being vertical,
   * then the default camera is at the origin, looking straight up. North will be down, and
   * east will be right. This hardly matters, since most commonly we will attach a
   * LocationLookat transformation to the camera to point it exactly where we want.
   *
   *
   */
  class PerspectiveCamera : public Camera {
  public:
    Direction right;     ///<Right-pointing basis vector in image plane
    Direction down;      ///<Down-pointing basis vector in image plane
    Direction direction; ///<boresight-pointing basis vector, perpendicular to image plane
    /** Convert a horizontal field-of-view angle to a direction length.
     * See PerspectiveCamera::angle2dir() for derivation. We start from the middle equation
     * and work towards the direction length
     *
     * \f$\begin{eqnarray*}
     * \tan\left(\frac{\alpha}{2}\right)&=&\frac{|\vec{c}_r|}{2|\vec{c}_d|}\\
     * 2|\vec{c}_d|&=&\frac{|\vec{c}_r|}{\left(\frac{\alpha}{2}\right)}\\
     *  |\vec{c}_d|&=&\frac{|\vec{c}_r|}{2\left(\frac{\alpha}{2}\right)}\\
     * \end{eqnarray*}\f$
     *
     * @param angle Full horizontal field of view in degrees (to match POV-Ray)
     * @param rightlen Length of the right vector
     * @return Length of the direction vector
     */
    static double angle2dir(double angle, double rightlen) {
      //http://www.povray.org/documentation/3.7.0/r3_4.html#r3_4_2 from direction_length in default perspective camera
      return rightlen / (2*tand(angle/2));
    }
    /** Convert a direction length into a horizontal field-of-view angle.
     *
     * First, it only makes sense to talk about field of view angles
     * when the down and right vectors are perpendicular to each other
     * and to the direction vector.
     *
     * Looking at the diagram above, we can easily see that the
     * horizontal field of view \f$\alpha\f$ is determined by the right triangle
     * with the direction vector and half of the right vector as
     * legs. The half-field of view is the angle at location between
     * the direction vector and the hypotenuse of this triangle.
     * Therefore the tangent of that angle is equal to the
     * half of the length of right divided by the length of direction.
     *
     * \f$\begin{eqnarray*}
     * \tan\left(\frac{\alpha}{2}\right)&=&\frac{\frac{|\vec{c}_r|}{2}}{|\vec{c}_d|}\\
     *                       &=&\frac{|\vec{c}_r|}{2|\vec{c}_d|}
     * \end{eqnarray*}\f$
     *
     * This equation doesn't directly give us either \f$\alpha\f$ or \f$|\vec{c}_d|\f$
     * but it can easily be solved either way. In this function we need \f$\alpha\f$
     *
     * \f$\begin{eqnarray*}
     * \tan\left(\frac{\alpha}{2}\right)&=&\frac{|\vec{c}_r|}{2|\vec{c}_d|}\\
     *      \frac{\alpha}{2} &=&\tan^{-1}\left(\frac{|\vec{c}_r|}{2|\vec{c}_d|}\right)\\
     *            \alpha     &=&2\tan^{-1}\left(\frac{|\vec{c}_r|}{2|\vec{c}_d|}\right)\\
     * \end{eqnarray*}\f$
     *
     * @param dirlen Direction length
     * @param rightlen Right vector length
     * @return Full horizontal field of view in degrees (to match the POV-Ray convention)
     */
    static double dir2angle(double dirlen, double rightlen) {
      return 2*atand(rightlen /(2*dirlen));
    }
    /** Default camera. This has its axes aligned with the world axes, so:
     *  * Right is pointing east
     *  * Down is pointing north
     *  * Direction is pointing up
     * The camera field of view is square, with a horizontal field of view of \f$2\tan(0.5)\approx 53.13^\circ\f$
     * Think about a camera phone laying on a table pointing at the ceiling.
     */
    PerspectiveCamera():
            right    (Direction(1,0,0)),
            down     (Direction(0,1,0)),
            direction(Direction(0,0,1)) {
    };

    /** Camera intended to be used with an image buffer of a given size.
     * We adjust the length of the right vector to make the camera plane
     * a rectangle with an aspect ratio matching the given image aspect
     * ratio.
     * @param width Width of image buffer
     * @param height Height of image buffer
     */
    PerspectiveCamera(double width, double height):
            right    (Direction(width/height, 0,0)),
            down     (Direction(0,1,0)),
            direction(Direction(0,0,1)) {

    };
    /** Camera intended to be used with an image buffer of a given size.
     * We adjust the length of the right vector to make the camera plane
     * a rectangle with an aspect ratio matching the given image aspect
     * ratio. We then adjust the length of the direction vector such
     * that the horizontal field of view is the requested value.
     * @param width Width of image buffer
     * @param height Height of image buffer
     * @param angle Full horizontal field of view in degrees
     */
    PerspectiveCamera(double width, double height, double angle):
            right    (Direction(width/height, 0,0)),
            down     (Direction(0, 1, 0)),
            direction(Direction(0,0,angle2dir(angle, width / height))) {
    };
    /** Camera with specified camera vectors. Generally these should be mutually
     * perpendicular, but the camera will still work if they aren't (just don't
     * be surprised when you get weird skewing in your image).
     * @param Lright Right camera vector
     * @param Ldown Down camera vector
     * @param Ldirection Direction camera vector
     */
    PerspectiveCamera(Direction &Lright, Direction &Ldown,Direction &Ldirection):
            right(Lright),
            down(Ldown),
            direction(Ldirection) {

    };
  protected:
    /** Project the ray. Once we have figured out the camera vectors,
     * very little computation is required to actually figure the ray
     * for a given point on the camera plane -- its just the sum of
     * (scaled) basis vectors.
     * @param x Camera plane horizontal coordinate, ranging from -0.5 (left) to 0.5 (right)
     * @param y Camera plane horizontal coordinate, ranging from -0.5 (top) to 0.5 (bottom)
     * @return Ray in local frame through that point on the camera plane
     */
    virtual Ray projectLocal(double x, double y) const override {
      Ray result;
      result.v = static_cast<Direction>(direction + right * x + down * y);
      return result;
    }
  };
}

#endif //KWANTRACE_PERSPECTIVECAMERA_H
