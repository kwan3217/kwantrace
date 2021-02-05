//
// Created by jeppesen on 2/4/21.
//

#ifndef KWANTRACE_CAMERA_H
#define KWANTRACE_CAMERA_H
#include <memory>

namespace kwantrace {

  template<typename pixtype=uint8_t>
  class Camera {
  protected:
    virtual Ray project_local(double x, double y) = 0;

    virtual void prepareRender() {
      transformChain.prepareRender();
    }

    pixtype render_pixel(const Primitive& scene, double x, double y, std::vector<int>& indexes) {
      Ray ray = project(x, y);
      double t;
      indexes.clear();
      if(scene.intersect(ray, t, indexes)) {
        return pixtype(t*10);
      } else {
        return 0;
      }
    }

  public:
    TransformChain transformChain;

    Ray project(double x, double y) {
      return transformChain.Mb2w * project_local(x, y);
    }
    //! Render a scene into a given pixelbuf
    /**
     *
     * @param[in] scene primitive to render. It's called "scene" because the primitve is expected to be a composite object.
     * @param[in] width Width of target image in pixels
     * @param[in] height Height of target object in pixels
     * @param[in] pixbuf Pixel buffer -- a 1D array of pixels, which will be internally used as
     *    a 2D row-major array (IE rows are contiguous in memory)
     */
    virtual void render(Primitive& scene, int width, int height, pixtype pixbuf[]) {
      prepareRender();
      scene.prepareRender();
      std::vector<int> indexes;
      for (int row = 0; row < height; row++) {
        double y = 0.5-(double(row) + 0.5) / height;
        for (int col = 0; col < width; col++) {
          double x = (double(col) + 0.5) / width - 0.5;
          pixbuf[row*width+col]=render_pixel(scene,x,y,indexes);
        }
      }
    }

    std::unique_ptr<pixtype[]> render(Primitive& scene, int width, int height) {
      std::unique_ptr<pixtype[]> pixbuf = std::unique_ptr<pixtype[]>(new pixtype[width * height]);
      render(scene, width, height, pixbuf.get());
      return pixbuf;
    }
  };
}

#include "PerspectiveCamera.h"

#endif //KWANTRACE_CAMERA_H
