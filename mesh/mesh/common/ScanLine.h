#ifndef HJ_ScanLine_h__
#define HJ_ScanLine_h__

#include <vector>
#include "common/glmext.h"

namespace hj
{
  class ScanLine
  {
  public:
    /**
    * Point-In-Polygon Algorithm - Determining Whether A Point Is Inside A Complex Polygon
    * The function will return YES if the point x,y is inside the polygon, or
    * NO if it is not.  If the point is exactly on the edge of the polygon,
    * then the function may return YES or NO.
    *
    * Note that division by zero is avoided because the division is protected
    * by the "if" clause which surrounds it.
    * 
    * @param polygon: coordinates of corners
    * @param point: point to be tested
    * @return: true if the point is inside the polygon, false otherwise.
    * http://alienryderflex.com/polygon/
    */
    static bool PointInPolygon(const std::vector<glm::vec2> &polygon, const glm::vec2 &point);

    /* Check whether segment P0P1 intersects with triangle t0t1t2 */
    static bool Intersecting(const glm::vec2 &p0, 
      const glm::vec2 &p1, 
      const glm::vec2 &t0, 
      const glm::vec2 &t1,
      const glm::vec2 &t2);

  private:
    /* Check whether P and Q lie on the same side of line AB */
    static float Side(const glm::vec2 &p,
      const glm::vec2 &q, 
      const glm::vec2 &a, 
      const glm::vec2 &b);
  };
}
#endif // HJ_ScanLine_h__