#include "TrackBall.h"

#include <glm/gtx/quaternion.hpp>

namespace hj
{
  const double TRACKBALL_RADIUS = 0.6;

  glm::dvec3 TrackBall::MapToSphere(const glm::dvec2& point2d, 
    const glm::ivec2& view_size)
  {
    glm::dvec3 result(0);

    // This is actually doing the Sphere/Hyperbolic sheet hybrid thing,
    // based on Ken Shoemake's ArcBall in Graphics Gems IV, 1993.
    double x = (2.0* point2d.x - view_size.x) / view_size.x;
    double y = -(2.0* point2d.y - view_size.y) / view_size.y;
    double xval = x;
    double yval = y;
    double x2y2 = xval*xval + yval*yval;

    const double rsqr = TRACKBALL_RADIUS * TRACKBALL_RADIUS;
    result.x = xval;
    result.y = yval;
    if (x2y2 < 0.5*rsqr) {
      result.z = sqrt(rsqr - x2y2);
    }
    else {
      result.z = 0.5*rsqr / sqrt(x2y2);
    }

    return result;
  }

  glm::dmat4 TrackBall::RotateMatrix(const glm::dvec2& old_point2d, 
    const glm::dvec2& new_point2d, 
    const glm::ivec2& view_size)
  {
    glm::dvec3 new_point3d = MapToSphere(glm::dvec2(new_point2d.x, view_size.y - new_point2d.y), view_size);
    glm::dvec3 old_point3d = MapToSphere(glm::dvec2(old_point2d.x, view_size.y - old_point2d.y), view_size);

    glm::dvec3 axis = glm::cross(old_point3d, new_point3d);
    if (glm::length(axis) < 1e-7) {
      axis = glm::dvec3(1, 0, 0);
    }
    else {
      axis = glm::normalize(axis);
    }
    glm::dvec3 d = old_point3d - new_point3d;
    double t = 0.5 * glm::length(d) / TRACKBALL_RADIUS;
    if (t < -1.0) t = -1.0;
    if (t > 1.0) t = 1.0;
    double phi = 2.0 * asin(t);
    return glm::rotate(phi, axis);
  }

}