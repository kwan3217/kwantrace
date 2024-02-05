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
  auto plane=scene.add(std::make_shared<Plane>());
  plane->translate(0,0,-1);
  plane->setPigment(std::make_shared<kwantrace::ConstantColor>(1, 1, 0));

  auto buildgroup=[](double r, double g, double b) {
    auto group=std::make_shared<kwantrace::Union>();
    auto sphere1= group->add(std::make_shared<kwantrace::Sphere>());
    sphere1->scale(0.5);
    sphere1->translate(0,0.5,0);
    auto sphere2= group->add(std::make_shared<kwantrace::Sphere>());
    sphere2->scale(0.25);
    sphere2->translate(0,-0.25,0);
    auto sphere3= group->add(std::make_shared<kwantrace::Sphere>());
    sphere3->scale(0.25);
    sphere3->translate(0,0.5,0.5);
    group->setPigment(std::make_shared<kwantrace::ConstantColor>(r, g, b));
    return group;
  };
  auto groupX=scene.add(buildgroup(1,0,0));
  auto groupXRotate=groupX->rotateX(0);
  groupX->translate(-2,5,0);
  auto groupY=scene.add(buildgroup(0,1,0));
  auto groupYRotate=groupY->rotateY(90);
  groupY->translate(0,5,0);
  auto groupZ=scene.add(buildgroup(0,0,1));
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
