#pragma once

#include "Math/math.hpp"

class BaseSubscription;
class Mat4;
class Quat;
class Vec3;
class Window;

class Camera {
public:
  virtual ~Camera() {};

  virtual const Vec3& getPosition() const = 0;
  virtual const Vec3& getOrientation() const = 0;
  virtual const Vec3& getUp() const = 0;

  virtual real_t getFoV() const = 0;
  virtual real_t getAspectRatio() const = 0;

  virtual void attachWindow(Window *win) = 0;

  // If you must get precise values for whatever reason, eg. to account for
  // rounding errors
  virtual real_t getAspectNum() const = 0;
  virtual real_t getAspectDenom() const = 0;

  // Accessors

  const Mat4 projectionMatrix();
  const Mat4 modelViewMatrix();

  // Translation functions

  /**
   * Translates the camera by a single vector representing a displacement.
   */
  virtual void translateBy(const Vec3&) {};

  /**
   * Translates the camera in the direction of the given vector by the given
   * number of units.
   */
  virtual void translateBy(real_t, const Vec3&) {};

  /**
   * Translates the camera by the Mat4 representing a displacement. Assumes
   * there is no component which causes a rotation.
   */
  virtual void translateBy(const Mat4&) {};

  // Rotation functions

  /**
   * Rotates the camera by the given quaternion, assumed to represent a rotation
   * in R^3. The quaternion is not assumed to be unit.
   */
  virtual void rotateBy(const Quat&) {};

  /**
   * Rotates the camera around the given axis by the given angle.
   */
  virtual void rotateBy(real_t, const Vec3&) {};

  /**
   * Rotates the camera by the given Mat4. Assumes there is no displacement
   * component in the matrix.
   */
  virtual void rotateBy(const Mat4&) {};

  // Speed functions

  /**
   * Sets the horizontal rotation speed multiplier.
   */
  virtual void setHorizontalRotationSpeed(real_t) {};

  /**
   * Sets the vertical rotation speed multiplier.
   */
  virtual void setVerticalRotationSpeed(real_t) {};
};
