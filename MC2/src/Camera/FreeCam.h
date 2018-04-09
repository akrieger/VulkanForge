#pragma once

#include "Camera/Camera.h"
#include "Events/Types/EventData.h"
#include "Events/PubSub.hpp"
#include "Math/math.hpp"
#include "Math/vector.hpp"

class Quat;
class Mat4;
class Window;

/**
 * A camera without a fixed up vector. Like being in space.
 */
class FreeCam : public Camera {
public:
  FreeCam();
  FreeCam(Window *win);
  virtual ~FreeCam();

  const Vec3& getPosition() const;
  const Vec3& getOrientation() const;
  const Vec3& getUp() const;

  real_t getFoV() const;
  real_t getAspectRatio() const;

  void attachWindow(Window *win);

  real_t getAspectNum() const;
  real_t getAspectDenom() const;

  void translateBy(const Vec3& delta);
  void translateBy(real_t magnitude, const Vec3& axis);
  void translateBy(const Mat4& translation_mat);

  void rotateBy(const Quat& rotation_quat);
  void rotateBy(real_t radians, const Vec3& axis);
  void rotateBy(const Mat4& rotation_mat);

  void translateAndRotateBy(const Mat4& transform_mat);

  void setHorizontalRotationSpeed(real_t h_turn_speed);
  void setVerticalRotationSpeed(real_t v_turn_speed);

  void setTranslationSpeed(real_t h_speed);
  void setVerticalTranslationSpeed(real_t v_speed);

private:
  typedef enum {
    FORWARD = 1 << 0,
    BACKWARD = 1 << 1,
    LEFT = 1 << 2,
    RIGHT = 1 << 3,
    UP = 1 << 4,
    DOWN = 1 << 5,
  } Direction;

  Vec3 position;
  Vec3 orientation;
  Vec3 implicit_up;

  Window* win;

  real_t fov;
  real_t aspect_num;
  real_t aspect_denom;
  real_t aspect;

  real_t h_turn_speed;
  real_t v_turn_speed;
  real_t h_speed;
  real_t v_speed;
  
  real_t h_turn_modifier = 1.0;
  real_t v_turn_modifier = 1.0;
  real_t h_speed_modifier = 1.0;
  real_t v_speed_modifier = 1.0;
  
  bool rotating;
  char motion;

  subscription_t mouse_move_sub;
  subscription_t mouse_click_sub;
  subscription_t key_sub;
  subscription_t timer_sub;

  void mouseMoved(EventData *d);
  void mouseClicked(EventData *d);
  void keyPressed(EventData *d);
  void onTick(EventData *d);
};
