//
// Created by jeppesen on 2/3/21.
//

#ifndef KWANTRACE_SPHERE_H
#define KWANTRACE_SPHERE_H

namespace kwantrace {
  /** Represent a sphere. The body coordinate frame has:
   *    * Sphere has radius 1
   *    * Sphere is centered on origin.
   *    * For UV purposes, the sphere has a polar axis
   *      parallel to the Z axis with +Z north, with the
   *      prime meridian on the +X axis and longitude
   *      increasing to the east.
   */
  class Sphere : public Primitive {
  public:
    /** Determine the intersection of a rayLocal and a sphere.
     *
     * @param rayLocal Ray in object coordinates
     * @param t [out] Parameter of rayLocal intersection. Will always be positive, and if there are two intersections, will
     *                always be the nearer of the two that is in front of the camera.
     * @return true if rayLocal intersects sphere in front of camera, false otherwise
     *
     * Everyone knows what a circle is -- a circle is round. But what does that mean in a mathematical sense?
     * A wheel is round if it can roll, and a wheel will roll most smoothly if its axle is always the same difference
     * above the ground. So, we can define a circle as the 2D shape with all points the same distance from the axle.
     * Similarly we can say a sphere is the 3D shape that has all of the points some distance from some center point.
     *
     * Now we have something we can grab on to mathematically. We know the distance formula \f$x^2+y^2=r^2\f$, and
     * it turns out that this is already almost perfect for our level curve form. A sphere similarly is \f$x^2+y^2+z^2=r^2\f$.
     *
     * Since we can do arbitrary scaling and transformation outside, we can consider the One True Sphere which sits
     * at the origin and lords over the \f$r^2=1\f$ surface. So, we have our equation
     *
     *    * \f$x^2+y^2+z^2-1=0\f$
     *
     * and we plug in our x,y,z from the ray equation:
     *
     *    * \f$(x_0+v_xt)^2+(y_0+v_yt)^2+(z_0+v_zt)^2-1=0\f$
     *
     * It's pretty complicated, and it will get worse before it gets better, but it will get better. All of the
     * axes have a similar term to expand, so considering arbitrary axis \f$j=x,y,z\f$
     *
     *    *\f$\begin{eqnarray*}
     *    (j_0+v_jt)^2 &=&j_0^2+j_0v_jt+j_0v_jt+v_j^2t^2 \\
     *     &=&j_0^2+2j_0v_jt+v_j^2t^2 \end{eqnarray*}\f$
     *
     * Plugging that in for each coordinate, we get:
     *
     *    *\f$\begin{eqnarray*}
     *    (x_0+v_xt)^2+(x_0+v_xt)^2+(x_0+v_xt)^2-1^2 &=&0 \\
     *    0&=&x_0^2+2x_0v_xt+v_x^2t^2 + y_0^2+2y_0v_yt+v_y^2t^2 + z_0^2+2z_0v_zt+v_z^2t^2-1\end{eqnarray*}\f$
     *
     * This is the point of maximum badness -- it either stays the same or gets simpler from here on. Since we know that
     * we are trying to solve for \f$t\f$, let's try to treat everything else as coefficients and group the terms by
     * powers of \f$t\f$:
     *
     *    *\f$\begin{eqnarray*}
     *    0&=&x_0^2+2x_0v_xt+v_x^2t^2 + y_0^2+2y_0v_yt+v_y^2t^2 + z_0^2+2z_0v_zt+v_z^2t^2-1 \\
     *     &=&+v_x^2t^2+v_y^2t^2+v_z^2t^2+2x_0v_xt+2y_0v_yt+2z_0v_zt+x_0^2 + y_0^2 + z_0^2-1 \\
     *     &=&+(v_x^2+v_y^2+v_z^2)t^2+2(x_0v_x+y_0v_y+z_0v_z)t+(x_0^2 + y_0^2 + z_0^2-1)\end{eqnarray*}\f$
     *
     * Now we can just read off the coefficients and wash it through the quadratic formula:
     *
     *    *\f$\begin{eqnarray*}
     *    0&=&(v_x^2+v_y^2+v_z^2)t^2+2(x_0v_x+y_0v_y+z_0v_z)t+(x_0^2 + y_0^2 + z_0^2-1) \\
     *    a&=&v_x^2+v_y^2+v_z^2 \\
     *    b&=&2(x_0v_x+y_0v_y+z_0v_z) \\
     *    c&=&x_0^2 + y_0^2 + z_0^2-1\end{eqnarray*}\f$
     *
     * In fact, we can take it a bit further if we recognize that the dot product is the sum of the element-wise
     * products of the input vectors. There are dot products in each coefficient:
     *
     *    *\f$\begin{eqnarray*}
     *    a&=&v_x^2+v_y^2+v_z^2 \\
     *     &=&\vec{v}\cdot\vec{v} \\
     *    b&=&2(x_0v_x+y_0v_y+z_0v_z) \\
     *     &=&2(\vec{r}_0\cdot\vec{v}) \\
     *    c&=&x_0^2 + y_0^2 + z_0^2-1 \\
     *     &=&\vec{r}_0\cdot\vec{r}_0-1\end{eqnarray*}\f$
     *
     * These forms are simpler and make use of Jeppesen's law of Signs. Now we dump this through the best actual
     * computational form of the quadratic formula (See Numerical Recipes in C):
     *
     *    *\f$\begin{eqnarray*}
     *    d&=&b^2-4ac \\
     *    q&=&-\frac{b + \operatorname{sgn}(b)\sqrt{d}}{2}\\
     *    t_1=\frac{q}{a} \\
     *    t_2=\frac{c}{q}\end{eqnarray*}\f$
     *
     * There are no constraints, so it's just a matter of finding out which \f$t\f$ is smallest. First off,
     * if \f$d \lt 0\f$, there are no real roots, so its a quick exit. If it's positive or zero, then both
     * roots will be real and we return the smallest positive root. We do this with just a chain of if blocks.
     *
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

    /** Normal vector of surface. This shows why we like to work in body coordinates.
     * In this frame, the surface is perpendicular to the radius vector, so we can
     * just use the direction of the radius vector. Furthermore, since we will only be
     * called on a surface point at radius 1, the position *is* the direction. We don't
     * even need to normalize. We just cast the position to be a direction.
     *
     * @param pointLocal Point in body coordinates
     * @return Normal vector at this point
     */
    virtual Direction normalLocal(const Position &pointLocal) const override {
      return Direction(pointLocal);
    }

    /** Determine if a point is inside the object. For a unit sphere,
     * this is just if the point has a distance of less than or greater
     * than 1.
     *
     * @param pointLocal Point in body coordinates
     * @return True if inside sphere, false otherwise
     */
    virtual bool insideLocal(const Position &pointLocal) const override {
      return pointLocal.norm() < 1;
    }

    /** Calculate the UV coordinates on a sphere. In this case, we are
     * going to return the latitude and longitude. The polar axis is
     * the Z axis, with north being in the +Z direction. Longitude will
     * run from 0 on the prime meridian, increasing to the east to reach
     * 1 at the prime meridian on the other side. Latitude will run from 0
     * at the south pole, through 0.5 at the equator to 1.0 at the north
     * pole.
     *
     * @param point Point in body coordinates
     * @return UV vector, with normalized longitude as the U coordinate
     * and normalized latitude as the V coordinate.
     */
    static Eigen::Vector2d uvLocal(const Position &point) {
      double lon = atan2(point.y(), point.x());
      if (lon < 0) lon += EIGEN_PI;
      double lat = asin(point.z() / point.norm());
      return Eigen::Vector2d(lon / (2 * EIGEN_PI), (lat / EIGEN_PI) + 0.5);
    }
  };
}
#endif //KWANTRACE_SPHERE_H
