//
// Created by jeppesen on 2/4/21.
//

#ifndef KWANTRACE_PERSPECTIVECAMERA_H
#define KWANTRACE_PERSPECTIVECAMERA_H

namespace kwantrace {
  /**
   * Perspective (pinhole) camera
   *
   * @tparam pixdepth
   * @tparam pixtype
   */
  class PerspectiveCamera : public Camera {
  public:
    Direction direction;
    Direction right;
    Direction up;
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
    PerspectiveCamera() : direction(Position(1, 0, 0)), right(Position(0, 1, 0)),
                          up(Position(0, 0, 1)) {};

    PerspectiveCamera(double width, double height) : direction(Position(1, 0, 0)),
                                                     right(Position(0, width / height, 0)),
                                                     up(Position(0, 0, 1)) {};

    PerspectiveCamera(double width, double height, double angle) : direction(Position(angle2dir(angle, width / height), 0, 0)),
                                                                   right(Position(0, width / height, 0)),
                                                                   up(Position(0, 0, 1)) {};

    PerspectiveCamera(Position &Ldirection, Position &Lright, Position &Lup) : direction(
            Ldirection), right(Lright), up(Lup) {};
  protected:
    virtual Ray project_local(double x, double y) const override {
      Ray result;
      result.v = static_cast<Direction>(direction + right * x + up * y);
      return result;
    }
  };
}

#endif //KWANTRACE_PERSPECTIVECAMERA_H
