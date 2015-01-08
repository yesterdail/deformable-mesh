#include "Frustum.h"

#include <math.h>
#include <cassert>

namespace hj
{
  Frustum::Frustum(float fovy, float ratio, float near_dist, float far_dist)
    : near_(near_dist)
    , far_(far_dist)
  {
    SetFovy(fovy);
    SetRatio(ratio);
  }

  Frustum::Frustum(float left, float right, float bottom, float top, float near_dist, float far_dist)
    : lnear_(left)
    , rnear_(right)
    , bnear_(bottom)
    , tnear_(top)
    , near_(near_dist)
    , far_(far_dist)
  {
    assert(lnear_ == -rnear_ && bnear_ == -tnear_ && near_ > 0 && far_ > near_);
  }

  Frustum::~Frustum(void)
  {
  }

  bool Frustum::operator==(const Frustum& rhs) const
  {
    return (rhs.lnear_ == lnear_) && (rhs.rnear_ == rnear_) && (rhs.bnear_ == bnear_) && (rhs.tnear_ == tnear_) &&
      (rhs.near_ == near_) && (rhs.far_ == far_);
  }

  void Frustum::SetFovy(float fovy)
  {
    float half_height = tan(0.5f * deg2rad(fovy)) * near_;
    bnear_ = -half_height;
    tnear_ = half_height;
  }

  void Frustum::SetRatio(float ratio)
  {
    float half_width = 0.5f * (tnear_ - bnear_) * ratio;
    lnear_ = -half_width;
    rnear_ = half_width;
  }

  void Frustum::SetNearDist(float near_dist)
  {
    near_ = near_dist;
  }

  void Frustum::SetFarDist(float far_dist)
  {
    far_ = far_dist;
  }

  float Frustum::GetFovy() const
  {
    return rad2deg(2 * atan(tnear_/near_));
  }

  float Frustum::GetRatio() const
  {
    return (rnear_ - lnear_)/(tnear_ - bnear_);
  }

  float Frustum::GetNearDist() const
  {
    return near_;
  }

  float Frustum::GetFarDist() const
  {
    return far_;
  }

  float Frustum::GetLeft() const
  {
    return lnear_;
  }

  float Frustum::GetRight() const
  {
    return rnear_;
  }

  float Frustum::GetTop() const
  {
    return tnear_;
  }

  float Frustum::GetBottom() const
  {
    return bnear_;
  }

}
