#include "Trackball.h"
#include "Camera.h"

#include <glm/gtx/quaternion.hpp>

namespace hj
{
  Trackball::Trackball(PerspectiveCamera* camera_ptr)
    : camera_ptr_(camera_ptr)
    , size_(0.8f)
    , zoom_in_direction_(glm::vec2(0, 1))
    , move_center_(false)
  {
    center_ = camera_ptr->GetFocus();
  }

  Trackball::Trackball()
    : camera_ptr_(NULL)
    , size_(0.8f)
    , zoom_in_direction_(glm::vec2(0, 1))
    , move_center_(false)
  {
  }

  Trackball::~Trackball(void)
  {
  }

  PerspectiveCamera* Trackball::GetCamera() const
  {
    return camera_ptr_;
  }

  void Trackball::SetCamera(PerspectiveCamera* camera_ptr)
  {
    camera_ptr_ = camera_ptr;
    center_ = camera_ptr->GetFocus();
  }

  void Trackball::Rotate(const glm::quat &quat)
  {
    glm::vec3 position = camera_ptr_->GetPosition();
    position -= center_;
    position = glm::rotate(quat, position);
    position += center_;

    glm::vec3 focus = camera_ptr_->GetFocus();
    focus -= center_;
    // Usually focus - center == 0, so no need to rotate. But if we combine trackball
    // with some other navigations, this might be useful.
    focus = glm::rotate(quat, focus);
    focus += center_;

    glm::vec3 up_vector = camera_ptr_->GetUpVector();
    up_vector = glm::rotate(quat, up_vector);

    camera_ptr_->PositionCamera(position, focus, up_vector);
  }

  void Trackball::Rotate(const glm::vec3 &axis, float phi)
  {
    // use Camera2WorldTransform to get axis in world coordinates according to the axis given in camera coordinates
    Rotate(glm::angleAxis(phi, Camera2WorldTransform(glm::normalize(axis))));
  }

  void Trackball::Rotate(const glm::vec2& newMouse, const glm::vec2& lastMouse, int viewport_width, int viewport_height)
  {
    /* Project the points onto the virtual trackball,
     * then figure out the axis of rotation, which is the cross
     * product of P1-P2 and O-P1 (O is the center of the ball, 0,0,0)
     * Note:  This is a deformed trackball -- it is a trackball in the center,
     * but is deformed into a hyperbolic sheet of rotation away from the
     * center (projectToSphere does that job).  This particular function was
     * chosen after trying out several variations.
     *
     * It is assumed that the arguments to this routine are in the range
     * (-1.0 ... 1.0) */

    if(newMouse == lastMouse) {
      return;
    }

    // First, figure out z-coordinates for projection of P1 and P2 to deformed sphere
    glm::vec3 p1 = ProjectToSphere(ScaleMouse(lastMouse, viewport_width, viewport_height));
    glm::vec3 p2 = ProjectToSphere(ScaleMouse(newMouse, viewport_width, viewport_height));
    // Now, find the axis we are going to rotate about
    glm::vec3 axis = glm::cross(p2, p1);
    //... and calculate the angle phi between the two vectors which is the
    //  angle we need to rotate about
    glm::vec3 d = p1 - p2;
    float t = glm::length(d) / (2.0f * size_);
    // avoid problems with out-of-control values...
    if (t > 1.0f) t = 1.0f;
    if (t < -1.0f) t = -1.0f;
    float phi = 2.f * asin(t);
    Rotate(axis, phi);
  }

  glm::vec2 Trackball::ScaleMouse(const glm::vec2 &coords, int viewport_width, int viewport_height)
  {
    return glm::vec2(coords.x * 2 / viewport_width - 1,  coords.y * 2 / viewport_height - 1);
  }

  glm::vec3 Trackball::Camera2WorldTransform(const glm::vec3& axis) const
  {
    glm::mat4 view_matrix = camera_ptr_->GetViewMatrix();

    // need only the matrix that represents the current rotation of the camera-tripod.
    glm::mat4 rotation(0);
    rotation[0][0] = view_matrix[0][0]; rotation[0][1] = view_matrix[0][1]; rotation[0][2] = view_matrix[0][2];
    rotation[1][0] = view_matrix[1][0]; rotation[1][1] = view_matrix[1][1]; rotation[1][2] = view_matrix[1][2];
    rotation[2][0] = view_matrix[2][0]; rotation[2][1] = view_matrix[2][1]; rotation[2][2] = view_matrix[2][2];
    rotation[3][3] = 1;
    
    return glm::transformVector(glm::inverse(rotation), axis);
    //glm::dvec4 result = glm::inverse(rotation) * glm::dvec4(axis, 1);
    //result *= (1/result.w);
    //return glm::vec3(result);
  }

  glm::vec3 Trackball::ProjectToSphere(const glm::vec2& xy) const
  {
    static const float sqrt2 = sqrt(2.0f);
    glm::vec3 result;

    float d = glm::length(xy);
    if(d < size_ * sqrt2 / 2.0) {
      // Inside sphere
      // The factor "sqrt2/2.f" make a smooth changeover from sphere to hyperbola. If we leave
      // factor 1/sqrt(2) away, the trackball would bounce at the changeover.
      result.z = sqrt(size_ * size_ - d*d);
    }
    else {
      // On hyperbola
      float t = size_ / sqrt2;
      result.z = t*t / d;
    }
    result.x = xy.x;
    result.y = xy.y;
    return result;
  }

  void Trackball::Zoom(float factor)
  {
    // zoom factor is inverse proportional to scaling of the look vector, so invert:
    factor = 1.f / factor;
    //camera_ptr_->SetPosition((1-factor) * camera_ptr_->GetFocus() 
    //  + factor * camera_ptr_->GetPosition());
	camera_ptr_->SetFovy(factor*camera_ptr_->GetFovy());
  }

  void Trackball::Zoom(const glm::vec2& newMouse, const glm::vec2& lastMouse, int viewport_width, int viewport_height)
  {
    Zoom(1 + glm::dot(ScaleMouse(lastMouse, viewport_width, viewport_height) - 
      ScaleMouse(newMouse, viewport_width, viewport_height), zoom_in_direction_));
  }

  void Trackball::Move(float length, const glm::vec3 &axis)
  {
    if (length == 0.f || glm::length(axis) == 0.f)
      return;

    float frustFactor = camera_ptr_->GetFocalLength() / camera_ptr_->GetNearDist();
    float frustWidth = camera_ptr_->GetRight() - camera_ptr_->GetLeft();
    float frustHeight = camera_ptr_->GetTop() - camera_ptr_->GetBottom();
    glm::vec3 axis2 = glm::normalize(axis) * length * frustFactor;
    axis2.x *= frustWidth;
    axis2.y *= frustHeight;

    // find axis in world coordinates according to the axis given in camera coordinates
    axis2 = Camera2WorldTransform(axis2);

    MoveCamera(axis2);
    if(move_center_) {
      center_ -= axis2;
    }
  }

  void Trackball::Move(const glm::vec2& newMouse, const glm::vec2& lastMouse, int viewport_width, int viewport_height)
  {
    glm::vec2 mouse_motion = ScaleMouse(lastMouse, viewport_width, viewport_height) - 
      ScaleMouse(newMouse, viewport_width, viewport_height);
    glm::vec3 axis = glm::vec3(mouse_motion, 0);
    Move(glm::length(mouse_motion)*0.5f, axis);
  }

  void Trackball::MoveCamera(const glm::vec3& motionvector)
  {
    camera_ptr_->SetPosition(camera_ptr_->GetPosition() + motionvector);
    camera_ptr_->SetFocus(camera_ptr_->GetFocus() + motionvector);
  }

}
