//
// Created by jeppesen on 10/6/21.
//

#ifndef KWANTRACE_PLANE_H
#define KWANTRACE_PLANE_H

namespace kwantrace {
/** Represents a plane. Like in POV-Ray, this renders as an infinite flat 2D plane, and
 * combines in CSG like a half-space. It has a well-defined inside and outside, with all
 * points on one side of the surface being inside.
 */
  class Plane : public Primitive {
    /** Intersect a ray with the plane z=0.
     * @param[in] rayLocal Ray already transformed into local space
     * @param[out] t ray parameter of intersection, unspecified if function returns false
     * @return true if ray hits plane, false otherwise
     * \see kwantrace::Primitive::intersectLocal()
     *
     * For a plane, our surface equation is super-simple, just:
     *
     *    * \f$f(\vec{r})=r_z\f$
     *
     * which gives us:
     *
     *    * \f$\begin{eqnarray*}
     *              & &r_z & = & 0  \\
     *        r_{0z}&+&v_zt&=&0 \\
     *              &-&r_{0z} &=&v_zt \\
     *              & &     t &=&-\frac{z_0}{v_z}\end{eqnarray*}\f$
     */
    bool intersectLocal(const kwantrace::Ray &rayLocal, double &t) const override {
      if (rayLocal.v.z() == 0) {
        t = 0;
        return rayLocal.r0.z() == 0;
      }
      t = -rayLocal.r0.z() / rayLocal.v.z();
      return t > 0;
    }

    /**
     * \copydoc kwantrace::Primitive::normalLocal()
     *
     * Note that this function only needs to be valid at the surface.
     * Many primitives have a normal function which is correct at
     * the surface, but returns some value everywhere or almost
     * everywhere in space. This one for instance always returns
     * the \f$\hat{z}\f$ vector, since that is correct anywhere on the plane.
     */
    kwantrace::Direction normalLocal(const kwantrace::Position &rLocal) const override {
      return kwantrace::Direction(0, 0, 1);
    }

    /**   * \copydoc kwantrace::Primitive::insideLocal()
     * One naive way to
     * define "inside" is that if a point is exactly on the plane,
     * it is inside. However, it is much more useful to consider the plane
     * to divide space in half, and to consider one entire half of space
     * to be "inside". Since our plane is at \f$z=0\f$, we will treat any
     * point \f$z<0\f$ as inside the plane. Note that it doesn't really matter
     * if we use \f$z<0\f$ or \f$z\le 0\f$.
     *
     */
    bool insideLocal(const kwantrace::Position &rLocal) const override {
      return rLocal.z() < 0;
    }
  };

}


#endif //KWANTRACE_PLANE_H
