#ifndef HJ_MeshRenderer_h__
#define HJ_MeshRenderer_h__

#include "common/macro.h"
#include "common/glmext.h"
#include "common/TriMesh.h"

namespace hj
{
  class GLFramebuffer;
  class GLProgram;
  class Image;

  class MeshRenderer
  {
  public:
    /**
    * Default constructor
    */
    MeshRenderer();

    /**
    * Destructor.
    */
    ~MeshRenderer();

    /**
    * Sets pointer to input volume and view size
    * @param view_size: render view size.
    * @return: True if data are valid, false otherwise.
    */
    bool Initialize(const glm::ivec2& view_size);

    /**
    * Retrieves pixel data of an MPR view.
    * @param pixels: Pointer to the buffer receiving pixel data.
    * @param pixel_length: Size of pixels.
    */
    void GetPixel(uint8_t* pixels,
      uint32_t pixel_length,
      bool swap_channels);

    /**
    * Resizes output image.
    * @param view_size: render view size.
    * @return: True if data are valid, false otherwise.
    */
    bool ResizeOutput(const glm::ivec2 &new_size);

    /**
    * Loads a mesh.
    * @param file_paths: path of mesh.
    * @return: True if all parameters are valid, false otherwise.
    */
    bool LoadMesh(const std::string& filename);

    /**
    * Loads a texture.
    * @param file_paths: path of mesh.
    * @return: True if all parameters are valid, false otherwise.
    */
    bool LoadTexture(const std::string& filename);

    /**
    * Resets positions/directions of the camera.
    * @return: True if successful, false otherwise.
    */
    bool ResetCamera();

    /**
    * Rotates VR.
    * @param newMouseX: current mouse position X in view coordinate.
    * @param newMouseY: current mouse position Y in view coordinate.
    * @param lastMouseX: last mouse position X in view coordinate.
    * @param lastMouseY: last mouse position Y in view coordinate.
    */
    void Rotate(float newMouseX,
      float newMouseY,
      float lastMouseX,
      float lastMouseY);

    /**
    * Zoom VR.
    * @param newMouseX: current mouse position X in view coordinate.
    * @param newMouseY: current mouse position Y in view coordinate.
    * @param lastMouseX: last mouse position X in view coordinate.
    * @param lastMouseY: last mouse position Y in view coordinate.
    */
    void Zoom(float newMouseX,
      float newMouseY,
      float lastMouseX,
      float lastMouseY);

    /**
    * Move VR.
    * @param newMouseX: current mouse position X in view coordinate.
    * @param newMouseY: current mouse position Y in view coordinate.
    * @param lastMouseX: last mouse position X in view coordinate.
    * @param lastMouseY: last mouse position Y in view coordinate.
    */
    void Move(float newMouseX,
      float newMouseY,
      float lastMouseX,
      float lastMouseY);

    /**
    * Set smooth rendering.
    */
    void SetSmooth();

    /**
    * Set flat rendering.
    */
    void SetFlat();

    /**
    * Set wire frame rendering.
    */
    void SetWireframe(bool w);

    /**
    * Set solid rendering.
    */
    void SetSolid(bool s);

    /**
    * Set rendering with texture.
    */
    void SetTexture(bool t);

  private:

    /**
    * Paint.
    */
    bool Run();

    /**
    * draw the loaded mesh.
    */
    void drawMainObject(float r, float g, float b);

  private:
    /** Output frame buffer. */
    GLFramebuffer* out_fbo_ptr_;

    /** input mesh. */
    TriMesh* mesh_;

    /** mesh center in world coordinate. */
    Point center_;

    /** mesh radius in world coordinate. */
    float radius_;

    /** All vertices for drawing. */
    std::vector<unsigned int> triverts_;

    /** ROI vertices for drawing. */
    std::vector<unsigned int> roiverts_;

    /** model view transform matrix. */
    glm::mat4 modelview_;

    /** projection transform matrix. */
    glm::mat4 projection_;

    /** fov y in degree. */
    const float fovy_;

    /** wire frame rendering. */
    bool wireframe_;

    /** solid rendering. */
    bool solid_;

    /** rendering with texture. */
    bool texture_;

    Image* texture_image_;
  };
}
#endif // HJ_MeshRenderer_h__