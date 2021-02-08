//
// Created by jeppesen on 2/6/21.
//

#ifndef KWANTRACE_SHADER_H
#define KWANTRACE_SHADER_H

namespace kwantrace {
  /** Emulate POV-Ray ambient shading. The ambient property of an object is uniform throughout space, but is a color
   * vector. The ambient color of this point is the intrinsic color at the point times the ambient constant.
   *
   */
  ObjectColor ambient_shade(const Position& r,          ///<[in] Point in world space
                            const ColorField& intrinsicColor, ///<[in] intrinsic color field
                            const ObjectColor& ambient        ///<[in] ambient value field
  ) {
    return (ambient.array()*intrinsicColor(r).array()).matrix();
  }
  /** Emulate a slightly-extended ambient shading. In this case, we allow the ambient property to vary throughout space.
   *
   */
  ObjectColor ambient_shade(const Position& r,                ///<[in] Point in world space
                            const ColorField& intrinsicColor, ///<[in] intrinsic color field
                            const ColorField& ambient         ///<[in] ambient value field
  ) {
    return (ambient(r).array()*intrinsicColor(r).array()).matrix();
  }
  /**
   * Lambert diffuse shading model
   * @param r
   * @param intrinsicColor
   * @param n
   * @param v_light
   * @return
   */
  ObjectColor diffuse_shade(const Position& r,                ///<[in] Point in world space
                            const ColorField& intrinsicColor, ///<[in] intrinsic color field
                            const Direction& n,
                            const Direction& v_light
  ) {
      double dot=n.dot(v_light);
      if(dot<0) {
        return ObjectColor();
      } else {
        return intrinsicColor(r)*dot;
      }
  };

  class Shader {
  private:
    RenderableList objectList;
    LightList lightList;
  public:
    /**
     * Calculate the shade at this point
     * @return color produced by this shader
     */
    virtual ObjectColor shade(
      const Renderable& object,                          ///<[in] Object being shaded
      const Position& r,                                 ///<[in] Position of intersection
      const Direction& v,                                ///<[in] Direction of incoming ray
      const Direction& n                                 ///<[in] Normal vector
    )=0;
  };

  class AmbientShader:public Shader {
  public:
    virtual ObjectColor shade(
            const Renderable& object,   ///<[in] Object being shaded
            const Position& r,          ///<[in] Position of intersection
            const Direction& v,         ///<[in] Direction of incoming ray
            const Direction& n          ///<[in] Normal vector
    ) {

    }
  };

  class CompositeShader:public Shader {
  private:
    std::vector<std::shared_ptr<Shader>> shaderList;
  public:
    virtual std::shared_ptr<Shader> add(std::shared_ptr<Shader> shader) {
      shaderList.push_back(shader);
      return shader;
    }
    virtual ObjectColor shade(
            const Renderable& object,
            const Renderable& scene,
            const LightList& lights,
            const Position& r,
            const Direction& v,
            const Direction& n
    ) {
      ObjectColor result;
      for(auto shader:shaderList) {
        result+=shader->shade(object,scene,lights,r,v,n);
      }
      return result;
    }
  };
}

#endif //KWANTRACE_SHADER_H
