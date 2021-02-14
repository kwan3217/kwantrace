//
// Created by jeppesen on 2/6/21.
//

#ifndef KWANTRACE_LIGHT_H
#define KWANTRACE_LIGHT_H

namespace kwantrace {
  /** Class describing a light source. This is both a base
   * class, and a concrete implementation of a point light
   * source.
   *
   */
  class Light {
  public:
    static const constexpr double initialDist=1e-6; ///< Yuck! Ugly hack coefficient
    Position location; ///< Position of the light in world coordinates
    ObjectColor color; ///< Color of the light
    /** Construct a light
     * @param Llocation Location of light
     * @param Lcolor Color of light
     */
    Light(const Position& Llocation, const ObjectColor& Lcolor):location(Llocation),color(Lcolor) {}
    virtual ~Light()=default; ///< Allow subclasses

    /** Prepare for render. This class doesn't need to do anything. */
    virtual void prepareRender() {};

    /** Construct a ray from the given position to the light
     *
     * @param r0 Initial position of the light
     * @return A ray with a direction such that t=0 means at the initial surface, and t=1 is at the light
     *
     * Ugly Light Hack:
     *
     * In principle you just draw a ray from the intersection point you just found, to the light. The
     * problem happens at t=0, IE at the intersection. Since you are doing this ray specifically because
     * you found an object intersection at this point, there should be an intersection at exactly
     * t=0. Due to floating point precision, there is roughly a 50% chance that the object
     * will shade itself at this point. So, we advance the ray a small amount, just enough to be solidly
     * off the surface. This advance is controlled by initialDist, which represents the fraction
     * of the distance from the intersection to the light to advance.
     *
     * You can't just say "don't intersect with the same object again" because some primitives
     * like torii and meshes might self-shadow. Maybe you can check if the normal at the intersection
     * is pointing in the right direction, but that seems expensive. Maybe you just check the normal
     * if the intersection is too close.
     */
    virtual Ray rayTo(const Position& r0) {
      Direction v=Direction(location-r0);
      return Ray(r0,v)+initialDist;
    }
    /** Calculate the amount of this light which is visible. For point lights,
     * either it is or it isn't, so you get either exactly 0.0 or 1.0. Other
     * lights might have a different blocking model which will permit partial
     * illumination
     * @param blocker All objects in a scene that might block this light
     * @param r Ray from intersection point to light
     * @return Fraction of this light seen at the original point, IE not blocked.
     *
     * \bug This currently checks all objects along the ray all the way out
     * to infinity. We should only check objects out to t=1, and we should do an early
     * exit if the ray is blocked by anything, rather than try to find the nearest
     * intersection.
     */
    virtual double amountVisible(const Renderable& blocker, const Ray& r) {
      double t_dontcare;
      return blocker.intersect(r,t_dontcare)?0.0:1.0;
    }
    /** Calculate the amount of this light that is visible. See amountVisible(Renderable&,Ray&) for
     * details.
     * @param blocker All objects in a scene that might block this light
     * @param r0
     * @return
     */
    virtual double amountVisible(const Renderable& blocker, const Position& r0) {
      return amountVisible(blocker,rayTo(r0));
    }
  };
  typedef std::vector<std::shared_ptr<Light>> LightList; ///< Type name alias for a collection of lights
}


#endif //KWANTRACE_LIGHT_H
