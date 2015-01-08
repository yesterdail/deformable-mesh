#ifndef Trackball_h__
#define Trackball_h__

#include "glmext.h"

namespace hj
{
  class PerspectiveCamera;

  class Trackball
  {
  public:
    Trackball();

    Trackball(PerspectiveCamera* camera_ptr);

    ~Trackball(void);

    /** 
    * Returns the camera this trackball operates on.
    */
    PerspectiveCamera* GetCamera() const;

    /** 
    * Assigns the camera to be modified by the trackball.
    */
    void SetCamera(PerspectiveCamera* camera_ptr);

    /** 
    * Rotate the trackball according to new mouse position.
    * @param mouse coordinates of mouse in view port, scaled to [-1, 1]x[-1, 1]
    */
    void Rotate(const glm::vec2& newMouse, const glm::vec2& lastMouse, int viewport_width, int viewport_height);

    /** 
    * Zoom in according to new mouse position.
    * @param mouse coordinates of mouse in view port, scaled to [-1, 1]x[-1, 1]
    */
    void Zoom(const glm::vec2& newMouse, const glm::vec2& lastMouse, int viewport_width, int viewport_height);

    /** 
    * Move the trackball according to new mouse position.
    * @param mouse coordinates of mouse in view port, scaled to [-1, 1]x[-1, 1]
    */
    void Move(const glm::vec2& newMouse, const glm::vec2& lastMouse, int viewport_width, int viewport_height);

    /**
    * Rotate the trackball according to Quaternion quat.
    * @param quat Quaternion representation rotation.
    */
    void Rotate(const glm::quat &quat);

    /**
    * Rotate the trackball by angle phi around axis axis.
    * @param axis axis in camera coordinates.
    */
    void Rotate(const glm::vec3 &axis, float phi);

  private:
    /** 
    * transform vectors given in camera coordinates to vectors in world coordinates.
    */
    glm::vec3 Camera2WorldTransform(const glm::vec3& axis) const;

    /** 
    * projection math.
    * Project an x,y pair onto a sphere of radius r OR a hyperbolic sheet
    * if we are away from the center of the sphere.
    */
    glm::vec3 ProjectToSphere(const glm::vec2& xy) const;

    /** 
    * Scale mouse position to range (-1.0 ... 1.0).
    */
    glm::vec2 ScaleMouse(const glm::vec2 &coords, int viewport_width, int viewport_height);

    /** 
    * Zoom in by factor factor.
    */
    void Zoom(float factor);

    /** 
    * Move the trackball along axis.
    * @param length Distance relative to frustum dimensions at trackball center.
    *               Trackball center will get out of sight when >> 1.
    * @param axis Axis in camera coordinates along which to move.
    */
    void Move(float length, const glm::vec3 &axis);

    /**
    * Move the camera in a certain direction
    */
    void MoveCamera(const glm::vec3& motionvector);

  private:
    PerspectiveCamera* camera_ptr_;

    /** this holds the center around which the camera will be rotated. */
    glm::vec3 center_;

    /** the trackball size effects the trackball sensitivity. */
    float size_;

    /**
    * specifies in which direction mouse must be moved to zoom in.
    * The greater the length of this vector, the longer the way the mouse must be moved
    * to achieve a certain zoom factor.
    */
    glm::vec2 zoom_in_direction_;

    /**
    * whether to change the center in move method.
    * center_ will alway be (0,0,0) if false.
    */
    bool move_center_;
  };

}
#endif // Trackball_h__
