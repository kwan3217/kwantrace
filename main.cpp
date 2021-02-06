#include <memory>
#include <fstream>
#include <cstdio>
#include "kwantrace.h"

int main() {
  const int width=1920;
  const int height=1080;
  std::ofstream ouf;
  ouf.open("image.pgm",std::ios::out|std::ios::trunc);
  ouf << "P6" << std::endl;
  ouf << width << " " << height << std::endl;
  ouf << 255 << std::endl;
  ouf.close();

  kwantrace::Union scene;
  scene.addChild(std::make_shared<kwantrace::Sphere>());
  scene.last().translate(5,0,0);
  scene.last().set_pigment(std::make_shared<kwantrace::ConstantColor>(1,0,0));

  scene.addChild(std::make_shared<kwantrace::Sphere>());
  scene.last().scale(0.5,0.5,1);
  scene.last().translate(5,2,0);
  scene.last().set_pigment(std::make_shared<kwantrace::ConstantColor>(0,1,0));

  scene.addChild(std::make_shared<kwantrace::Sphere>());
  scene.last().translate(5,0,2);
  scene.last().set_pigment(std::make_shared<kwantrace::ConstantColor>(0,0,1));

  kwantrace::PerspectiveCamera<> cam=kwantrace::PerspectiveCamera<>(width,height);
  std::unique_ptr<uint8_t[]> pixbuf=cam.render(scene,width,height);

  FILE* oufb=fopen("image.pgm","ab");
  fwrite(pixbuf.get(),width*3,height,oufb);
  fclose(oufb);
}