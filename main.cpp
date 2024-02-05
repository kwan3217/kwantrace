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
#include <fstream>
#include "kwantrace.h"
#include "Composite.h"

int main() {
  kwantrace::PointToward::exercisePointToward();
  const int width=1920;
  const int height=1080;

  kwantrace::Scene<> scene;
  auto camera=scene.set(std::make_shared<kwantrace::PerspectiveCamera>(width,height));
  camera->locationLookat(kwantrace::Position(-5,5,2),kwantrace::Position(5,0,2));
  //camera->translate(0,0,2);
  //camera->rotateX(-90);

  auto shader=scene.set(std::make_shared<kwantrace::POVRayShader>());
  //auto sphere1=scene.add(std::make_shared<kwantrace::Sphere>());
  //sphere1->translate(5,0,0);
  //sphere1->setPigment(std::make_shared<kwantrace::ConstantColor>(1, 0, 0));

  auto plane=scene.add(std::make_shared<kwantrace::Plane>());
  plane->translate(0,0,-1);
  plane->setPigment(std::make_shared<kwantrace::ConstantColor>(1, 1, 0));


  auto groupX=std::make_shared<kwantrace::Union>();
  auto groupY=std::make_shared<kwantrace::Union>();
  auto groupZ=std::make_shared<kwantrace::Union>();
  scene.add(groupX);
  auto sphereX1= groupX->add(std::make_shared<kwantrace::Sphere>());
  sphereX1->scale(0.5);
  sphereX1->translate(0,0.5,0);
  auto sphereX2= groupX->add(std::make_shared<kwantrace::Sphere>());
  sphereX2->scale(0.25);
  sphereX2->translate(0,-0.25,0);
  auto sphereX3= groupX->add(std::make_shared<kwantrace::Sphere>());
  sphereX3->scale(0.25);
  sphereX3->translate(0,0.5,0.5);
  groupX->setPigment(std::make_shared<kwantrace::ConstantColor>(1, 0, 0));
  auto groupXRotate=groupX->rotateX(0);
  groupX->translate(-2,5,0);

  scene.add(groupY);
  auto sphereY1= groupY->add(std::make_shared<kwantrace::Sphere>());
  sphereY1->scale(0.5);
  sphereY1->translate(0,0.5,0);
  auto sphereY2= groupY->add(std::make_shared<kwantrace::Sphere>());
  sphereY2->scale(0.25);
  sphereY2->translate(0,-0.25,0);
  auto sphereY3= groupY->add(std::make_shared<kwantrace::Sphere>());
  sphereY3->scale(0.25);
  sphereY3->translate(0,0.5,0.5);
  groupY->setPigment(std::make_shared<kwantrace::ConstantColor>(0, 1, 0));
  auto groupYRotate=groupY->rotateY(90);
  groupY->translate(0,5,0);

  scene.add(groupZ);
  auto sphereZ1= groupZ->add(std::make_shared<kwantrace::Sphere>());
  sphereZ1->scale(0.5);
  sphereZ1->translate(0,0.5,0);
  auto sphereZ2= groupZ->add(std::make_shared<kwantrace::Sphere>());
  sphereZ2->scale(0.25);
  sphereZ2->translate(0,-0.25,0);
  auto sphereZ3= groupZ->add(std::make_shared<kwantrace::Sphere>());
  sphereZ3->scale(0.25);
  sphereZ3->translate(0,0.5,0.5);
  groupZ->setPigment(std::make_shared<kwantrace::ConstantColor>(0, 0, 1));
  auto groupZRotate=groupZ->rotateZ(90);
  groupZ->translate(2,5,0);

  kwantrace::ObjectColor white;
  white<<1,1,1,0,0;
  auto light1=scene.add(std::make_shared<kwantrace::Light>(kwantrace::Position(-20,-20,20),white));

  for(int i=0;i<100;i++) {
    groupXRotate->setd(i*3.6);
    groupYRotate->setd(i*3.6);
    groupZRotate->setd(i*3.6);
    auto pixbuf = scene.render(width, height);
    std::ofstream ouf;
    char oufn[20];
    sprintf(oufn,"Frames/image%02d.ppm",i);
    ouf.open(oufn, std::ios::out | std::ios::trunc);
    ouf << "P6" << std::endl;
    ouf << width << " " << height << std::endl;
    ouf << 255 << std::endl;
    ouf.close();
    FILE *oufb = fopen(oufn, "ab");
    fwrite(pixbuf.get(), width * 3, height, oufb);
    fclose(oufb);
    printf("Finished frame %d of 100\n",i);
  }
}
