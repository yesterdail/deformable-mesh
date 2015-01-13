#ifndef HJ_Manager_h__
#define HJ_Manager_h__

#include <string>
#include "common/config.h"

namespace hj
{
  class GLOffScreenRender;
  class GLFramebuffer;
  class MeshRenderer;
  class GraphicsRenderer;

  class Manager
  {
  public:
    /**
    * Default constructor.
    */
    HJ_EXPORT Manager();

    /**
    * Destructor.
    */
    HJ_EXPORT ~Manager();

    /**
    * Loads a series of volume files and creates corresponding MPR and projection views.
    * @param viewWidth: Width of Rendering View.
    * @param viewHeight: Height of Rendering View.
    */
    HJ_EXPORT bool CreateView(int viewWidth, int viewHeight);

    /**
    * Retrieves pixel data.
    * @param output_buffer: Pointer to the buffer receiving pixel data.
    * @return: Size of encoded data, -1 if any parameters are invalid.
    */
    HJ_EXPORT const void GetView(uint8_t* output_buffer, int buffer_len);

    /**
    * Resizes output image.
    * @param new_width: Width of Rendering View.
    * @param new_height: Height of Rendering View.
    * @return: True if data are valid, false otherwise.
    */
    HJ_EXPORT bool Resize(int new_width, int new_height);

    /**
    * Loads a mesh.
    * @param file_paths: path of mesh.
    * @return: True if all parameters are valid, false otherwise.
    */
    HJ_EXPORT bool LoadMesh(const std::string& filename);

    /**
    * Loads a texture.
    * @param file_paths: path of mesh.
    * @return: True if all parameters are valid, false otherwise.
    */
    HJ_EXPORT bool LoadTexture(const std::string& filename);

    /**
    * Resets positions/directions of the camera.
    * @return: True if successful, false otherwise.
    */
    HJ_EXPORT bool ResetCamera();

    /**
    * Rotates VR.
    * @param newMouseX: current mouse position X in view coordinate.
    * @param newMouseY: current mouse position Y in view coordinate.
    * @param lastMouseX: last mouse position X in view coordinate.
    * @param lastMouseY: last mouse position Y in view coordinate.
    */
    HJ_EXPORT void Rotate(float newMouseX,
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
    HJ_EXPORT void Zoom(float newMouseX,
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
    HJ_EXPORT void Move(float newMouseX,
      float newMouseY,
      float lastMouseX,
      float lastMouseY);

    /**
    * Set smooth rendering.
    */
    HJ_EXPORT void SetSmooth();

    /**
    * Set flat rendering.
    */
    HJ_EXPORT void SetFlat();

    /**
    * Set wire frame rendering.
    */
    HJ_EXPORT void SetWireframe(bool w);

    /**
    * Set solid rendering.
    */
    HJ_EXPORT void SetSolid(bool s);

    /**
    * Set rendering with texture.
    */
    HJ_EXPORT void SetTexture(bool t);

    /**
    * Set anchor points.
    * @param polygon: polygon points array.
    */
    HJ_EXPORT void SetAnchorPoints(float *polyx,
      float *polyy,
      int count);

    /**
    * Set control points.
    * @param polygon: polygon points array.
    */
    HJ_EXPORT void SetControlPoints(float *polyx,
      float *polyy,
      int count);

    /**
    * actions after selection.
    * @param point: mouse point.
    */
    HJ_EXPORT bool PostSelection(float mouseX, float mouseY);

    /**
    * mesh deformation.
    * @param point: mouse point.
    */
    HJ_EXPORT bool Deformation(float mouseX, float mouseY);

    /**
    * Cancel deformation.
    */
    HJ_EXPORT void CancelDeform();

    /**
    * restore mesh to original.
    */
    HJ_EXPORT void RestoreMesh();

    /**
    * Cut mesh accordint to the line.
    */
    HJ_EXPORT void CutMesh();

    /**
    * Create New graphics when mouse down.
    * @param x: mouse position X.
    * @param y: mouse position Y.
    */
    HJ_EXPORT bool OnMouseDown_GraphicsOverlay(float x, float y);

    /**
    * resize new graphics when mouse move.
    * @param x: mouse position X.
    * @param y: mouse position Y.
    */
    HJ_EXPORT bool OnMouseMove_GraphicsOverlay(float x, float y);

    /**
    * Release mouse.
    * @param x: mouse position X.
    * @param y: mouse position Y.
    */
    HJ_EXPORT bool OnMouseUp_GraphicsOverlay(float x, float y);

    /**
    * Set current drawing graphics type.
    */
    HJ_EXPORT void SetToolType(int type);

    /**
    * Get current drawing graphics type.
    */
    HJ_EXPORT int GetToolType();

    /**
    * Remove all graphic objects.
    */
    HJ_EXPORT void RemoveAllGraphics();

    /**
    * Remove selected graphic object.
    */
    HJ_EXPORT void RemoveSelection();

    /**
    * Add a default cylinder at the center of (mouseX, mouseY).
    * @param mouseX: current mouse position X in view coordinate.
    * @param mouseY: current mouse position Y in view coordinate.
    */
    HJ_EXPORT void AddCylinder(float mouseX, float mouseY);

    /**
    * Check cylinder selection according to the mouse position.
    */
    HJ_EXPORT bool CheckSelection(float mouseX, float mouseY);

    /**
    * Get Selected cylinder properties, store as inner radius, outer radius, and height.
    * @return: true if any cylinder is selected.
    */
    HJ_EXPORT bool GetSelectionProperty(float properties[3]);

    /**
    * Set inner radius of current selected cylinder.
    */
    HJ_EXPORT void SetInnerRadius(float r);

    /**
    * Set outer radius of current selected cylinder.
    */
    HJ_EXPORT void SetOuterRadius(float r);

    /**
    * Set height of current selected cylinder.
    */
    HJ_EXPORT void SetHeight(float h);

  private:
    GLOffScreenRender* offscreen_render_ptr_;

    GLFramebuffer* out_fbo_ptr_;

    MeshRenderer* renderer_ptr_;

    GraphicsRenderer* graphics_renderer_ptr_;
  };
}
#endif // HJ_Manager_h__