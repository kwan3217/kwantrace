//
// Created by jeppesen on 2/7/21.
//

#ifndef KWANTRACE_COMPOSITE_H
#define KWANTRACE_COMPOSITE_H

namespace kwantrace {
  /** Represents some kind of composition of Renderable objects.
   * Note that this by itself does not represent anything visible! Only
   * its children are visible.
   *
   * This class descends from, and therefore is itself, Transformable.
   * In order to support this, any transformation of this object is
   * passed down to its children.
   */
  class Composite : public Renderable {
  protected:
    RenderableList children; ///< List of child objects
  public:
    /** \copydoc Renderable::prepareRender()
     *
     * This method, in addition to calling the superclass, also
     * sets the parent of each child to this, and calls the
     * prepareRender() method of the child.
     */
    virtual void prepareRender() override {
      Renderable::prepareRender();
      for (auto &&child:children) {
        child->setParent(this);
        child->prepareRender();
      }
    }

    /**Add a child to this composite. The input is passed right back out
     * so that you can construct a child, add it to its parent, and get
     * a handle to it, all in one line:
     *
     *      auto child=parent.add(std::make_shared<ChildType>(..child constructor arguments..));
     *
     * @param child A pointer to a child object
     * @return The pointer is returned as-is.
     */
    std::shared_ptr<Renderable> add(std::shared_ptr<Renderable> child) {
      children.push_back(child);
      return child;
    }

    /** \copydoc Renderable::add()
     *
     * This calls the superclass add(), then adds the transform
     * to each child.
     *
     * @param transform Transformation to be added
     */
    virtual void add(std::shared_ptr<Transformation> transform) override {
      Transformable::add(transform);
      for (auto &&child:children) {
        child->add(transform);
      }
    }
  };

  /** Represents a Constructive Solid Geometry (CSG) union. As is implied by union,
   * a point is inside a union if it is inside *any* of its children. See Intersection
   * for an object where you have to be inside *all* of the children.
   */
  class Union : public Composite {
  public:
    /** \copydoc Renderable::intersect()
     *
     * Since this is a union, the intersection is the child that
     * has the smallest possible t parameter, with no constraint
     * as to whether that point is inside or outside of the other
     * children.
     */
    virtual Observer<Primitive> intersect(const Ray &ray, double &t) const override {
      const Primitive *result=nullptr;
      t = std::numeric_limits<double>::infinity();
      for (auto &&child:children) {
        double this_t;
        const Primitive *this_result = child->intersect(ray, this_t);
        if (this_result && this_t < t) {
          t = this_t;
          result = this_result;
        }
      }
      return result;
    };

    virtual bool inside(const Position &r) const override {
      bool result = false;
      for (auto &&child:children) {
        result |= child->inside(r);
      }
      return result;
    };
  };

  /** Represents a Constructive Solid Geometry (CSG) union. As is implied by union,
   * a point is inside a union if it is inside *any* of its children. See Intersection
   * for an object where you have to be inside *all* of the children.
   */
  class Intersection : public Composite {
  public:
    /** \copydoc Renderable::intersect()
     *
     * In an intersection, the valid ray intersect is the smallest one that is inside of every *other* child, since
     * of course the intersect will be on the surface of one of the children.
     */
    virtual Observer<Primitive> intersect(const Ray &ray, double &t) const override {
      const Primitive *result=nullptr;
      t = std::numeric_limits<double>::infinity();
      for (auto &&child:children) {
        double this_t;
        const Primitive *this_result = child->intersect(ray, this_t);
        if (this_result && this_t < t) {
          t = this_t;
          result = this_result;
        }
      }
      return result;
    };

    virtual bool inside(const Position &r) const override {
      bool result = false;
      for (auto &&child:children) {
        result |= child->inside(r);
      }
      return result;
    };
  };

}

#endif //KWANTRACE_COMPOSITE_H
