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

/** Example of implementing a primitive
 * Every primitive needs three functions to customize it:
 *   * A function to determine where a ray and the primitive meet
 *   * A function to determine the normal vector at a point.
 *   * A function to determine whether a point is inside the object.
 */
class Plane: public kwantrace::Primitive {
  /** Intersect a ray with the plane z=0.
   * \copydoc kwantrace::Primitive::intersectLocal()
   *
   * For a plane, our surface equation is super-simple, just:
   *
   *    * \f$f_x(x)=0\f$
   *    * \f$f_y(y)=0\f$
   *    * \f$f_z(z)=z\f$
   *
   * which gives us:
   *
   *    * \f$\begin{eqnarray*}
   *    f_x(x)&+&f_y(y)&+&f(z) & = & 0 \\
   *    0     &+& 0    &+&  z & = & 0  \\
   *          & &      & &  z & = & 0  \\
   *          & &      z_0&+&v_zt&=&0 \\
   *         & &      & & -z_0 &=&v_zt \\
   *           & &      & &  t &=&-\frac{z_0}{v_z}\end{eqnarray*}\f$
   */
  bool intersectLocal(const kwantrace::Ray &rayLocal, double &t) const override {
    if(rayLocal.v.z()==0) {
      t=0;
      return rayLocal.r0.z()==0;
    }
    t=-rayLocal.r0.z()/rayLocal.v.z();
    return t>0;
  }
  /**
   * \copydoc kwantrace::Primitive::normalLocal()
   *
   * Note that this function only needs to be valid at the surface.
   * Many primitives have a normal function which is correct at
   * the surface, but returns some value everywhere or almost
   * everywhere in space. This one for instance always returns
   * the \f$\hat{z}\f$ vector, since that is correct anywhere on the plane.
   */
  kwantrace::Direction normalLocal(const kwantrace::Position &rLocal) const override {
    return kwantrace::Direction(0,0,1);
  }
  /**   * \copydoc kwantrace::Primitive::insideLocal()
   * One naive way to
   * define "inside" is that if a point is exactly on the plane,
   * it is inside. However, it is much more useful to consider the plane
   * to divide space in half, and to consider one entire half of space
   * to be "inside". Since our plane is at \f$z=0\f$, we will treat any
   * point \f$z<0\f$ as inside the plane. Note that it doesn't really matter
   * if we use \f$z<0\f$ or \f$z\le 0\f$.
   *
   */
  bool insideLocal(const kwantrace::Position &rLocal) const override {
    return rLocal.z()<0;
  }
};

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

  /*
  auto sphere2=scene.addObject(std::make_shared<kwantrace::Sphere>());
  sphere2->scale(0.5,0.5,1);
  sphere2->translate(6,1.5,-0.5);
  sphere2->setPigment(std::make_shared<kwantrace::ConstantColor>(0, 1, 0));

  auto sphere3=scene.add(std::make_shared<kwantrace::Sphere>());
  sphere3->translate(5,0,2);
  sphere3->setPigment(std::make_shared<kwantrace::ConstantColor>(0, 0, 1));
  */
  auto plane=scene.add(std::make_shared<Plane>());
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