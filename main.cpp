#include <memory>
#include <fstream>
#include <cstdio>
#include "kwantrace.h"
#include "Composite.h"

int main() {
  const int width=1920;
  const int height=1080;
  std::ofstream ouf;
  ouf.open("image.pgm",std::ios::out|std::ios::trunc);
  ouf << "P6" << std::endl;
  ouf << width << " " << height << std::endl;
  ouf << 255 << std::endl;
  ouf.close();

  kwantrace::Scene<> scene;
  scene.set(std::make_shared<kwantrace::PerspectiveCamera>(width,height));
  scene.set(std::make_shared<kwantrace::POVRayShader>());
  auto sphere1=scene.addObject(std::make_shared<kwantrace::Sphere>());
  sphere1->translate(5,0,0);
  sphere1->setPigment(std::make_shared<kwantrace::ConstantColor>(1, 0, 0));

  auto sphere2=scene.addObject(std::make_shared<kwantrace::Sphere>());
  sphere2->scale(0.5,0.5,1);
  sphere2->translate(6,1.5,-0.5);
  sphere2->setPigment(std::make_shared<kwantrace::ConstantColor>(0, 1, 0));

  auto sphere3=scene.addObject(std::make_shared<kwantrace::Sphere>());
  sphere3->translate(5,0,2);
  sphere3->setPigment(std::make_shared<kwantrace::ConstantColor>(0, 0, 1));
  kwantrace::ObjectColor white;
  white<<1,1,1,0,0;
  auto light1=scene.addLight(std::make_shared<kwantrace::Light>(kwantrace::Position(-20,-20,20),white));

  auto pixbuf=scene.render(width,height);


  FILE* oufb=fopen("image.pgm","ab");
  fwrite(pixbuf.get(),width*3,height,oufb);
  fclose(oufb);
}