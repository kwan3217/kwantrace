//
// Created by jeppesen on 2/7/21.
//

#ifndef KWANTRACE_COMPOSITE_H
#define KWANTRACE_COMPOSITE_H

namespace kwantrace {
  class Composite : public Renderable {
  protected:
    RenderableList children; ///< List of child objects
  public:
    virtual void prepareRender() override {
      Renderable::prepareRender();
      for (auto &&child:children) {
        child->setParent(this);
        child->prepareRender();
      }
    }

    std::shared_ptr<Renderable> addObject(std::shared_ptr<Renderable> child) {
      children.push_back(child);
      return child;
    }

    virtual void addTransform(std::shared_ptr<Transformation> transform) override {
      Transformable::addTransform(transform);
      for (auto &&child:children) {
        child->addTransform(transform);
      }
    }
  };

  class Union : public Composite {
  public:
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
