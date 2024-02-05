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

#ifndef KWANTRACE_SHADER_H
#define KWANTRACE_SHADER_H

namespace kwantrace {
  /** Represents a shading model. A shading model is handed the
   * object that was hit, all objects in the scene (most likely
   * as a Composite, note that the object that was hit will be
   * in all objects as well), all lights in the scene, the normal
   * at the intersection, the position of the intersection, and
   * the direction of the ray at the intersection.
   *
   * Subclasses will use this data to implement various shading models.
   */
  class Shader {
  public:
    /** Calculate the shade at this point
     *
     * @param[in] object Object being shaded
     * @param[in] scene A composite object containing all objects in the scene
     * @param[in] lightList all the lights in the scene
     * @param[in] r Position of intersection
     * @param[in] v Direction of incoming ray, must be normalized
     * @param[in] n Normal vector, must be normalized
     * @return Color of this ray. This is still a bit of a fuzzy concept, but the R, G, and B colors
     * of the first ray (from the camera) is used to color the pixel in the pixel buffer.
     */
    virtual RayColor shade(
      const Renderable& object,
      const Renderable& scene,
      const LightList& lightList,
      const Position& r,
      const Direction& v,
      const Direction& n
    )const=0;
    /** Prepare for a render. Default implementation doesn't do anything.
     * Subclasses might want to do something. */
    virtual void prepareRender() {};
    virtual ~Shader()=default; ///< Allow there to be subclasses
  };

  /** Represents faked ambient light.
   * In many scenes, there are many large diffuse reflectors around the scene.
   * Think of a scene in a room with white walls. If there is a point light
   * like a lamp in the scene, the lamp will cast shadows, but the shadows will
   * not be completely dark. In reality, this is because some light is
   * scattered off the other objects in the scene and end up in the shadow. This
   * light model is out of our current scope (look up [radiosity](https://en.wikipedia.org/wiki/Radiosity_(computer_graphics))) so instead we
   * say that if a point is in shadow, it gets colored as a fraction of its
   * intrinsic color instead of full darkness. In a certain sense, the object
   * is literally glowing.
   *
   * POV-Ray uses a default of 0.1 for ambient, which is pretty dim but still
   * definitely visible. In POV-Ray, the ambient light is adjustable at the global
   * level and at the per-object level, but is constant over a primitive. We could
   * easily support a color field of ambient, but right now (until we implement
   * per-object finish) everything has a constant nonzero ambient.
   */
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

  /** Represents Lambertian (diffuse) reflection. This represents
   * direct illumination of matte surfaces, and depends only on
   * the angle between the normal and the light source. See
   * [Wikipedia](https://en.wikipedia.org/wiki/Lambertian_reflectance)
   * for details.
   *
   * A pure Lambertian reflector looks the same from every angle, IE the
   * radiance from the reflector is the same. A certain amount of
   * energy is hitting the reflector at an angle, and this in effect
   * smears out the energy.
   *
   * Considering any particular infinitesimally
   * small light beam: If all of the energy of the light hits a flat reflector,
   * the differential area element that is hit is the cross section
   * of the beam times divided by the cosine of the angle. The angle
   * is equal to the angle of the original beam if it hits straight on,
   * and increases ultimately to infinity when it hits parallel to the
   * surface. Since the energy is the same and the area is larger,
   * the irradiance at any point in the area is less.
   *
   * Working through
   * the trigonometry reveals that the area hit is the area of the beam
   * *divided* by the cosine of the angle from the normal, and that
   * therefore the irradiance is the irradiance of the original beam
   * *times* the cosine of the angle. This would predict full brightness
   * when the light is coming in along the normal, varying smoothly
   * towards zero as the angle increases to 90&deg;
   *
   * Note that Lambertian reflectance, IE equal radiance when *viewing*
   * from any angle, is an axiom. It's not a physical law requiring
   * any material to follow. A material is *modeled* as having it.
   * No real material is perfectly Lambertian, (no real material is
   * perfectly *anything*) so this model is accurate solely to the extent
   * that the surface actually matches this property. Having said that,
   * freshly fallen snow and charcoal are described as near-perfect
   * light and dark Lambertian reflectors, respectively.
   *
   * POV-Ray has a per-finish setting for diffuse reflection. By default
   * that diffuse reflection is Lambertian, but it can be modified by the
   * `brilliance` keyword. We don't model that yet. The diffuse coefficient
   * is constant in any given finish in POV-Ray, but we can imagine using
   * a ColorField to represent this coefficient along with any other when
   * we get to doing finishes. For now, we use a constant 1.0 brightness.
   * If this is combined with ambient, you could potentially get a super-saturated
   * 1.1 brightness at a point from a single light.
   */
  class DiffuseShader:public Shader {
  public:
    /** \copydoc Shader::shade()
     *
     * In order to calculate the Lambertian reflectance, we need to know the
     * angle between each light and the local normal. We also need to know
     * whether a given light is blocked. In order to do each of these things,
     * we do the following for each light:
     *    * Construct a ray from the current intersection to the light
     *    * Determine if the light is visible. The Light object returns
     *      a floating-point number anticipating non-point-lights,
     *      but a point light returns either 1.0 (not blocked) or
     *      0.0 (blocked)
     *    * If the visibility is 0.0, the light is blocked and
     *      contributes exactly zero color.
     *    * Otherwise:
     *        * determine the angle between the light and the normal
     *        * Run the Lambertian model to calculate the reflectance
     *        * Scale the reflectance by the color of the light, the visiblity,
     *           and the intrinsic color at this point.
     * The diffuse shade is the vector sum of all of the lights.
     */
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
            double dot=n.dot(r_light.v.normalized());
            if(dot>0) {
              result+=(dot*objectColor.array()*light->color.array()).matrix().head<3>();
            }
          }
        }
      }
      return result;
    }
  };

  /** Represents a list of shaders. This makes it cleaner to separate each
   *  of the shading models, then run them all consecutively and add them up.
   */
  class CompositeShader:public Shader {
  private:
    std::vector<std::shared_ptr<Shader>> shaderList; ///< List of shaders
  public:
    /** Prepare this shader for rendering. This calls the superclass
     * method, then calls the prepareRender method for each child.
     */
    virtual void prepareRender() override {
      Shader::prepareRender();
      for(auto shader:shaderList) {
        shader->prepareRender();
      }
    };
    /** Add a shader to this object
     *
     * @param shader Pointer to shader to add
     * @return the input pointer
     */
    virtual std::shared_ptr<Shader> add(std::shared_ptr<Shader> shader) {
      shaderList.push_back(shader);
      return shader;
    }
    /** \copydoc Shader::shade()
     *
     * This implementation runs each child shader in turn and adds the
     * results together.
     */
    virtual RayColor shade(
            const Renderable& object,
            const Renderable& scene,
            const LightList& lightList,
            const Position& r,
            const Direction& v,
            const Direction& n
    ) const override {
      RayColor result=RayColor::Zero();
      for(auto shader:shaderList) {
        result+=shader->shade(object,scene,lightList,r,v,n);
      }
      return result;
    }
  };

  /** A specialization of the CompositeShader that is intended to fully emulate the
   * POV-Ray shading model, at least to the extent that I use it.
   */
  class POVRayShader:public CompositeShader {
  public:
    POVRayShader() {
      add(std::make_shared<AmbientShader>());
      add(std::make_shared<DiffuseShader>());
    }
  };
}

#endif //KWANTRACE_SHADER_H
