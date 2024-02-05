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

#ifndef KWANTRACE_SCENE_H
#define KWANTRACE_SCENE_H

namespace kwantrace {
  /** Pixel buffer
   *
   * @tparam pixdepth Number of color channels. Three is typical color, but I can imagine less for native
   *                  grayscale or to try to get a performance improvement. I can also imagine more for
   *                  simulating an imaging spectrometer.
   * @tparam pixtype Type of one channel of one pixel
   */
  template<int pixdepth=3, typename pixtype=uint8_t>
  class PixelBuffer {
  private:
    int _width;  ///< Width of pixel buffer in pixels
    int _height; ///< Height of pixel buffer in pixels
    std::unique_ptr<pixtype[]> _buf; ///< Actual pixel buffer -- allocated on construction, deallocated at destruction
  public:
    /**Get width of pixel buffer.
     * @return width of pixel buffer in pixels */
    int width() {return _width;};
    /**Get height of pixel buffer.
     * @return height of pixel buffer in pixels */
    int height() {return _height;};
    /** Construct a pixel buffer with the given size
     * @param Lwidth Width in pixels
     * @param Lheight Height in pixels  */
    PixelBuffer(int Lwidth, int Lheight):_width(Lwidth),_height(Lheight) {
      _buf=std::make_unique<pixtype[]>(_width*_height*pixdepth);
    }
    /** Flatten coordinates of a pixel buffer
     *
     * @param[in] col Column index
     * @param[in] row Row index
     * @param[in] channel Channel index
     * @return lvalue (writable) reference to the correct cell of the pixel buffer
     */
    pixtype& operator()(int col, int row, int channel) {
      return _buf[((row*_width)+col)*pixdepth+channel];
    }

    Observer<pixtype> get() {return _buf.get();} ///< Get a pointer to the pixels @return pointer to the pixels
  };

  /** Manager for the whole rendering process. Your code is responsible
   * for loading the scene with objects, lights, a camera, etc. Once
   * everything is in place, you call the Scene::render() method to
   * actually do the rendering.
   *
   * 1. Set up the scene (done by the caller)
   *    * Set up renderable objects
   *        * Set the textures etc for each object
   *    * Set up lights
   *    * Set up camera
   * 1. Render the scene (done in `kwantrace::Scene::render()`)
   *    1. Call `prepareRender()` on all objects, lights, shaders, etc. This fills caches, concatenates transformation
   *       matrices, etc. Do everything possible before the render, so that it doesn't have to be done for each pixel.
   *    1. Loop over each pixel in the image. For each pixel:
   *        1. Determine the pixel ray
   *        1. Intersect it with the scene and find the nearest object.
   *        1. Run the shader to figure out what color the object will be. This may itself
   *            spawn shadow rays, reflected rays, refracted rays, etc.
   *        1. Save the color in the pixel buffer
   *
   * This can be done in a loop -- set up the scene, render it, change the scene, re-render it, etc.
   *
   * @tparam pixdepth Number of color channels. Three is typical color, but I can imagine less for native
   *                  grayscale or to try to get a performance improvement. I can also imagine more for
   *                  simulating an imaging spectrometer.
   * @tparam pixtype Type of one channel of one pixel
   */
  template<int pixdepth=3, typename pixtype=uint8_t>
  class Scene {
  private:
    Union objects;          ///< All objects in the scene
    LightList lightList;    ///< All lights in the scene
    std::shared_ptr<Shader> shader; ///< Shader to use
    std::shared_ptr<Camera> camera; ///< Camera to use
    /** Render a scene into a given pixelbuf. This covers converting a pixel coordinate
     * to a coordinate in the normalized image plane, then calls renderPixel to actually
     * do the work.
     *
     * If you wanted to add multithreading, this is the place to do it. All methods
     * are intended to be thread safe by only using const methods on the scene and
     * its children once rendering has begun, and by only writing to the pixel buffer.
     *
     * @param[in] width Width of target image in pixels
     * @param[in] height Height of target object in pixels
     * @param[in] pixbuf Pixel buffer -- a 1D array of pixels, which will be internally used as
     *    a 2D row-major array (IE rows are contiguous in memory)
     */
    virtual void render(int width, int height, PixelBuffer<pixdepth,pixtype>& pixbuf) {
      objects.prepareRender();
      for(auto&& light:lightList) light->prepareRender();
      shader->prepareRender();
      camera->prepareRender();
      for (int row = 0; row < height; row++) {
        double y = (double(row) + 0.5) / height-0.5;
        for (int col = 0; col < width; col++) {
          double x = (double(col) + 0.5) / width - 0.5;
          renderPixel(x, y, col, row, pixbuf);
        }
      }
    }
    /** Render a pixel. This creates a ray, checks the ray for intersections against the scene,
     * and runs the shader on the correct intersection (which might itself spawn rays)
     */
    void renderPixel(
      double x,                             ///<[in] horizontal coordinate on the camera plane, intended to run from (0.5,0.5)
      double y,                             ///<[in] horizontal coordinate on the camera plane, *also* intended to run from (0.5,0.5)
      int col,                              ///<[in] column in pixel buffer
      int row,                              ///<[in] row in pixel buffer
      PixelBuffer<pixdepth,pixtype>& pixbuf ///<[in] pixel buffer to render into
    ) {
      Ray ray = camera->project(x, y);
      double t;
      Observer<Primitive> finalObject=objects.intersect(ray, t);
      if(finalObject) {
        Position r=ray(t);
        RayColor color=shader->shade(*finalObject,objects,lightList,r,ray.v.normalized(),finalObject->normal(r));
        for(int i=0;i<pixdepth;i++) {
          if(color[i]<=0) {
            pixbuf(col, row, i)=0;
          } else if (color[i]>=1.0) {
            pixbuf(col, row, i)=std::numeric_limits<pixtype>::max();
          } else {
            pixbuf(col, row, i)=pixtype(color[i] * std::numeric_limits<pixtype>::max());
          }
        }
      }
    }
  public:
    /** Add an object to the scene. This just forwards the object to
     * the underlying Union member field representing the scene.
     * @param object Pointer to object to add
     * @return Same pointer is passed back out
     */
    std::shared_ptr<Renderable> add(std::shared_ptr<Renderable> object) {
      return objects.add(object);
    }
    /** Add a light to the scene. This just forwards the object to
     * the underlying light list member field.
     * @param light Pointer to light to add
     * @return Same pointer is passed back out
     */
    std::shared_ptr<Light> add(std::shared_ptr<Light> light) {
      lightList.push_back(light);
      return light;
    }
    /** Set the scene camera.
     * @param Lcamera Pointer to camera
     * @return Same pointer is passed back out
     */
    std::shared_ptr<Camera> set(std::shared_ptr<Camera> Lcamera) {
      camera=Lcamera;
      return camera;
    }
    /** Set the scene shader.
     * @param Lshader Pointer to shader
     * @return Same pointer is passed back out
     */
    std::shared_ptr<Shader> set(std::shared_ptr<Shader> Lshader) {
      shader=Lshader;
      return shader;
    }
    /** Render the scene. This function creates a pixbuf of the appropriate size and
     * delegates actual rendering to render(int,int,PixelBuffer)
     * @param width Width of image in pixels
     * @param height Height of image in pixels
     * @return Pixel buffer
     */
    PixelBuffer<pixdepth,pixtype> render(int width, int height) {
      auto pixbuf = PixelBuffer<pixdepth,pixtype>(width,height);
      render(width, height, pixbuf);
      return pixbuf;
    }
  };
}

#endif //KWANTRACE_SCENE_H
