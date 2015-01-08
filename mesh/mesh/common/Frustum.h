#ifndef Frustum_h__
#define Frustum_h__

#include "Macro.h"

namespace hj
{
  /**
  * This class is used by the Camera for culling in connection with axis aligned bounding boxes
  * or points.
  */
  class Frustum
  {
  public:
    /**
    * Constructor
    * @param fovy the field of view angle, in degrees, in the y direction.
    * @param ratio the field of view angle, in degrees, in the y direction. 
    * @param near_dist distance to near plane.
    * @param far_dist distance to far plane.
    */
    Frustum(float fovy, float ratio, float near_dist, float far_dist);

    /**
    * Constructor
    */
    Frustum(float left, float right, float bottom, float top, float near_dist, float far_dist);

    ~Frustum(void);

    bool operator==(const Frustum& rhs) const;

    /**
    * set the viewing angle in vertical direction in degrees.
    */
    void SetFovy(float fovy);

    /**
    * set width of frustum according to current height (or y viewing angle)
    */
    void SetRatio(float ratio);

    void SetNearDist(float near_dist);

    void SetFarDist(float far_dist);

    float GetFovy() const;
    float GetRatio() const;
    float GetNearDist() const;
    float GetFarDist() const;
    float GetLeft() const;
    float GetRight() const;
    float GetTop() const;
    float GetBottom() const;

  private:
    /** the distances from the center of the near plane to the left, right, top and bottom borders. */
    float lnear_;
    float rnear_;
    float bnear_;
    float tnear_;

    /** the distances from the position of the camera to the near- and far-plane. */
    float near_;
    float far_;
  };

}
#endif // Frustum_h__
