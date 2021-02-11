//
// Created by jeppesen on 2/7/21.
//

#ifndef KWANTRACE_SCENE_H
#define KWANTRACE_SCENE_H

namespace kwantrace {
  template<int pixdepth=3, typename pixtype=uint8_t>
  class PixelBuffer {
  private:
    int _width;
    int _height;
    std::unique_ptr<pixtype[]> _buf;
  public:
    int width() {return _width;};
    int height() {return _height;};
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
    auto get() {return _buf.get();}
  };

  template<int pixdepth=3, typename pixtype=uint8_t>
  class Scene {
  private:
    Union objects;
    LightList lightList;
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Camera> camera;
    /** Render a scene into a given pixelbuf
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
        double y = 0.5-(double(row) + 0.5) / height;
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
    std::shared_ptr<Renderable> addObject(std::shared_ptr<Renderable> object) {
      return objects.addObject(object);
    }
    std::shared_ptr<Light> addLight(std::shared_ptr<Light> light) {
      lightList.push_back(light);
      return light;
    }
    std::shared_ptr<Camera> set(std::shared_ptr<Camera> Lcamera) {
      camera=Lcamera;
      return camera;
    }
    std::shared_ptr<Shader> set(std::shared_ptr<Shader> Lshader) {
      shader=Lshader;
      return shader;
    }
    PixelBuffer<pixdepth,pixtype> render(int width, int height) {
      auto pixbuf = PixelBuffer<pixdepth,pixtype>(width,height);
      render(width, height, pixbuf);
      return pixbuf;
    }
  };
}

#endif //KWANTRACE_SCENE_H
