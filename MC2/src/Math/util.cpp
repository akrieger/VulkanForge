#include "Math/math.hpp"
#include "Math/matrix.hpp"
#include "Math/vector.hpp"

#include "Math/util.hpp"

void glhTranslate(Mat4& matrix, const Vec3& v) {
	matrix[3][0] = matrix[0][0]*v.x+matrix[1][0]*v.y+matrix[2][0]*v.z+matrix[3][0];
	matrix[3][1] = matrix[0][1]*v.x+matrix[1][1]*v.y+matrix[2][1]*v.z+matrix[3][1];
	matrix[3][2] = matrix[0][2]*v.x+matrix[1][2]*v.y+matrix[2][2]*v.z+matrix[3][2];
	matrix[3][3] = matrix[0][3]*v.x+matrix[1][3]*v.y+matrix[2][3]*v.z+matrix[3][3];
}

/**
 * Adapted from code in glh (http://sourceforge.net/projects/glhlib/) and
 * http://www.opengl.org/wiki/GluLookAt_code
 */
void lookAt(const Vec3& eye, const Vec3& target, const Vec3& up, Mat4& out) {

   //------------------
   Vec3 forward(target - eye);
   forward.normalize();
   //------------------
   //Side = forward x up
   Vec3 side(forward.cross(up));
   side.normalize();
   //------------------
   //Recompute up as: up = side x forward
   Vec3 up2(side.cross(forward));
   //------------------
   out(0,0) = side[0];
   out(1,0) = side[1];
   out(2,0) = side[2];
   out(3,0) = 0.0;
   //------------------
   out(0,1) = up2[0];
   out(1,1) = up2[1];
   out(2,1) = up2[2];
   out(3,1) = 0.0;
   //------------------
   out(0,2) = -forward[0];
   out(1,2) = -forward[1];
   out(2,2) = -forward[2];
   out(3,2) = 0.0;
   //------------------
   out(0,3) = out(1,3) = out(2,3) = 0.0;
   out(3,3) = 1.0;
   //------------------
   glhTranslate(out, eye * -1);
}

void glhFrustum(real_t left, real_t right, real_t bottom, real_t top,
                real_t znear, real_t zfar, Mat4& matrix)
{
    real_t temp, temp2, temp3, temp4;
    temp = 2.0 * znear;
    temp2 = right - left;
    temp3 = top - bottom;
    temp4 = zfar - znear;
    matrix[0][0] = temp / temp2;
    matrix[0][1] = 0.0;
    matrix[0][2] = 0.0;
    matrix[0][3] = 0.0;
    matrix[1][0] = 0.0;
    matrix[1][1] = temp / temp3;
    matrix[1][2] = 0.0;
    matrix[1][3] = 0.0;
    matrix[2][0] = (right + left) / temp2;
    matrix[2][1] = (top + bottom) / temp3;
    matrix[2][2] = (-zfar - znear) / temp4;
    matrix[2][3] = -1.0;
    matrix[3][0] = 0.0;
    matrix[3][1] = 0.0;
    matrix[3][2] = (-temp * zfar) / temp4;
    matrix[3][3] = 0.0;
}

/**
 * Code adapted from http://www.opengl.org/wiki/GluPerspective_code
 */
void frustrum(real_t fovyInDegrees, real_t aspectRatio,
              real_t znear, real_t zfar, Mat4& matrix)
{
    real_t ymax, xmax;
#if defined(REAL_T_FLOAT)
    ymax = znear * tanf(fovyInDegrees * PI / 360.0);
#elif defined(REAL_T_DOUBLE)
    ymax = znear * tan(fovyInDegrees * PI / 360.0);
#else
#error Must define REAL_T_FLOAT or REAL_T_DOUBLE
#endif
    xmax = ymax * aspectRatio;
    glhFrustum(-xmax, xmax, -ymax, ymax, znear, zfar, matrix);
}

void ortho(int w, int h, real_t near, real_t far, Mat4& matrix) {
  matrix[0][0] = 1.0 / w;
  matrix[1][1] = 1.0 / h;
  matrix[2][2] = -2.0 / (far - near);
  matrix[3][2] = - (far + near) / (far - near);
  matrix[3][3] = 1.0;
}