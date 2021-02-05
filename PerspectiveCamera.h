//
// Created by jeppesen on 2/4/21.
//

#ifndef KWANTRACE_PERSPECTIVECAMERA_H
#define KWANTRACE_PERSPECTIVECAMERA_H

#include "common.h"

namespace kwantrace {
  template<typename pixtype=uint8_t>
  class PerspectiveCamera : public Camera<pixtype> {
  public:
    DirectionVector direction;
    DirectionVector right;
    DirectionVector up;
    static double angle2dir(double angle, double rightlen) {
      //http://www.povray.org/documentation/3.7.0/r3_4.html#r3_4_2 from direction_length in default perspective camera
      return 0.5 * rightlen / tand(angle/2);
    }
    static double dir2angle(double dirlen, double rightlen) {
      //dirlen=0.5 * rightlen / tand(angle/2)
      //2*dirlen=rightlen / tand(angle/2)
      // tand(angle/2)=rightlen /(2*dirlen)
      //      angle/2 =atand(rightlen /(2*dirlen))
      //      angle   =2*atand(rightlen /(2*dirlen))
      return 2*atand(rightlen /(2*dirlen));
    }
    PerspectiveCamera() : direction(PositionVector(1, 0, 0)), right(PositionVector(0, 1, 0)),
                          up(PositionVector(0, 0, 1)) {};

    PerspectiveCamera(double width, double height) : direction(PositionVector(1, 0, 0)),
                                                     right(PositionVector(0, width / height, 0)),
                                                     up(PositionVector(0, 0, 1)) {};

    PerspectiveCamera(double width, double height, double angle) : direction(PositionVector(angle2dir(angle, width / height), 0, 0)),
                                                                   right(PositionVector(0, width / height, 0)),
                                                                   up(PositionVector(0, 0, 1)) {};

    PerspectiveCamera(PositionVector &Ldirection, PositionVector &Lright, PositionVector &Lup) : direction(
            Ldirection), right(Lright), up(Lup) {};
  protected:
    virtual Ray project_local(double x, double y) {
      Ray result;
      result.v = static_cast<DirectionVector>(direction + right * x + up * y);
      return result;
    }
  };
}

#endif //KWANTRACE_PERSPECTIVECAMERA_H
