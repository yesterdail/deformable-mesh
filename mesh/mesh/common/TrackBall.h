#ifndef HJ_TrackBall_h__
#define HJ_TrackBall_h__

#include "glmext.h"

namespace hj
{
  class TrackBall
  {
  public:

    // map 2D screen point to unit sphere
    static glm::dvec3 MapToSphere(const glm::dvec2& point2d, const glm::ivec2& view_size);

    // get rotate matrix.
    static glm::dmat4 RotateMatrix(const glm::dvec2& old_point2d,
      const glm::dvec2& new_point2d,
      const glm::ivec2& view_size);
  };
}
#endif // HJ_TrackBall_h__