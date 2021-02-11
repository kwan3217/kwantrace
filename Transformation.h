//
// Created by jeppesen on 2/10/21.
//

#ifndef KWANTRACE_TRANSFORMATION_H
#define KWANTRACE_TRANSFORMATION_H

#include <iostream>

namespace kwantrace {
/** Represent an arbitrary transformation. It can have any members it needs, but must be able
 * to take its members and generate a Matrix4d on demand. Members are intended to be changed (IE properties).
 *
 * I really didn't want to make this one -- I hoped to be able to use the Eigen transformations
 * directly, but things like translate, rotate, scale etc don't have a common base class, so they
 * can't be grouped in a common container.
 *
 * Besides, if I do it this way, I have a good spot for my thesis on the Point-Toward transformation.
 *
 * I have to call this "Transformation" instead of "Transformer" because otherwise I will be
 * thinking about robots in disguise...
 */
  class Transformation {
  public:
    virtual Eigen::Matrix4d matrix() const = 0;
  };

  class ScalarTransformation:public Transformation {
  private:
    double _amount;
  public:
    ScalarTransformation(double Lamount=0):_amount(Lamount) {};
    double get() const {return _amount;}
    void set(double Lamount) {_amount=Lamount;}
    double getd() const {return rad2deg(_amount);}
    void setd(double Lamount) {_amount=deg2rad(Lamount);}
  };

  class VectorTransformation:public Transformation {
  private:
    Eigen::Vector3d _amount;
  public:
    VectorTransformation(const Eigen::Vector3d Lamount=Eigen::Vector3d::Zero()):_amount(Lamount) {};
    VectorTransformation(double x, double y, double z):_amount(x,y,z) {};
    double getX() const {return _amount.x();}
      void setX(double Lx) {_amount.x()=Lx;}
    double getY() const {return _amount.y();}
      void setY(double Ly) {_amount.y()=Ly;}
    double getZ() const {return _amount.z();}
      void setZ(double Lz) {_amount.z()=Lz;}
    Eigen::Vector3d getV() const {return _amount;}
               void setV(const Eigen::Vector3d Lamount) {_amount=Lamount;}
  };

  class Translation:public VectorTransformation {
  public:
    using VectorTransformation::VectorTransformation;
    virtual Eigen::Matrix4d matrix() const override {
      Eigen::Matrix4d result=Eigen::Matrix4d::Identity();
      result(0,3)=getX();
      result(1,3)=getY();
      result(2,3)=getZ();
      return result;
    }
  };

  class Scaling:public VectorTransformation {
  public:
    using VectorTransformation::VectorTransformation;
    virtual Eigen::Matrix4d matrix() const override {
      Eigen::Matrix4d result=Eigen::Matrix4d::Identity();
      result(0,0)=getX()==0?1:getX();
      result(1,1)=getY()==0?1:getY();
      result(2,2)=getZ()==0?1:getZ();
      return result;
    }
  };

  class UniformScaling:public ScalarTransformation {
  public:
    using ScalarTransformation::ScalarTransformation;
    virtual Eigen::Matrix4d matrix() const override {
      Eigen::Matrix4d result=Eigen::Matrix4d::Identity();
      result(0,0)=get()==0?1:get();
      result(1,1)=get()==0?1:get();
      result(2,2)=get()==0?1:get();
      return result;
    }
  };

  template<int i1, int i2>
  class RotateScalar:public ScalarTransformation {
  public:
    using ScalarTransformation::ScalarTransformation;
    /** \include Transformation::matrix()
     *
     */
    virtual Eigen::Matrix4d matrix() const override {
      Eigen::Matrix4d result=Eigen::Matrix4d::Identity();
      double c = cos(get());
      double s = sin(get());
      result(i1, i1)= c; result(i1, i2)= -s;
      result(i2, i1)= s; result(i2, i2)=  c;
      return result;
    }
  };
  typedef RotateScalar<1,2> RotateX;
  typedef RotateScalar<2,0> RotateY;
  typedef RotateScalar<0,1> RotateZ;

  /** Represent the Point-Toward transformation. This rotates an object such that
   * p_b in the body frame points at p_r in the world frame, and t_b in the body frame is towards
   * t_r in the world frame.
   *
   * ## Problem Statement
   * \f$
   *    \def\M#1{{[\mathbf{#1}]}}
   *    \def\MM#1#2{{[\mathbf{#1}{#2}]}}
   *    \def\T{^\mathsf{T}}
   *    \def\operatorname#1{{\mbox{#1}}}
   * \f$
   * Given a rigid body with vectors from its origin to direction (normalized vector)
   * \f$\hat{p}_b\f$ and \f$\hat{t}_b\f$ in the body frame, and an external frame
   * centered on the same origin with directions \f$\hat{p}_r\f$ and \f$\hat{t}_r\f$,
   * find the physical rotation that points \f$\hat{p}_b\f$ and \f$\hat{p}_r\f$
   * at the same direction, while simultaneously pointing \f$\hat{t}_b\f$ as close as
   * possible to \f$\hat{t}_r\f$.
   *
   * ### Example
   * The Space Shuttle has a thrust vector which is not parallel to any of the body axes.
   * We wish to point the thrust vector in the correct direction in the reference system,
   * while simultaneously flying heads-down, which is equivalent to pointing the tail
   * towards the ground. In this case, \f$\hat{p}_b\f$ is the thrust vector in the body
   * frame, \f$\hat{p}_r\f$ is the guidance-calculated thrust vector in the reference
   * frame, \f$\hat{t}_b\f$ is the body axis which points heads-up, say \f$\hat{z}_b\f$,
   * and \f$\hat{t}_r\f$ is the vector from the spacecraft location towards the center
   * of the Earth.
   *
   * \image html 320px-Point_constraint.svg.png
   *
   * \image html 320px-Toward_constraint.svg.png
   *
   * ## Solution
   * We are going to do this with matrices. The solution matrix is going to be called
   * \f$\MM{M}{_{b2r}}\f$ and will transform *from* the body frame *to* the reference frame.
   *
   * First, it is obviously impossible to in general satisfy both the "point" constraint
   * \f$\hat{p}_r=\MM{M}{_{b2r}}\hat{p}_b\f$ and the toward constraint \f$\hat{t}_r=\MM{M}{_{b2r}}\hat{t}_b\f$.
   * Satisfying both is possible if and only if the angle between \f$\hat{p}_r\f$ and \f$\hat{t}_r\f$
   * is the same as the angle between \f$\hat{p}_b\f$ and \f$\hat{t}_b\f$. When these
   * angles do not match, the point constraint will be perfectly satisfied, and the
   * angle between the body and reference toward vectors will be as small as possible.
   * Using geometric intuition, it is obvious but not proven here that the angle is
   * minimum when the point vector, transformed body toward vector, and reference toward
   * vector are all in the same plane. This means that we can create a third vector
   * \f$\hat{s}=\operatorname{normalize}(\hat{p} \times \hat{t})\f$. This vector is
   * normal to the plane containing point and toward in both frames, so when the plane
   * is the same, these vectors match. Therefore we have another constraint which can
   * be perfectly satisfied, \f$\hat{s}_r=\MM{M}{_{b2r}}\hat{s}_b\f$.
   * So, we have:
   *
   * \f$\begin{bmatrix}\hat{p}_r && \hat{s}_r\end{bmatrix}=\MM{M}{_{b2r}}\begin{bmatrix}\hat{p}_b && \hat{s}_b\end{bmatrix}\f$
   *
   * This isn't quite enough data, it works out to nine unknowns and six equations.
   * We can add one more constraint by considering the vector \f$\hat{u}\f$ perpendicular
   * to both \f$\hat{p}\f$ and \f$\hat{s}\f$:
   *
   * \f$\hat{u}=\hat{p} \times \hat{s}\f$
   *
   * We already know that this will be unit length since \f$\hat{p}\f$ and \f$\hat{s}\f$
   * are already perpendicular. Since these three vectors are perpendicular in both frames,
   * only an orthogonal matrix can transform all three vectors and maintain the angles
   * between them, so this third vector is equivalent to adding an orthogonality constraint.
   *
   * \f$\M{R}=\begin{bmatrix}\hat{p}_r && \hat{s}_r\ && \hat{u}_r \end{bmatrix}\f$
   *
   * (treating the vectors as column vectors)
   *
   * \f$\begin{eqnarray*}
   *    \M{B}&=&\begin{bmatrix}\hat{p}_b && \hat{s}_b\ && \hat{u}_b \end{bmatrix} \\
   *    \M{R}&=&\MM{M}{_{b2r}}\M{B} \\
   *    \M{R}\M{B}^{-1}&=&\MM{M}{_{b2r}}\end{eqnarray*}\f$
   *
   *The above calls for a matrix inverse, but who has time for that? Since all the columns of
   * \f$\M{B}\f$ (and \f$\M{R}\f$ for that matter) are unit length and perpendicular to each
   * other, the matrix is orthogonal, which means that its inverse is its transpose.
   *
   * \f$\M{R}\M{B}^T=\MM{M}{_{b2r}}\f$
   *
   * And that's the solution. Note that if you need \f$\MM{M}{_{r2b}}\f$, it is also a transpose
   * since this answer is still an orthonormal (IE rotation) matrix.
   *
   * @param p_b
   * @param p_r point vector in world frame
   * @param t_b toward vector in body frame
   * @param t_r toward vector in world frame
   * @return Rotation matrix which physically rotates the object such that it points toward
   * the indicated vectors. This is \f$\MM{M}{_{b2r}}\f$ above.
   *
   * None of the inputs require normalization. This is all done internally.
   */
  class PointToward:public Transformation {
  private:
    Eigen::Vector3d p_b, p_r, t_b, t_r;
  public:
    PointToward(
            const Eigen::Vector3d &Lp_b, ///< point vector in body frame
            const Eigen::Vector3d &Lp_r, ///< point vector in world frame
            const Eigen::Vector3d &Lt_b, ///< toward vector in body frame
            const Eigen::Vector3d &Lt_r  ///< toward vector in world frame
    ) : p_b(Lp_b), p_r(Lp_r), t_b(Lt_b), t_r(Lt_r) {}
    Eigen::Vector3d getPb() const                     {return p_b;}
               void setPb(const Eigen::Vector3d& Lpb) {p_b=Lpb;}
    Eigen::Vector3d getPr() const                     {return p_r;}
               void setPr(const Eigen::Vector3d& Lpr) {p_b=Lpr;}
    Eigen::Vector3d getTb() const                     {return t_b;}
               void setTb(const Eigen::Vector3d& Ltb) {p_b=Ltb;}
    Eigen::Vector3d getTr() const                     {return t_r;}
               void setTr(const Eigen::Vector3d& Ltr) {p_b=Ltr;}

    virtual Eigen::Matrix4d matrix() const override {
      Eigen::Matrix3d R, B;
      Direction s_r = (p_r.cross(t_r)).normalized();
      Direction u_r = (p_r.cross(s_r)).normalized();
      R << p_r.normalized(), s_r, u_r;
      Direction s_b = (p_b.cross(t_b)).normalized();
      Direction u_b = (p_b.cross(s_b)).normalized();
      B << p_b.normalized(), s_b, u_b;
      Eigen::Matrix4d M_b2r = Eigen::Matrix4d::Identity();
      M_b2r.block<3, 3>(0, 0) = R * B.transpose();
      return M_b2r;
    }
  };

  /** Exercise pointToward().
   * \image html Space_Shuttle_Coordinate_System.jpg
   *
   * The space shuttle has a thrust axis 13&deg; below the X axis, so:
   * \f$\hat{p}_b=\begin{bmatrix}\cos 13^\circ \\ 0 \\ -\sin 13^\circ \end{bmatrix}
   *   =\begin{bmatrix}0.974370 \\ 0.000000 \\ -0.224951 \end{bmatrix}\f$
   *
   * The heads-up vector is \f$\hat{t}_b=\hat{z}_b\f$. At a particular instant,
   * the guidance command says to point the thrust vector 30&deg; above the horizon
   * at an azimuth of 80&deg; east of North. We'll take the local topocentric horizon
   * frame as the reference frame, with \f$\hat{x}_r\f$ in the horizon plane pointing
   * east, \f$\hat{y}_r\f$ pointing north, and \f$\hat{z}_r\f$ pointing up. In this
   * frame, the guidance command is:
   *
   * \f$\hat{p}_r=\begin{bmatrix}\cos 30^\circ \sin 80^\circ \\
   *                             \cos 30^\circ \cos 80^\circ \\
   *                             \sin 30^\circ\end{bmatrix}=\begin{bmatrix}0.852869 \\
   *                                                                       0.150384 \\
   *                                                                       0.500000\end{bmatrix}\f$
   *
   * The vehicle is also commanded to the heads-down attitude, which means that
   * \f$\hat{t}_r=-\hat{z}_r\f$. These are all the inputs we need.
   *
   * \f$\hat{s}_b=\operatorname{normalize}(\hat{p}_b \times \hat{t}_b)=\begin{bmatrix} 0 \\
   *                                                                                  -1 \\
   *                                                                                   0 \end{bmatrix}\f$
   *
   * \f$\hat{u}_b=\operatorname{normalize}(\hat{p}_b \times \hat{s}_b)=\begin{bmatrix} -0.224951 \\
   *                                                                                    0.000000 \\
   *                                                                                   -0.974370 \end{bmatrix}\f$
   *\f$\hat{s}_r=\operatorname{normalize}(\hat{p}_r \times \hat{t}_r)=\begin{bmatrix} -0.173648 \\
   *                                                                                   0.984808 \\
   *                                                                                   0.000000 \end{bmatrix}f$
   *
   *\f$\hat{u}_r=\operatorname{normalize}(\hat{p}_r \times \hat{s}_r)=\begin{bmatrix} -0.492404 \\
   *                                                                                  -0.086824 \\
   *                                                                                  -0.866025 \end{bmatrix}\f$
   *
   * \f$\M{R}=\begin{bmatrix}\hat{p}_r && \hat{s}_r\ && \hat{u}_r \end{bmatrix}=\begin{bmatrix}0.852869&&-0.173648&&-0.492404\\
   *                                                                                   0.150384&& 0.984808&&-0.086824\\
   *                                                                                   0.500000&& 0.000000&& 0.866025\end{bmatrix}\f$
   *
   * \f$\M{B}=\begin{bmatrix}\hat{p}_b && \hat{s}_b\ && \hat{u}_b \end{bmatrix}=\begin{bmatrix}0.974370&& 0.000000&&-0.224951\\
   *                                                                                           0.000000&&-1.000000&&-0.000000\\
   *                                                                                          -0.224951&& 0.000000&&-0.974370\end{bmatrix}\f$
   *\f$\M{M_{br}}=\M{R}\M{B}^{-1}=\begin{bmatrix}0.941776&& 0.173648&& 0.287930\\
   *                                             0.166061&&-0.984808&& 0.050770\\
   *                                             0.292372&& 0.000000&&-0.956305\end{bmatrix}\f$
   *
   * There is the solution, but does it work?
   *
   * \f$\begin{eqnarray*}\M{M_{br}}\hat{p}_b&=&\begin{bmatrix} 0.852869\\ 0.150384\\ 0.500000\end{bmatrix}&=&\hat{p}_r \\
   *                     \M{M_{br}}\hat{s}_b&=&\begin{bmatrix}-0.173648\\ 0.984808\\ 0.000000\end{bmatrix}&=&\hat{s}_r \\
   *                     \M{M_{br}}\hat{u}_b&=&\begin{bmatrix}-0.492404\\-0.086824\\ 0.866025\end{bmatrix}&=&\hat{u}_r \\
   *                     \M{M_{br}}\hat{t}_b&=&\begin{bmatrix} 0.287930\\ 0.050770\\-0.956305\end{bmatrix}, \operatorname{vangle}(\M{M_{br}}\hat{t}_b,\hat{t}_r)=17^\circ\end{eqnarray*}\f$
   *
   * That's a decisive yes.
   */
  static void exercise_pointToward() {
    Direction p_b(cosd(13),
                  0,
                  -sind(13));
    std::cout << "p_b:"<< std::endl << p_b << std::endl;
    Direction t_b(0,0,1);
    std::cout << "t_b:"<< std::endl << t_b << std::endl;
    Direction p_r(cosd(30)*sind(80),
                  cosd(30)*cosd(80),
                  sind(30)         );
    std::cout << "p_r:"<< std::endl << p_r << std::endl;
    Direction t_r(0,0,-1);
    std::cout << "t_r:"<< std::endl << t_r << std::endl;
    Direction s_b=p_b.cross(t_b).normalized();
    std::cout << "s_b:"<< std::endl << s_b << std::endl;
    Direction u_b=p_b.cross(s_b).normalized();
    std::cout << "u_b:"<< std::endl << u_b << std::endl;
    Direction s_r=p_r.cross(t_r).normalized();
    std::cout << "s_r:"<< std::endl << s_r << std::endl;
    Direction u_r=p_r.cross(s_r).normalized();
    std::cout << "u_r:"<< std::endl << u_r << std::endl;
    Eigen::Matrix3d R;
    R << p_r,s_r,u_r;
    std::cout << "R:  "<< std::endl << R << std::endl;
    Eigen::Matrix3d B;
    B << p_b,s_b,u_b;
    std::cout << "B:  "<< std::endl << B << std::endl;
    Eigen::Matrix3d M_b2r_direct=R*B.transpose();
    std::cout << "M_b2r (direct):  "<< std::endl << M_b2r_direct << std::endl;
    auto M_b2r=PointToward(p_b,p_r,t_b,t_r).matrix();
    std::cout << "M_b2r:  "<< std::endl << M_b2r << std::endl;
    std::cout << "M_b2r*p_b (should equal p_r):  "<< std::endl << M_b2r*p_b << std::endl;
    std::cout << "M_b2r*s_b (should equal s_r):  "<< std::endl << M_b2r*s_b << std::endl;
    std::cout << "M_b2r*u_b (should equal u_r):  "<< std::endl << M_b2r*u_b << std::endl;
    std::cout << "M_b2r*t_b (should be towards t_r):  "<< std::endl << M_b2r*t_b << std::endl;
  }
  /** Creates a matrix which rotates an object at location around location to point its x axis at look_at, and
   * its z axis as close as possible to sky. This rotation is unique
   *
   * @param location Location to rotate around
   * @param look_at Point X axis at
   * @param sky
   * @return Transformation matrix which does the job
   */
  Eigen::Matrix4d calcLocationLookat(const Position &location, const Position &look_at,
                                            const Direction &sky = Direction(0, 0, 1)) {
    Eigen::Matrix4d result;
    result=Translation(location).matrix()*PointToward(Direction(1,0,0),Direction(look_at-location),Direction(0,0,1),sky).matrix()*Translation(-location).matrix();
    return result;
  }

}

#endif //KWANTRACE_TRANSFORMATION_H
