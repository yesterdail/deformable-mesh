#include "Camera.h"

namespace hj
{
  PerspectiveCamera::PerspectiveCamera()
    : view_matrix_valid_(false)
    , projection_matrix_valid_(false)
    , frustum_(Frustum(45, 1, 0.1f, 50))
  {
    position_ = glm::vec3(0, 0, 0);
    focus_ = glm::vec3(0, 0, -1);
    up_vector_ = glm::vec3(0, 1, 0);
  }

  PerspectiveCamera::~PerspectiveCamera()
  {

  }

  void PerspectiveCamera::Initialize(const glm::vec3 &position, 
    const glm::vec3 &focus, 
    const glm::vec3 &up, 
    float fovy_deg, 
    float ratio, 
    float near, 
    float far)
  {
    position_ = position;
    focus_ = focus;
    up_vector_ = up;
    frustum_ = Frustum(fovy_deg, ratio, near, far);

    view_matrix_valid_ = false;
    projection_matrix_valid_ = false;
  }

  void PerspectiveCamera::SetPosition(const glm::vec3 &position)
  {
    position_ = position;
    view_matrix_valid_ = false;
  }

  void PerspectiveCamera::SetFocus(const glm::vec3 &focus)
  {
    focus_ = focus;
    view_matrix_valid_ = false;
  }

  void PerspectiveCamera::SetUpVector(const glm::vec3 &up)
  {
    up_vector_ = up;
    view_matrix_valid_ = false;
  }
  
  void PerspectiveCamera::SetFovy(float fovy)
  {
	  if (fovy < 1.0) fovy = 1.0;
	  if (fovy > 100.0) fovy = 100.0;
	  float ratio = frustum_.GetRatio();
	  frustum_.SetFovy(fovy);
	  frustum_.SetRatio(ratio);
	  projection_matrix_valid_ = false;
  }

  void PerspectiveCamera::PositionCamera(const glm::vec3 &pos, const glm::vec3 &focus, const glm::vec3 &up)
  {
    SetPosition(pos);
    SetFocus(focus);
    SetUpVector(up);
  }

  glm::mat4 PerspectiveCamera::GetViewMatrix()
  {
    if(!view_matrix_valid_) {
      view_matrix_ = glm::lookAt(position_, focus_, up_vector_);
      view_matrix_valid_ = true;
    }
    return view_matrix_;
  }

  glm::mat4 PerspectiveCamera::GetProjectionMatrix()
  {
    if(!projection_matrix_valid_) {
      float fov = frustum_.GetFovy();
      if(fov < 6.f) fov = 6.f;
      if(fov > 175.f) fov = 175.f;
      projection_matrix_ = glm::perspective(deg2rad(fov), frustum_.GetRatio(), 
        frustum_.GetNearDist(), frustum_.GetFarDist());
      projection_matrix_valid_ = true;
    }
    return projection_matrix_;
  }

  glm::mat4 PerspectiveCamera::GetViewProjectionMatrix()
  {
    return GetProjectionMatrix() * GetViewMatrix();
  }

}
