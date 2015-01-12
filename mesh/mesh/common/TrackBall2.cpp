#include "TrackBall2.h"

#include <glm/gtx/quaternion.hpp>

namespace hj
{
  const float TRACKBALL_RADIUS = 0.6f;

  glm::vec3 TrackBall2::MapToSphere(const glm::vec2& point2d,
    const glm::ivec2& view_size)
  {
    glm::vec3 result(0);

    // This is actually doing the Sphere/Hyperbolic sheet hybrid thing,
    // based on Ken Shoemake's ArcBall in Graphics Gems IV, 1993.
    float x = (2.0f* point2d.x - view_size.x) / view_size.x;
    float y = -(2.0f* point2d.y - view_size.y) / view_size.y;
    float xval = x;
    float yval = y;
    float x2y2 = xval*xval + yval*yval;

    const float rsqr = TRACKBALL_RADIUS * TRACKBALL_RADIUS;
    result.x = xval;
    result.y = yval;
    if (x2y2 < 0.5f*rsqr) {
      result.z = sqrt(rsqr - x2y2);
    }
    else {
      result.z = 0.5f*rsqr / sqrt(x2y2);
    }

    return result;
  }

  glm::mat4 TrackBall2::RotateMatrix(const glm::vec2& old_point2d,
    const glm::vec2& new_point2d, 
    const glm::ivec2& view_size,
    const glm::mat4& fix)
  {
    glm::vec3 new_point3d = MapToSphere(glm::vec2(new_point2d.x, view_size.y - new_point2d.y), view_size);
    glm::vec3 old_point3d = MapToSphere(glm::vec2(old_point2d.x, view_size.y - old_point2d.y), view_size);

    glm::vec3 axis = glm::cross(old_point3d, new_point3d);
    if (glm::length(axis) < 1e-7) {
      axis = glm::vec3(1, 0, 0);
    }
    else {
      axis = glm::normalize(axis);
    }
    glm::vec3 d = old_point3d - new_point3d;
    float t = 0.5f * glm::length(d) / TRACKBALL_RADIUS;
    if (t < -1.0f) t = -1.0f;
    if (t > 1.0f) t = 1.0f;
    float phi = 2.0f * asin(t);
    axis = glm::transformVector(fix, axis);
    return glm::rotate(phi, axis);
  }

}