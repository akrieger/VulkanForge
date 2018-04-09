#include "Events/PubSub.hpp"
#include "Events/Subscriptions.hpp"
#include "Events/EventEmitter.hpp"

#include "Math/math.hpp"
#include "Math/matrix.hpp"
#include "Math/quaternion.hpp"

#include "Window/Window.h"

#include "Camera/FreeCam.h"

static real_t CROUCH_MOVE_MODIFIER = 0.1;

FreeCam::FreeCam() : orientation(Vec3(1,0,0)), position(Vec3(0,0,0)),
    implicit_up(Vec3(0,1,0)), fov(PI2), aspect(1.0), aspect_num(1.0),
    aspect_denom(1.0), h_turn_speed(2.0), v_turn_speed(2.0), h_speed(2.0),
    v_speed(2.0), motion(0) {

}

FreeCam::FreeCam(Window *win) : orientation(Vec3(1,0,0)), position(Vec3(0,0,0)),
    implicit_up(Vec3(0,1,0)), fov(PI2), aspect(1.0), aspect_num(1.0),
    aspect_denom(1.0), h_turn_speed(2.0), v_turn_speed(2.0), h_speed(2.0),
    v_speed(2.0), win(win), motion(0) {

  if (win) {
    attachWindow(win);
  }
}

FreeCam::~FreeCam() {
  if (win) {
    // Assume valid window == valid subscription iterators.
    PubSub::get().unsub(mouse_move_sub);
    PubSub::get().unsub(mouse_click_sub);
    PubSub::get().unsub(key_sub);
    PubSub::get().unsub(timer_sub);
    win = NULL;
  }
}

void FreeCam::attachWindow(Window *window) {
  win = window;

  EventEmitter<MouseMoveData>::sub([&](const MouseMoveData& mmd) -> bool {
    return false;
  });
  
  mouse_move_sub = PubSub::get().sub(MouseMove, win, [&](EventData *d) {
    this->mouseMoved(d);
  });

  mouse_click_sub = PubSub::get().sub(MouseClick, win, [&](EventData *d) {
    this->mouseClicked(d);
  });

  key_sub = PubSub::get().sub(KeyPress, win, [&](EventData *d) {
    this->keyPressed(d);
  });

  timer_sub = PubSub::get().sub(Timer, [&](EventData *d){
    this->onTick(d);
  });
}

void FreeCam::rotateBy(const Quat& rotation) {
  Quat temp(rotation);
  temp.normalize();
  orientation = temp * orientation;
  orientation.normalize();
};

void FreeCam::rotateBy(real_t radians, const Vec3& axis) {
  rotateBy(Quat(radians, axis));
};

void FreeCam::translateBy(const Vec3& delta) {
  position += delta;
}

void FreeCam::translateBy(real_t distance, const Vec3& direction) {
  position += (direction * distance);
}

void FreeCam::translateBy(const Mat4& translation_matrix) {
  position = translation_matrix.transform(position);
}

void FreeCam::mouseClicked(EventData *d) {
  MouseClickData mcd = d->MouseClickData;
  if (mcd.button == MouseButtonLeft) {
    if (mcd.action == KeyDown) {
      rotating = true;
    } else if (mcd.action == KeyUp) {
      rotating = false;
    }
  }
}

void FreeCam::mouseMoved(EventData *d) {
  if (!rotating) {
    return;
  }
  MouseMoveData mmd = d->MouseMoveData;
  // x increase -> right
  // x decrease -> left
  double theta_horizontal = -1 * (mmd.dx / win->getWidth()) * fov * h_turn_speed;

  // y increase -> down
  // y decrease -> up 
  double theta_vertical = (mmd.dy / win->getHeight()) * fov * v_turn_speed / aspect;

  //quaternions = *right* handed system - vector up, positive rotation -> left, negative -> right ?
  rotateBy(Quat(theta_horizontal, implicit_up));
  rotateBy(Quat(theta_vertical, implicit_up.cross(orientation)));

  implicit_up = orientation.cross(implicit_up.cross(orientation));
  implicit_up.normalize();
}

void FreeCam::keyPressed(EventData *d) {
  KeyPressData kpd = d->KeyPressData;
  if (kpd.modifiers & GLFW_MOD_SHIFT) {
    h_speed_modifier = CROUCH_MOVE_MODIFIER;
    v_speed_modifier = CROUCH_MOVE_MODIFIER;
  } else {
    h_speed_modifier = 1.0;
    v_speed_modifier = 1.0;
  }
  char direction = 0;
  switch (kpd.key) {
    case KeyW:
      direction = FORWARD;
      break;
    case KeyA:
      direction = LEFT;
      break;
    case KeyS:
      direction = BACKWARD;
      break;
    case KeyD:
      direction = RIGHT;
      break;
    case KeyE:
      direction = UP;
      break;
    case KeyQ:
      direction = DOWN;
      break;
    default:
      return;
  }
  if (kpd.action == KeyDown) {
    motion |= direction;
  } else if (kpd.action == KeyUp) {
    motion &= ~direction;
  }
}

void FreeCam::onTick(EventData *d) {
  TimerData td = d->TimerData;
  Vec3 motion_vec;
  Vec3 right = orientation.cross(implicit_up);
  bool net_motion = false;
  if (!motion) {
    return;
  }
  if (motion & FORWARD) {
    if (!(motion & BACKWARD)) {
      motion_vec += orientation;
      net_motion = true;
    }
  } else if (motion & BACKWARD) {
    if (!(motion & FORWARD)) {
      motion_vec -= orientation;
      net_motion = true;
    }
  }
  if (motion & RIGHT) {\
    if (!(motion & LEFT)) {
      motion_vec += right;
      net_motion = true;
    }
  } else if (motion & LEFT) {
    if (!(motion & RIGHT)) {
      motion_vec -= right;
      net_motion = true;
    }
  }
  if (motion & UP) {
    if (!(motion & DOWN)) {
      motion_vec += implicit_up;
      net_motion = true;
    }
  } else if (motion & DOWN) {
    if (!(motion & UP)) {
      motion_vec -= implicit_up;
      net_motion = true;
    }
  }
  
  if (!net_motion) { 
    return;
  }
  motion_vec.normalize();
  translateBy(td.dt * 50 * h_speed_modifier, motion_vec);
}

void FreeCam::rotateBy(const Mat4&) {}

void FreeCam::translateAndRotateBy(const Mat4&) {}

void FreeCam::setHorizontalRotationSpeed(double speed) {
  h_turn_speed = speed;
}

void FreeCam::setVerticalRotationSpeed(double speed) {
  v_turn_speed = speed;
}

void FreeCam::setTranslationSpeed(real_t speed) {
  h_speed = speed;
}

void FreeCam::setVerticalTranslationSpeed(real_t speed) {
  v_speed = speed;
}

const Vec3& FreeCam::getPosition() const {
  return position;
}

const Vec3& FreeCam::getOrientation() const {
  return orientation;
}

const Vec3& FreeCam::getUp() const {
  return implicit_up;
}

double FreeCam::getFoV() const {
  return fov;
}

double FreeCam::getAspectRatio() const {
  return (double)win->getWidth() / (double)win->getHeight();
}

double FreeCam::getAspectNum() const {
  return aspect_num;
}

double FreeCam::getAspectDenom() const {
  return aspect_denom;
}
