#include <memory>
#include <fstream>
#include <cstdio>
#include "kwantrace.h"

int main() {
  const int width=1920;
  const int height=1080;
  std::ofstream ouf;
  ouf.open("image.pgm",std::ios::out|std::ios::trunc);
  ouf << "P5" << std::endl;
  ouf << width << " " << height << std::endl;
  ouf << 255 << std::endl;
  ouf.close();

  kwantrace::Union scene;
  scene.push_back(std::make_shared<kwantrace::Sphere>());
  scene.last().translate(5,0,0);
  scene.last().pigment=std::make_shared<kwantrace::ConstantColor>(1,0,0,0,0);
  scene.push_back(std::make_shared<kwantrace::Sphere>());
  scene.last().scale(0.5,0.5,1);
  scene.last().translate(5,2,0);
  scene.last().pigment=std::make_shared<kwantrace::ConstantColor>(0,1,0,0,0);
  scene.push_back(std::make_shared<kwantrace::Sphere>());
  scene.last().translate(5,0,2);
  scene.last().pigment=std::make_shared<kwantrace::ConstantColor>(0,0,1,0,0);
  kwantrace::PerspectiveCamera<uint8_t> cam=kwantrace::PerspectiveCamera<uint8_t>(width,height);
  std::unique_ptr<uint8_t[]> pixbuf=cam.render(scene,width,height);

  FILE* oufb=fopen("image.pgm","ab");
  fwrite(pixbuf.get(),width,height,oufb);
  fclose(oufb);
}