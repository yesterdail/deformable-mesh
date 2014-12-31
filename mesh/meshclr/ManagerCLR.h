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

  private:
    /** Pointer to a VisualizationManager on the native heap. */
    Manager* mgr_ptr_;
  };
}

