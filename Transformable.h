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

#ifndef KWANTRACE_TRANSFORMABLE_H
#define KWANTRACE_TRANSFORMABLE_H


namespace kwantrace {
  ///! List of pointers to transforms
  typedef std::vector<std::shared_ptr<Transformation>> TransformList;
  /** Entity that can be transformed. In general, we use the POV-Ray model where each transformation
   * is thought of as *physically moving* the Transformable. For instance, if we start with a Transformable
   * that is located at the origin and do a translate(1,2,3), the object will then be located at x=1, y=2, z=3.
   *
   * Also like POV-Ray, we treat all transformations as being about the origin, not about the center of the
   * given object which might not be at the origin any more. For instance:
   *
   *    * If an object is already 5 units from the origin and you call scale(3,3,3)
   *         * the object will then be 15 units from the origin.
   *    * If an object is at <5,0,0> and pointing down the x axis, and you call rotateZ(90)
   *         * it will be pointing down the Y axis, but also at <0,5,0>.
   *
   * With a little bit of thought, we can see what kind of *frame* transformation corresponds to a
   * *physical* move. Let's look at a translation by \f$\vec{r}\f$. In the body frame, we look at
   * the origin. In the world frame, that same point has coordinates of \f$\vec{r}\f$
   *
   * This is designed to be efficient, with as much effort done at scene construction and prepareRender() as
   * possible, to save as much time effort during the render. This makes sense, because the render will be
   * called literally millions of times. You may chain literally any number of transformations, and only pay
   * the cost at prepareRender(). During the render, the cost of 0, 1, or 1000 transformations are all the same.
   */
  class Transformable {
  private:
    /** Combine transformations in the transformation list. In terms of physical transformations, it is as if
     * the the transforms in the list are performed in order.
     *
     * \internal In actuality, the transforms are converted to
     * matrices, and then combined by matrix multiplication with the transformations in order from the right.
     * This is the traditional way to combine matrices, and is required if you are then going to use M*v
     * to transform a column vector.
     * @return Matrix representing the combination of all transformations performed in order.
     */
    Eigen::Matrix4d combine() const {
      Eigen::Matrix4d result{Eigen::Matrix4d::Identity()};
      for (auto&& trans:transformList) {
        result = trans->matrix() * result;
      }
      return result.matrix();
    }
    /** List of pointers to physical transformations to be performed, in order. The transformations themselves
     * can be changed through their pointer, but prepareRender must be called to actually apply the transformation
     */
    TransformList transformList;
  public:
    Eigen::Matrix4d Mwb; ///< World-from-body transformation matrix, only valid between a call to prepareRender and any changes to any transforms in the list
    Eigen::Matrix4d Mbw; ///< Body-from-world transformation matrix, only valid between a call to prepareRender and any changes to any transforms in the list
    Eigen::Matrix4d MwbN;///< World-from-body transformation matrix for surface normals, only valid between a call to prepareRender and any changes to any transforms in the list
    virtual ~Transformable()=default; ///< Allow there to be subclasses
    /** Prepare for rendering
     *
     * \internal This is done by calling combine() to combine all of the transformations, and
     *    then computing ancillary matrices Mwb, Mbw, and MwbN, which will also be needed.
     */
    virtual void prepareRender() {
      Mwb = combine();
      Mbw = Mwb.inverse();
      MwbN = Mbw.transpose();
    }

    /** Add a transformation to the list
     *
     * @param[in] transform A transformation
     * @return a pointer to this transformation. The transformation may be modified through this pointer, but prepareRender()
     *   must be called in order to make the changes active.
     */
    virtual void add(std::shared_ptr<Transformation> transform) {
      transformList.push_back(transform);
    }
    /**Create a POV-Ray like translation operation and add it to the list. This is in the physical sense -- an
     *  object which was at the origin will be at point after this operation
     *
     * @param[in] point Vector to move the object.
     * @return a pointer to this transformation. The transformation may be modified through this pointer, but prepareRender()
     *   must be called in order to make the changes active.
     */
    std::shared_ptr<Translation> translate(Position point) {
      auto result=std::make_shared<Translation>(point);
      add(result);
      return result;
    }

    /**Create a POV-Ray like translation operation and add it to the list. This is in the physical sense -- an
     *  object which was at the origin will be at point after this operation
     *
     * @param[in] x x-coordinate to move to
     * @param[in] y y-coordinate to move to
     * @param[in] z z-coordinate to move to
     * @return pointer to the transformation
     */
    std::shared_ptr<Translation> translate(double x, double y, double z) {
      return translate(Position(x, y, z));
    }

    /**Create a POV-Ray like rotation operation around the X axis and add it to the list.
     *
     * @param angle rotation in degrees
     * @return pointer to the transformation
     */
    std::shared_ptr<RotateX> rotateX(double angle) {
      auto result=std::make_shared<RotateX>(deg2rad(angle));
      add(result);
      return result;
    }

    /**Create a POV-Ray like rotation operation around the Y axis and add it to the list.
     *
     * @param angle rotation in degrees
     * @return pointer to the transformation
     */
    std::shared_ptr<RotateY> rotateY(double angle) {
      auto result=std::make_shared<RotateY>(deg2rad(angle));
      add(result);
      return result;
    }

    /**Create a POV-Ray like rotation operation around the Z axis and add it to the list.
     *
     * @param angle rotation in degrees
     * @return pointer to the transformation
     */
    std::shared_ptr<RotateZ> rotateZ(double angle) {
      std::shared_ptr<RotateZ> result=std::make_shared<RotateZ>(deg2rad(angle));
      add(result);
      return result;
    }


    /** Create a POV-Ray like scaling operation and add it to the list. This simultaneously
     * scales the object in the x, y, and z directions
     * @param x Scale factor along X axis
     * @param y Scale factor along Y axis
     * @param z Scale factor along Z axis
     * @return pointer to the transformation
     */
    std::shared_ptr<Scaling> scale(double x, double y, double z) {
      auto result=std::make_shared<Scaling>(x,y,z);
      add(result);
      return result;
    }

    /** Create a POV-Ray like scaling operation and add it to the list. This simultaneously
     * scales the object in the x, y, and z directions
     * @param s Uniform scale factor
     * @return pointer to the transformation
     */
    std::shared_ptr<UniformScaling> scale(double s) {
      auto result=std::make_shared<UniformScaling>(s);
      add(result);
      return result;
    }

    /** Create a POV-Ray like scaling operation and add it to the list. This simultaneously
     * scales the object in the x, y, and z directions
     * @param amount Scale factor
     * @return pointer to the transformation
     */
    std::shared_ptr<Scaling> scale(Eigen::Vector3d amount) {
      auto result=std::make_shared<Scaling>(amount);
      add(result);
      return result;
    }

    std::shared_ptr<LocationLookat> locationLookat(
      const Position &location,
      const Position &look_at,
      const Direction& p_b=Direction( 0, 0, 1),
      const Direction& t_b=Direction( 0, 1, 0),
      const Direction& t_r=Direction( 0, 0,-1)
    ) {
      auto result=std::make_shared<LocationLookat>(location,look_at,p_b,t_b,t_r);
      add(result);
      return result;
    }
  };
}

#endif //KWANTRACE_TRANSFORMABLE_H
