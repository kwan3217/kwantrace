//
// Created by jeppesen on 2/6/21.
//

#ifndef KWANTRACE_SHADER_H
#define KWANTRACE_SHADER_H

namespace kwantrace {
  class Shader {
  private:
  public:
    /**
     * Calculate the shade at this point
     * @return color produced by this shader
     */
    virtual RayColor shade(
            const Renderable& object,   ///<[in] Object being shaded
            const Renderable& scene,    ///<[in] Intended to be a composite object containing all objects in the scene
            const LightList& lightList, ///<[in] all the lights in the scene
            const Position& r,          ///<[in] Position of intersection
            const Direction& v,         ///<[in] Direction of incoming ray, must be normalized
            const Direction& n          ///<[in] Normal vector, must be normalized
    )const=0;
    virtual void prepareRender() {};
  };

  class AmbientShader:public Shader {
  public:
    virtual RayColor shade(
            const Renderable& object,
            const Renderable& scene,
            const LightList& lightList,
            const Position& r,
            const Direction& v,
            const Direction& n
    ) const override {
      RayColor result=RayColor::Zero();
      ObjectColor objectColor;
      if(object.evalPigment(r,objectColor)) {
        result=0.1 * objectColor.head<3>();
      }
      return result;
    }
  };

  class DiffuseShader:public Shader {
  public:
    virtual RayColor shade(
            const Renderable& object,
            const Renderable& scene,
            const LightList& lightList,
            const Position& r,
            const Direction& v,
            const Direction& n
    ) const override {
      RayColor result=RayColor::Zero();
      ObjectColor objectColor;
      if(object.evalPigment(r,objectColor)) {
        for(auto&& light:lightList) {
          Ray r_light=light->rayTo(r);
          double lightVisible=light->amountVisible(scene,r_light);
          if(lightVisible>0) {
            double dot=n.dot(r_light.v);
            if(dot>0) {
              result+=(dot*objectColor.array()*light->color.array()).matrix().head<3>();
            }
          }
        }
      }
      return result;
    }
  };

  class CompositeShader:public Shader {
  private:
    std::vector<std::shared_ptr<Shader>> shaderList;
  public:
    virtual void prepareRender() override {
      for(auto shader:shaderList) {
        shader->prepareRender();
      }
    };
    virtual std::shared_ptr<Shader> add(std::shared_ptr<Shader> shader) {
      shaderList.push_back(shader);
      return shader;
    }
    virtual RayColor shade(
            const Renderable& object,
            const Renderable& objects,
            const LightList& lights,
            const Position& r,
            const Direction& v,
            const Direction& n
    ) const override {
      RayColor result=RayColor::Zero();
      for(auto shader:shaderList) {
        result+=shader->shade(object,objects,lights,r,v,n);
      }
      return result;
    }
  };

  class POVRayShader:public CompositeShader {
  public:
    POVRayShader() {
      add(std::make_shared<AmbientShader>());
      add(std::make_shared<DiffuseShader>());
    }
  };
}

#endif //KWANTRACE_SHADER_H
