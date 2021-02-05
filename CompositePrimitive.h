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
   * common ancestors, so at least there is no diamond problem. I wish that I knew how to declare
   * an add member function (or set of them) so that I could contain and delegate a member vector,
   * but I don't so I can't.
   */
  class CompositePrimitive : public Primitive, public std::vector<std::shared_ptr<Primitive>> {
  protected:
  public:
    auto lastIndex() {return size()-1;}
    Primitive& last() {return *(back());}
    virtual void prepareRender() {
      Primitive::prepareRender();
      for (auto&& primitive:*this) {
        primitive->prepareRender();
      }
    }
  };

}

#endif //KWANTRACE_COMPOSITEPRIMITIVE_H
