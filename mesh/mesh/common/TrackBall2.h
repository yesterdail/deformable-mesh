#ifndef HJ_TrackBall_h__
#define HJ_TrackBall_h__

#include "glmext.h"

namespace hj
{
  class TrackBall2
  {
  public:

    // map 2D screen point to unit sphere
    static glm::vec3 MapToSphere(const glm::vec2& point2d, const glm::ivec2& view_size);

    // get rotate matrix.
    static glm::mat4 RotateMatrix(const glm::vec2& old_point2d,
      const glm::vec2& new_point2d,
      const glm::ivec2& view_size,
      const glm::mat4& fix = glm::mat4(1));
  };
}
#endif // HJ_TrackBall_h__