//
// Created by jeppesen on 2/6/21.
//

#ifndef KWANTRACE_LIGHT_H
#define KWANTRACE_LIGHT_H

namespace kwantrace {
  class Light {
  public:
    static const constexpr double initialDist=1e-6;
    Position location;
    ObjectColor color;
    Light(const Position& Llocation, const ObjectColor& Lcolor):location(Llocation),color(Lcolor) {};
    virtual void prepareRender() {};
    virtual Ray rayTo(const Position& r0) {
      Direction v=Direction(location-r0).normalized();
      return Ray(r0,v)+initialDist;
    }
    virtual double amountVisible(const Renderable& blocker, const Ray& r) {
      double t_dontcare;
      return blocker.intersect(r,t_dontcare)?0.0:1.0;
    }
    virtual double amountVisible(const Renderable& blocker, const Position& r0) {
      return amountVisible(blocker,rayTo(r0));
    }
  };
  typedef std::vector<std::shared_ptr<Light>> LightList;
}


#endif //KWANTRACE_LIGHT_H
