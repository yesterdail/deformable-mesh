#include "ScanLine.h"

namespace hj
{


  bool ScanLine::PointInPolygon(const std::vector<glm::vec2> &polygon, const glm::vec2 &point)
  {
    int polyNumber = (int)polygon.size();
    int i, j = polyNumber - 1;
    bool oddNodes = false;
    for (i = 0; i < polyNumber; i++) {
      if ((polygon[i].y < point.y && polygon[j].y >= point.y 
        || polygon[j].y < point.y && polygon[i].y >= point.y) 
        && (polygon[i].x <= point.x || polygon[j].x <= point.x)) {
        oddNodes ^= (polygon[i].x + (point.y - polygon[i].y) / (polygon[j].y - polygon[i].y) 
          * (polygon[j].x - polygon[i].x) < point.x); 
      }
      j = i;
    }
    return oddNodes;
  }

  float ScanLine::Side(const glm::vec2 &p,
    const glm::vec2 &q, 
    const glm::vec2 &a, 
    const glm::vec2 &b)
  {
    float z1 = (b.x - a.x) * (p.y - a.y) - (p.x - a.x) * (b.y - a.y);
    float z2 = (b.x - a.x) * (q.y - a.y) - (q.x - a.x) * (b.y - a.y);
    return z1 * z2;
  }

  bool ScanLine::Intersecting(const glm::vec2 &p0,
    const glm::vec2 &p1,
    const glm::vec2 &t0,
    const glm::vec2 &t1,
    const glm::vec2 &t2)
  {
    /* Check whether segment is outside one of the three half-planes
    * delimited by the triangle. */
    float f1 = Side(p0, t2, t0, t1), f2 = Side(p1, t2, t0, t1);
    float f3 = Side(p0, t0, t1, t2), f4 = Side(p1, t0, t1, t2);
    float f5 = Side(p0, t1, t2, t0), f6 = Side(p1, t1, t2, t0);
    /* Check whether triangle is totally inside one of the two half-planes
    * delimited by the segment. */
    float f7 = Side(t0, t1, p0, p1);
    float f8 = Side(t1, t2, p0, p1);

    /* If segment is strictly outside triangle, or triangle is strictly
    * apart from the line, we're not intersecting */
    if ((f1 < 0 && f2 < 0) || (f3 < 0 && f4 < 0) || (f5 < 0 && f6 < 0)
      || (f7 > 0 && f8 > 0))
      return false; // NOT_INTERSECTING;

    /* If segment is aligned with one of the edges, we're overlapping */
    if ((f1 == 0 && f2 == 0) || (f3 == 0 && f4 == 0) || (f5 == 0 && f6 == 0))
      return true; // OVERLAPPING;

    /* If segment is outside but not strictly, or triangle is apart but
    * not strictly, we're touching */
    if ((f1 <= 0 && f2 <= 0) || (f3 <= 0 && f4 <= 0) || (f5 <= 0 && f6 <= 0)
      || (f7 >= 0 && f8 >= 0))
      return true; // TOUCHING;

    /* If both segment points are strictly inside the triangle, we
    * are not intersecting either */
    if (f1 > 0 && f2 > 0 && f3 > 0 && f4 > 0 && f5 > 0 && f6 > 0)
      return true;// NOT_INTERSECTING;

    /* Otherwise we're intersecting with at least one edge */
    return true;// INTERSECTING;
  }
}