#pragma once

#pragma unmanaged
#include "core/Manager.h"
#pragma managed

#include <msclr\marshal.h>

using namespace System;

namespace hj
{
  public ref class ManagerCLR
  {
  public:
    /**
    * Default constructor.
    */
    ManagerCLR(void);

    /**
    * Destructor.
    */
    ~ManagerCLR();

    /**
    * Dispose.
    */
    !ManagerCLR();

    /**
    * Loads a series of volume files and creates corresponding MPR and projection views.
    * @param viewWidth: Width of Rendering View.
    * @param viewHeight: Height of Rendering View.
    */
    bool CreateView(int viewWidth, int viewHeight);

    /**
    * Retrieves pixel data.
    * @param output_buffer: Pointer to the buffer receiving pixel data.
    * @return: Size of encoded data, -1 if any parameters are invalid.
    */
    void GetView(array<unsigned char>^ output_buffer);

    /**
    * Resizes output image.
    * @param new_width: Width of Rendering View.
    * @param new_height: Height of Rendering View.
    * @return: True if data are valid, false otherwise.
    */
    bool Resize(int new_width, int new_height);

    /**
    * Loads a mesh.
    * @param file_paths: path of mesh.
    * @return: True if all parameters are valid, false otherwise.
    */
    bool LoadMesh(String^ filename);

    /**
    * Loads a texture.
    * @param file_paths: path of mesh.
    * @return: True if all parameters are valid, false otherwise.
    */
    bool LoadTexture(String^ filename);

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

    /**
    * Set anchor points.
    * @param polygon: polygon points array.
    */
    void SetAnchorPoints(array<float>^ polyx,
      array<float>^ polyy,
      int count);

    /**
    * Set control points.
    * @param polygon: polygon points array.
    */
    void SetControlPoints(array<float>^ polyx,
      array<float>^ polyy,
      int count);

    /**
    * actions after selection.
    * @param point: mouse point.
    */
    bool PostSelection(float mouseX, float mouseY);

    /**
    * mesh deformation.
    * @param point: mouse point.
    */
    bool Deformation(float mouseX, float mouseY);

    /**
    * Cancel deformation.
    */
    void CancelDeform();

    /**
    * restore mesh to original.
    */
    void RestoreMesh();

    /**
    * Set line.
    * @param start: line start point.
    * @param end: line end point.
    */
    void SetLine(float startX,
      float startY,
      float endX,
      float endY);

    /**
    * Set Segment line depth.
    * @param d: depth.
    */
    void SetLineDepth(float d);

    /**
    * Get Segment line depth.
    */
    float GetLineDepth();

    /**
    * Create New graphics when mouse down.
    * @param x: mouse position X.
    * @param y: mouse position Y.
    */
    bool OnMouseDown_GraphicsOverlay(float x, float y);

    /**
    * resize new graphics when mouse move.
    * @param x: mouse position X.
    * @param y: mouse position Y.
    */
    bool OnMouseMove_GraphicsOverlay(float x, float y);

    /**
    * Release mouse.
    * @param x: mouse position X.
    * @param y: mouse position Y.
    */
    bool OnMouseUp_GraphicsOverlay(float x, float y);

    /**
    * Set current drawing graphics type.
    */
    void SetToolType(int type);

    /**
    * Get current drawing graphics type.
    */
    int GetToolType();

    /**
    * Remove all graphic objects.
    */
    void RemoveAllGraphics();

    /**
    * Remove selected graphic object.
    */
    void RemoveSelection();

  private:
    /** Pointer to a VisualizationManager on the native heap. */
    Manager* mgr_ptr_;
  };
}

