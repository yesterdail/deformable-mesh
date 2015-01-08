#ifndef HJ_CAMERA_H_
#define HJ_CAMERA_H_

#include "glmext.h"
#include "Frustum.h"

namespace hj
{
  class PerspectiveCamera
  {
  public:
    PerspectiveCamera();

    ~PerspectiveCamera();

    void Initialize(const glm::vec3 &position,
      const glm::vec3 &focus,
      const glm::vec3 &up,
      float fovy_deg,
      float ratio,
      float near,
      float far);

    void SetPosition(const glm::vec3 &position);

    void SetFocus(const glm::vec3 &focus);

    void SetUpVector(const glm::vec3 &up);

	void SetFovy(const float fovy);

    void PositionCamera(const glm::vec3 &pos, 
      const glm::vec3 &focus, 
      const glm::vec3 &up);

    glm::mat4 GetViewMatrix();

    glm::mat4 GetProjectionMatrix();

    glm::mat4 GetViewProjectionMatrix();

    glm::vec3 GetPosition() const { return position_; }
    glm::vec3 GetFocus() const { return focus_; }
    glm::vec3 GetUpVector() const { return up_vector_; }
    glm::vec3 GetLook() const { return glm::normalize(GetFocus() - GetPosition()); }
    float GetFovy() const { return frustum_.GetFovy(); }
    float GetRatio() const { return frustum_.GetRatio(); }
    float GetNearDist() const { return frustum_.GetNearDist(); }
    float GetFarDist() const { return frustum_.GetFarDist(); }
    float GetLeft() const { return frustum_.GetLeft(); }
    float GetRight() const { return frustum_.GetRight(); }
    float GetTop() const { return frustum_.GetTop(); }
    float GetBottom() const { return frustum_.GetBottom(); }
    float GetFocalLength() const { return glm::length(GetFocus() - GetPosition()); }

  private:
    /** location of the camera. */
    glm::vec3 position_;

    /** location, the camera looks at. */
    glm::vec3 focus_;

    /** up vector, always normalized. */
    glm::vec3 up_vector_;

    Frustum frustum_;

    glm::mat4 view_matrix_;
    glm::mat4 projection_matrix_;
    bool view_matrix_valid_;
    bool projection_matrix_valid_;

  };
}

#endif  // #ifndef HJ_CAMERA_H_
