//
// Created by jeppesen on 2/4/21.
//

#ifndef KWANTRACE_CAMERA_H
#define KWANTRACE_CAMERA_H
#include <memory>
#include <limits>
#include "Ray.h"
#include "Transformable.h"

namespace kwantrace {

  template<int pixdepth=3, typename pixtype=uint8_t>
  class Camera: public Transformable {
  protected:
    virtual Ray project_local(double x, double y) = 0;

    static pixtype& pixel(pixtype img[], int width, int col, int row, int channel) {
      return img[((row*width)+col)*pixdepth+channel];
    }

    void render_pixel(const Renderable& scene, double x, double y, std::vector<int>& indexes, int width, int col, int row, pixtype pixbuf[]) {
      Ray ray = project(x, y);
      double t;
      indexes.clear();
      if(scene.intersect(ray, t, indexes)) {
        ObjectColor color;
        if(scene.eval_pigment(ray(t),color, indexes)) {
          for(int i=0;i<pixdepth;i++) {
            if(color[i]<=0) {
              pixel(pixbuf, width, col, row, i)=0;
            } else if (color[i]>=1.0) {
              pixel(pixbuf, width, col, row, i)=std::numeric_limits<pixtype>::max();
            } else {
              pixel(pixbuf, width, col, row, i) = pixtype(color[i] * std::numeric_limits<pixtype>::max());
            }
          }
        }
      }
    }

  public:
    Ray project(double x, double y) {
      return Mb2w * project_local(x, y);
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
    virtual void render(Renderable& scene, int width, int height, pixtype pixbuf[]) {
      prepareRender();
      scene.prepareRender();
      std::vector<int> indexes;
      for (int row = 0; row < height; row++) {
        double y = 0.5-(double(row) + 0.5) / height;
        for (int col = 0; col < width; col++) {
          double x = (double(col) + 0.5) / width - 0.5;
          render_pixel(scene, x, y, indexes, width, col, row, pixbuf);
        }
      }
    }

    std::unique_ptr<pixtype[]> render(Renderable& scene, int width, int height) {
      std::unique_ptr<pixtype[]> pixbuf = std::unique_ptr<pixtype[]>(new pixtype[width * height * pixdepth]);
      render(scene, width, height, pixbuf.get());
      return pixbuf;
    }
  };
}

#include "PerspectiveCamera.h"

#endif //KWANTRACE_CAMERA_H
