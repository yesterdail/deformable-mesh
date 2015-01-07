#ifndef HJ_Rect_h__
#define HJ_Rect_h__

#include "common/glmext.h"

namespace hj
{
  class Rect
  {
  public:
    double l, t, w, h;

    Rect();

    Rect(double l, double t, double w, double h);

    void Draw();

    void Fill();

    bool Contains(const glm::dvec2& point);

  };
}
#endif // HJ_Rect_h__