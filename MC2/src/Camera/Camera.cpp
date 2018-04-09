#include "Math/matrix.hpp"
#include "Math/util.hpp"

#include "Camera/Camera.h"

const Mat4 Camera::projectionMatrix() {
  Mat4 projection;
  frustrum(
		getFoV() * 180 / PI,
    getAspectRatio(),
		0.1,
		300.0,
    projection
	);
  return projection;
}

const Mat4 Camera::modelViewMatrix() {
  Mat4 modelview;
  Vec3 cam_pos(getPosition());
	Vec3 look_at(getPosition() + getOrientation());
  Vec3 cam_up(getUp());
  
  lookAt(cam_pos, look_at, cam_up, modelview);

  return modelview;
}
