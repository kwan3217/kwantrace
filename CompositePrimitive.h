//
// Created by jeppesen on 2/4/21.
//

#ifndef KWANTRACE_COMPOSITEPRIMITIVE_H
#define KWANTRACE_COMPOSITEPRIMITIVE_H

#include <vector>
#include <memory>
#include "Primitive.h"

namespace kwantrace {
  /**
   * Boo! Multiple inheritance! Fortunately, kwantrace::Primitive and std::vector<> have no
   * common ancestors, so at least there is no diamond problem. I wish that
   */
  class CompositePrimitive : public Primitive, public std::vector<std::unique_ptr<Primitive>> {
  protected:
    /* TODO - this isn't going to work in a multi-threaded renderer */
    int lastHit = -1;
  public:
    auto lastIndex() {return size()-1;}
    Primitive& last() {return *((*this)[lastIndex()]);}
    virtual void prepareRender() {
      Primitive::prepareRender();
      for (auto&& primitive:*this) {
        primitive->prepareRender();
      }
    }
  };

}

#endif //KWANTRACE_COMPOSITEPRIMITIVE_H
