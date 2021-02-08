//
// Created by jeppesen on 2/7/21.
//

#ifndef KWANTRACE_COMPOSITE_H
#define KWANTRACE_COMPOSITE_H

namespace kwantrace {
  class Composite : public Renderable {
  protected:
    RenderableList children;
  public:
    virtual void prepareRender() override {
      Renderable::prepareRender();
      for (auto &&child:children) {
        child->setParent(this);
        child->prepareRender();
      }
    }

    std::shared_ptr<Renderable> addChild(std::shared_ptr<Renderable> child) {
      children.push_back(child);
      return child;
    }

    virtual std::shared_ptr<Eigen::Affine3d> addTransform(std::shared_ptr<Eigen::Affine3d> transform) override {
      std::shared_ptr<Eigen::Affine3d> result = addTransform(transform);
      for (auto &&child:children) {
        child->addTransform(transform);
      }
      return result;
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
